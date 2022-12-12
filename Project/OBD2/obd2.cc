#include "obd2.h"

namespace Project {

	void OBD2::init() {
        auto cb = [](void* arg, CAN::Message& msg) {
            auto &queue = ((OBD2 *)arg)->rxQueue;
            if (queue) queue.clear();
            queue << msg;
        };
		can.init(txIdStd, false, cb, this);
        can.setFilter(rxIdStd, rxMaskStd);
		if (getSupported() == 0) return;

		// try extended id
        can.setIdType(true);
        can.setId(txIdExt);
        can.setFilter(rxIdExt, rxMaskExt);
        getSupported();
	}

	bool OBD2::isSupported(uint8_t pid) {
		if (pid == 0) return true;
		pid--;
		uint32_t index = pid / 32;
		return (supported[index] & (1ul << (31 - (pid % 32)))) != 0;
	}

	int OBD2::getSupported() {
		const uint8_t pids[] = { 
			PIDS_SUPPORT_01_20, 
			PIDS_SUPPORT_21_40, 
			PIDS_SUPPORT_41_60, 
			PIDS_SUPPORT_61_80, 
			PIDS_SUPPORT_81_A0, 
			PIDS_SUPPORT_A1_C0, 
		};
		for (int i = 0; i < 6; i++) {
			auto res = request(pids[i]);
			if (res.errorStr) return -1;
			supported[i] = res.raw;
			if ((supported[i] & 1ul) == 0ul) break;
		}
		return 0;
	}

	OBD2::Msg OBD2::request(uint8_t pid) {
		Msg msg = {};
		if (!isSupported(pid)) {
			msg.errorStr = errorCode[ERROR_NOT_SUPPORTED];
			return msg;
		}

        rxQueue.clear();
		uint8_t txBuffer[8] = {0x02, SHOW_CURRENT_DATA, pid, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
		can.transmit(txBuffer);

		// wait for response
		CAN::Message canMsg = {};
		auto res = rxQueue.pop(canMsg, waitResponseMs);
		if (res != osOK) {
			msg.errorStr = errorCode[-res - 1];
			return msg;
		}

		// get raw val
		auto rawLen = canMsg.data[0] - 2;
		auto &rawMode = canMsg.data[1];
		auto &rawPid = canMsg.data[2];
		auto *rawData = canMsg.data + 3;
		bool check = (SHOW_CURRENT_DATA | 0x40) == rawMode && pid == rawPid;
		if (!check) {
			msg.errorStr = errorCode[ERROR_CHECK];
			return msg;
		}

		// set raw
		msg.raw = *(uint32_t *) rawData;
		if (rawLen < 4) msg.raw = msg.raw >> (32 - (8 * rawLen));

		// set val
		switch (pid) {
			default: break;
			case RUN_TIME_SINCE_ENGINE_START:
			case DISTANCE_TRAVELED_WITH_MIL_ON:
			case DISTANCE_TRAVELED_SINCE_CODES_CLEARED:
			case TIME_RUN_WITH_MIL_ON:
			case TIME_SINCE_TROUBLE_CODES_CLEARED:
			case INTAKE_MANIFOLD_ABSOLUTE_PRESSURE:
			case VEHICLE_SPEED:
			case WARM_UPS_SINCE_CODES_CLEARED:
			case ABSOLUTE_BAROMETRIC_PRESSURE:
			case MAX_FUEL_AIR_EQUIV_RATIO:
				msg.val = float (msg.raw);
				break;
			case CALCULATED_ENGINE_LOAD:
			case THROTTLE_POSITION:
			case COMMANDED_EGR:
			case COMMANDED_EVAPORATIVE_PURGE:
			case FUEL_TANK_LEVEL_INPUT:
			case RELATIVE_THROTTLE_POSITION:
			case ABSOLUTE_THROTTLE_POSITION_B:
			case ABSOLUTE_THROTTLE_POSITION_C:
			case ABSOLUTE_THROTTLE_POSITION_D:
			case ABSOLUTE_THROTTLE_POSITION_E:
			case ABSOLUTE_THROTTLE_POSITION_F:
			case COMMANDED_THROTTLE_ACTUATOR:
			case ETHANOL_FUEL_PERCENTAGE:
			case RELATIVE_ACCELERATOR_PEDAL_POSITTION:
			case HYBRID_BATTERY_PACK_REMAINING_LIFE:
				msg.val = float(msg.raw) / 2.55f;
				break;
			case SHORT_TERM_FUEL_TRIM_BANK_1:
			case LONG_TERM_FUEL_TRIM_BANK_1:
			case SHORT_TERM_FUEL_TRIM_BANK_2:
			case LONG_TERM_FUEL_TRIM_BANK_2:
			case SHORT_TERM_OXYGEN_TRIM_BANK_1:
			case LONG_TERM_OXYGEN_TRIM_BANK_1:
			case SHORT_TERM_OXYGEN_TRIM_BANK_2:
			case LONG_TERM_OXYGEN_TRIM_BANK_2:
			case EGR_ERROR:
				msg.val = float(msg.raw) / 1.28f - 100.f;
				break;
			case OXYGEN_SENSOR_1_SHORT_TERM_FUEL_TRIM:
			case OXYGEN_SENSOR_2_SHORT_TERM_FUEL_TRIM:
			case OXYGEN_SENSOR_3_SHORT_TERM_FUEL_TRIM:
			case OXYGEN_SENSOR_4_SHORT_TERM_FUEL_TRIM:
			case OXYGEN_SENSOR_5_SHORT_TERM_FUEL_TRIM:
			case OXYGEN_SENSOR_6_SHORT_TERM_FUEL_TRIM:
			case OXYGEN_SENSOR_7_SHORT_TERM_FUEL_TRIM:
			case OXYGEN_SENSOR_8_SHORT_TERM_FUEL_TRIM:
			case ABSOLUTE_EVAP_SYSTEM_VAPOR_PRESSURE:
				msg.val = float(msg.raw) * 0.005f;
				break;
			case ENGINE_COOLANT_TEMPERATURE:
			case AIR_INTAKE_TEMPERATURE:
			case AMBIENT_AIR_TEMPERATURE:
			case ENGINE_OIL_TEMPERATURE:
				msg.val = float (msg.raw) - 40.f;
				break;
			case FUEL_PRESSURE:
				msg.val = float (msg.raw) * 3.f;
				break;
			case ENGINE_RPM:
				msg.val = float (msg.raw) / 4.f;
				break;
			case TIMING_ADVANCE:
				msg.val = float(msg.raw) / 2.0f - 64;
				break;
			case MAF_AIR_FLOW_RATE:
				msg.val = float (msg.raw) / 100.f;
				break;
			case FUEL_RAIL_PRESSURE:
				msg.val = float(msg.raw) * 0.079f;
				break;
			case FUEL_RAIL_GAUGE_PRESSURE:
			case FUEL_RAIL_ABSOLUTE_PRESSURE:
			case MAX_AIR_FLOW_RATE_FROM_MAF:
				msg.val = float (msg.raw) * 10.f;
				break;
			case OXYGEN_SENSOR_1_FUEL_AIR_EQUIVALENCE_RATIO_V:
			case OXYGEN_SENSOR_2_FUEL_AIR_EQUIVALENCE_RATIO_V:
			case OXYGEN_SENSOR_3_FUEL_AIR_EQUIVALENCE_RATIO_V:
			case OXYGEN_SENSOR_4_FUEL_AIR_EQUIVALENCE_RATIO_V:
			case OXYGEN_SENSOR_5_FUEL_AIR_EQUIVALENCE_RATIO_V:
			case OXYGEN_SENSOR_6_FUEL_AIR_EQUIVALENCE_RATIO_V:
			case OXYGEN_SENSOR_7_FUEL_AIR_EQUIVALENCE_RATIO_V:
			case OXYGEN_SENSOR_8_FUEL_AIR_EQUIVALENCE_RATIO_V:
			case OXYGEN_SENSOR_1_FUEL_AIR_EQUIVALENCE_RATIO:
			case OXYGEN_SENSOR_2_FUEL_AIR_EQUIVALENCE_RATIO:
			case OXYGEN_SENSOR_3_FUEL_AIR_EQUIVALENCE_RATIO:
			case OXYGEN_SENSOR_4_FUEL_AIR_EQUIVALENCE_RATIO:
			case OXYGEN_SENSOR_5_FUEL_AIR_EQUIVALENCE_RATIO:
			case OXYGEN_SENSOR_6_FUEL_AIR_EQUIVALENCE_RATIO:
			case OXYGEN_SENSOR_7_FUEL_AIR_EQUIVALENCE_RATIO:
			case OXYGEN_SENSOR_8_FUEL_AIR_EQUIVALENCE_RATIO:
			case FUEL_AIR_COMMANDED_EQUIVALENCE_RATE:
				msg.val = float(msg.raw) / 32768.f;
				break;
			case EVAP_SYSTEM_VAPOR_PRESSURE:
				msg.val = float(msg.raw) / 4.f;
				break;
			case CATALYST_TEMPERATURE_BANK_1_SENSOR_1:
			case CATALYST_TEMPERATURE_BANK_2_SENSOR_1:
			case CATALYST_TEMPERATURE_BANK_1_SENSOR_2:
			case CATALYST_TEMPERATURE_BANK_2_SENSOR_2:
				msg.val = float(msg.raw) / 10.f - 40;
				break;
			case CONTROL_MODULE_VOLTAGE:
				msg.val = float(msg.raw) / 1000.f;
				break;
			case ABSOLUTE_LOAD_VALUE:
				msg.val = float(msg.raw) / 2.55f;
				break;
			case EVAP_SYSTEM_VAPOR_PRESSURE2:
				msg.val = float (int32_t(msg.raw) - 32767) / 1000.f; // Pa to kPa
				break;
			case FUEL_INJECTION_TIMING:
				msg.val = float(msg.raw) / 128.f - 210.f;
				break;
			case ENGINE_FUEL_RATE:
				msg.val = float (msg.raw) / 20.f;
				break;
		}

		// set str
		switch (pid) {
			case FUEL_TYPE:
				if (msg.raw >= 24) msg.errorStr = errorCode[ERROR_INVALID_VALUE];
				msg.str = fuelTypes[msg.raw];
				break;
			case OBD_STANDARDS_THIS_VEHICLE_CONFORMS_TO:
				if (msg.raw > 33 || msg.raw == 0) msg.errorStr = errorCode[ERROR_INVALID_VALUE];
				msg.str = standards[msg.raw - 1];
				break;
			default:
				msg.str = nullptr;
				break;
		}
		return msg;
	}

	OBD2::Msg OBD2::requestVin(uint8_t vin, char *txt) {
		Msg msg = {};
		if (vin != VEHICLE_ID_NUMBER && vin != ECU_NAME) return msg;

        rxQueue.clear();
		uint8_t txBuffer[8] = {0x02, REQUEST_VEHICLE_INFORMATION, vin, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
		can.transmit(txBuffer);

		// wait for response
		CAN::Message canMsg = {};
		auto res = rxQueue.pop(canMsg, waitResponseMs);
		if (res != osOK) {
			msg.errorStr = errorCode[-res - 1];
			return msg;
		}

		// first chunk, read 3 bytes
		if (canMsg.data[0] != 0x10) {
			msg.errorStr = errorCode[4];
			return msg;
		}
		txt[0] = canMsg.data[5];
		txt[1] = canMsg.data[6];
		txt[2] = canMsg.data[7];

		// request more
		uint8_t txBuffer2[8] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		can.transmit(txBuffer2);

		uint8_t maxN = vin == VEHICLE_ID_NUMBER ? 17 : 20;
		for (uint8_t i = 1, n = 3; n < maxN; i++) {
			res = rxQueue.pop(canMsg, waitResponseMs);
			if (res != osOK || canMsg.data[0] != (0x20 + i)) return msg;
			for (int j = 1; j < 8 && n < maxN; j++, n++)
				txt[n] = canMsg.data[j];
		}
		return msg;
	}


	const char* const OBD2::pidNames[] = {
		/* 0x00 */ "PIDs supported [01 - 20]",
		/* 0x01 */ "Monitor status since DTCs cleared",
		/* 0x02 */ "Freeze DTC",
		/* 0x03 */ "Fuel system status",
		/* 0x04 */ "Calculated engine load",
		/* 0x05 */ "Engine coolant temperature",
		/* 0x06 */ "Short term fuel trim — Bank 1",
		/* 0x07 */ "Long term fuel trim — Bank 1",
		/* 0x08 */ "Short term fuel trim — Bank 2",
		/* 0x09 */ "Long term fuel trim — Bank 2",
		/* 0x0a */ "Fuel pressure",
		/* 0x0b */ "Intake manifold absolute pressure",
		/* 0x0c */ "Engine RPM",
		/* 0x0d */ "Vehicle speed",
		/* 0x0e */ "Timing advance",
		/* 0x0f */ "Intake air temperature",
		/* 0x10 */ "MAF air flow rate",
		/* 0x11 */ "Throttle position",
		/* 0x12 */ "Commanded secondary air status",
		/* 0x13 */ "Oxygen sensors present (in 2 banks)",
		/* 0x14 */ "Oxygen Sensor 1 - Short term fuel trim",
		/* 0x15 */ "Oxygen Sensor 2 - Short term fuel trim",
		/* 0x16 */ "Oxygen Sensor 3 - Short term fuel trim",
		/* 0x17 */ "Oxygen Sensor 4 - Short term fuel trim",
		/* 0x18 */ "Oxygen Sensor 5 - Short term fuel trim",
		/* 0x19 */ "Oxygen Sensor 6 - Short term fuel trim",
		/* 0x1a */ "Oxygen Sensor 7 - Short term fuel trim",
		/* 0x1b */ "Oxygen Sensor 8 - Short term fuel trim",
		/* 0x1c */ "OBD standards this vehicle conforms to",
		/* 0x1d */ "Oxygen sensors present (in 4 banks)",
		/* 0x1e */ "Auxiliary input status",
		/* 0x1f */ "Run time since engine start",
		/* 0x20 */ "PIDs supported [21 - 40]",
		/* 0x21 */ "Distance traveled with Malfunction Indicator Lamp (MIL) on",
		/* 0x22 */ "Fuel Rail Pressure (relative to manifold vacuum)",
		/* 0x23 */ "Fuel Rail Gauge Pressure (diesel, or gasoline direct injection)",
		/* 0x24 */ "Oxygen Sensor 1 - Fuel-Air Equivalence Ratio",
		/* 0x25 */ "Oxygen Sensor 2 - Fuel-Air Equivalence Ratio",
		/* 0x26 */ "Oxygen Sensor 3 - Fuel-Air Equivalence Ratio",
		/* 0x27 */ "Oxygen Sensor 4 - Fuel-Air Equivalence Ratio",
		/* 0x28 */ "Oxygen Sensor 5 - Fuel-Air Equivalence Ratio",
		/* 0x29 */ "Oxygen Sensor 6 - Fuel-Air Equivalence Ratio",
		/* 0x2a */ "Oxygen Sensor 7 - Fuel-Air Equivalence Ratio",
		/* 0x2b */ "Oxygen Sensor 8 - Fuel-Air Equivalence Ratio",
		/* 0x2c */ "Commanded EGR",
		/* 0x2d */ "EGR Error",
		/* 0x2e */ "Commanded evaporative purge",
		/* 0x2f */ "Fuel Tank Level Input",
		/* 0x30 */ "Warm-ups since codes cleared",
		/* 0x31 */ "Distance traveled since codes cleared",
		/* 0x32 */ "Evap. System Vapor Pressure",
		/* 0x33 */ "Absolute Barometric Pressure",
		/* 0x34 */ "Oxygen Sensor 1 - Fuel-Air Equivalence Ratio",
		/* 0x35 */ "Oxygen Sensor 2 - Fuel-Air Equivalence Ratio",
		/* 0x36 */ "Oxygen Sensor 3 - Fuel-Air Equivalence Ratio",
		/* 0x37 */ "Oxygen Sensor 4 - Fuel-Air Equivalence Ratio",
		/* 0x38 */ "Oxygen Sensor 5 - Fuel-Air Equivalence Ratio",
		/* 0x39 */ "Oxygen Sensor 6 - Fuel-Air Equivalence Ratio",
		/* 0x3a */ "Oxygen Sensor 7 - Fuel-Air Equivalence Ratio",
		/* 0x3b */ "Oxygen Sensor 8 - Fuel-Air Equivalence Ratio",
		/* 0x3c */ "Catalyst Temperature: Bank 1, Sensor 1",
		/* 0x3d */ "Catalyst Temperature: Bank 2, Sensor 1",
		/* 0x3e */ "Catalyst Temperature: Bank 1, Sensor 2",
		/* 0x3f */ "Catalyst Temperature: Bank 2, Sensor 2",
		/* 0x40 */ "PIDs supported [41 - 60]",
		/* 0x41 */ "Monitor status this drive cycle",
		/* 0x42 */ "Control module voltage",
		/* 0x43 */ "Absolute load value",
		/* 0x44 */ "Fuel-Air commanded equivalence ratio",
		/* 0x45 */ "Relative throttle position",
		/* 0x46 */ "Ambient air temperature",
		/* 0x47 */ "Absolute throttle position B",
		/* 0x48 */ "Absolute throttle position C",
		/* 0x49 */ "Absolute throttle position D",
		/* 0x4a */ "Absolute throttle position E",
		/* 0x4b */ "Absolute throttle position F",
		/* 0x4c */ "Commanded throttle actuator",
		/* 0x4d */ "Time run with MIL on",
		/* 0x4e */ "Time since trouble codes cleared",
		/* 0x4f */ "Maximum fuel-air equivalence ratio",
		/* 0x50 */ "Maximum air flow rate from Mass Air Flow sensor",
		/* 0x51 */ "Fuel Type",
		/* 0x52 */ "Ethanol fuel percentage",
		/* 0x53 */ "Absolute Evap system Vapor Pressure",
		/* 0x54 */ "Evap system vapor pressure",
		/* 0x55 */ "Short term secondary oxygen sensor trim",
		/* 0x56 */ "Long term secondary oxygen sensor trim",
		/* 0x57 */ "Short term secondary oxygen sensor trim",
		/* 0x58 */ "Long term secondary oxygen sensor trim",
		/* 0x59 */ "Fuel rail absolute pressure",
		/* 0x5a */ "Relative accelerator pedal position",
		/* 0x5b */ "Hybrid battery pack remaining life",
		/* 0x5c */ "Engine oil temperature",
		/* 0x5d */ "Fuel injection timing",
		/* 0x5e */ "Engine fuel rate",
		/* 0x5f */ "Emission requirements to which vehicle is designed",
	};

	const char* const OBD2::pidUnits[]  = {
		/* 0x00 */ nullptr,
		/* 0x01 */ nullptr,
		/* 0x02 */ nullptr,
		/* 0x03 */ nullptr,
		/* 0x04 */ "%",
		/* 0x05 */ "°C",
		/* 0x06 */ "%",
		/* 0x07 */ "%",
		/* 0x08 */ "%",
		/* 0x09 */ "%",
		/* 0x0a */ "kPa",
		/* 0x0b */ "kPa",
		/* 0x0c */ "rpm",
		/* 0x0d */ "km/h",
		/* 0x0e */ "° before TDC",
		/* 0x0f */ "°C",
		/* 0x10 */ "grams/sec",
		/* 0x11 */ "%",
		/* 0x12 */ nullptr,
		/* 0x13 */ nullptr,
		/* 0x14 */ "V",
		/* 0x15 */ "V",
		/* 0x16 */ "V",
		/* 0x17 */ "V",
		/* 0x18 */ "V",
		/* 0x19 */ "V",
		/* 0x1a */ "V",
		/* 0x1b */ "V",
		/* 0x1c */ nullptr,
		/* 0x1d */ nullptr,
		/* 0x1e */ nullptr,
		/* 0x1f */ "seconds",
		/* 0x20 */ nullptr,
		/* 0x21 */ "km",
		/* 0x22 */ "kPa",
		/* 0x23 */ "kPa",
		/* 0x24 */ "ratio",
		/* 0x25 */ "ratio",
		/* 0x26 */ "ratio",
		/* 0x27 */ "ratio",
		/* 0x28 */ "ratio",
		/* 0x29 */ "ratio",
		/* 0x2a */ "ratio",
		/* 0x2b */ "ratio",
		/* 0x2c */ "%",
		/* 0x2d */ "%",
		/* 0x2e */ "%",
		/* 0x2f */ "%",
		/* 0x30 */ "count",
		/* 0x31 */ "km",
		/* 0x32 */ "Pa",
		/* 0x33 */ "kPa",
		/* 0x34 */ "ratio",
		/* 0x35 */ "ratio",
		/* 0x36 */ "ratio",
		/* 0x37 */ "ratio",
		/* 0x38 */ "ratio",
		/* 0x39 */ "ratio",
		/* 0x3a */ "ratio",
		/* 0x3b */ "ratio",
		/* 0x3c */ "°C",
		/* 0x3d */ "°C",
		/* 0x3e */ "°C",
		/* 0x3f */ "°C",
		/* 0x40 */ nullptr,
		/* 0x41 */ nullptr,
		/* 0x42 */ "V",
		/* 0x43 */ "%",
		/* 0x44 */ "ratio",
		/* 0x45 */ "%",
		/* 0x46 */ "°C",
		/* 0x47 */ "%",
		/* 0x48 */ "%",
		/* 0x49 */ "%",
		/* 0x4a */ "%",
		/* 0x4b */ "%",
		/* 0x4c */ "%",
		/* 0x4d */ "minutes",
		/* 0x4e */ "minutes",
		/* 0x4f */ nullptr,
		/* 0x50 */ "g/s",
		/* 0x51 */ nullptr,
		/* 0x52 */ "%",
		/* 0x53 */ "kPa",
		/* 0x54 */ "kPa",
		/* 0x55 */ "%",
		/* 0x56 */ "%",
		/* 0x57 */ "%",
		/* 0x58 */ "%",
		/* 0x59 */ "kPa",
		/* 0x5a */ "%",
		/* 0x5b */ "%",
		/* 0x5c */ "°C",
		/* 0x5d */ "°",
		/* 0x5e */ "L/h",
		/* 0x5f */ nullptr,
	};

	const char* const OBD2::fuelTypes[24] = {
		"Not available",
		"Gasoline",
		"Methanol",
		"Ethanol",
		"Diesel",
		"LPG",
		"CNG",
		"Propane",
		"Electric",
		"Bifuel running Gasoline",
		"Bifuel running Methanol",
		"Bifuel running Ethanol",
		"Bifuel running LPG",
		"Bifuel running CNG",
		"Bifuel running Propane",
		"Bifuel running Electricity",
		"Bifuel running electric and combustion engine",
		"Hybrid gasoline",
		"Hybrid Ethanol",
		"Hybrid Diesel",
		"Hybrid Electric",
		"Hybrid running electric and combustion engine",
		"Hybrid Regenerative",
		"Bifuel running diesel", 
	};

	const char* const OBD2::standards[33] = {
		"OBD-II as defined by the CARB",
		"OBD as defined by the EPA",
		"OBD and OBD-II",
		"OBD-I",
		"Not OBD compliant",
		"EOBD (Europe)",
		"EOBD and OBD-II",
		"EOBD and OBD",
		"EOBD, OBD and OBD II",
		"JOBD (Japan)",
		"JOBD and OBD II",
		"JOBD and EOBD",
		"JOBD, EOBD, and OBD II",
		"Reserved",
		"Reserved",
		"Reserved",
		"Engine Manufacturer Diagnostics (EMD)",
		"Engine Manufacturer Diagnostics Enhanced (EMD+)",
		"Heavy Duty On-Board Diagnostics (Child/Partial) (HD OBD-C)",
		"Heavy Duty On-Board Diagnostics (HD OBD)",
		"World Wide Harmonized OBD (WWH OBD)",
		"Reserved",
		"Heavy Duty Euro OBD Stage I without NOx control (HD EOBD-I)",
		"Heavy Duty Euro OBD Stage I with NOx control (HD EOBD-I N)",
		"Heavy Duty Euro OBD Stage II without NOx control (HD EOBD-II)",
		"Heavy Duty Euro OBD Stage II with NOx control (HD EOBD-II N)",
		"Reserved",
		"Brazil OBD Phase 1 (OBDBr-1)",
		"Brazil OBD Phase 2 (OBDBr-2)",
		"Korean OBD (KOBD)",
		"India OBD I (IOBD I)",
		"India OBD II (IOBD II)",
		"Heavy Duty Euro OBD Stage VI (HD EOBD-IV)", 
	};

	const char* const OBD2::errorCode[6] = {
		/* -1 */ "Error Check",
		/* -2 */ "Error Timeout",
		/* -3 */ "Error Resource",
		/* -4 */ "Error Parameter",
		/* -5 */ "Invalid value",
		/* -6 */ "Not supported",
	};

		
} // namespace Project
