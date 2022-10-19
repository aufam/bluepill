#ifndef PROJECT_OBD2_H
#define PROJECT_OBD2_H

#include "periph/can.h"
#include "periph/uart.h"
#include "periph/usb.h"
#include "os.h"

namespace Project {

    struct OBD2 {
        struct Msg {
            uint32_t raw;
            union { 
                int32_t val; ///< actual value with 2 decimal points
                uint32_t u; ///< unsigned version of val, if u == 0x80000000 -> val is invalid
            };
            const char *str;
            const char *errorStr;
        };
        using CAN = Periph::CAN;

        inline static const uint32_t txIdStd      = 0x7DFu;
        inline static const uint32_t txIdExt      = 0x18DB33F1u;
        inline static const uint32_t rxIdStd      = 0x7EFu;
        inline static const uint32_t rxMaskStd    = 0x7E8u;
        inline static const uint32_t rxIdExt      = 0x18DAF11Fu;
        inline static const uint32_t rxMaskExt    = 0x18DAF110u;
        inline static const uint32_t nPid         = 0x60u; ///< number of available parameter IDs
        inline static const uint32_t waitResponseMs = 100; ///< maximum wait for response in ms
        static const char* const pidNames[nPid];
        static const char* const pidUnits[nPid];
        static const char* const fuelTypes[24];
        static const char* const standards[33];
        static const char* const errorCode[6];

        CAN &can; ///< reference to CAN peripheral
        uint32_t supported[6]; ///< every bit represents if a parameter is supported
        constexpr explicit OBD2(Periph::CAN &can) : can(can), supported{} {}

        void init(); ///< CAN init and update supported buffer
        /// get list of parameter and store to supported buffer
        /// @retval 0 success, -1 fail
        int getSupported();
        bool isSupported(uint8_t pid);
        /// request current parameter
        /// @param pid parameter ID, see PID
        /// @retval OBD2 message
        Msg request(uint8_t pid);
        /// request vehicle info and print to txt
        /// @param vin vehicle info, see VehicleInfo
        /// @param txt buffer to store the result
        /// @retval OBD2 message
        Msg requestVin(uint8_t vin, char *txt);

        enum Mode {
            SHOW_CURRENT_DATA = 1,
            SHOW_FREEZE_FRAME_DATA,
            SHOW_STORED_DIAGNOSTIC_TROUBLE_CODES,
            CLEAR_DIAGNOSTIC_TROUBLE_CODES,
            TEST_RESULTS_OXYGEN_SENSOR,
            TEST_RESULTS_OTHER_COMPONENTS,
            SHOW_PENDING_DIAGNOSTIC_TROUBLE_CODES,
            CONTROL_OPERATION_OF_ONBOARD_COMPONENT,
            REQUEST_VEHICLE_INFORMATION,
            PERMANENT_DIAGNOSTIC_TROUBLE_CODES
        };

        enum FuelType {
            NOT_AVAILABLE,
            GASOLINE,
            METHANOL,
            ETHANOL,
            DIESEL,
            LPG,
            CNG,
            PROPANE,
            ELECTRIC,
            BIFUEL_RUNNING_GASOLINE,
            BIFUEL_RUNNING_METHANOL,
            BIFUEL_RUNNING_ETHANOL,
            BIFUEL_RUNNING_LPG,
            BIFUEL_RUNNING_CNG,
            BIFUEL_RUNNING_PROPANE,
            BIFUEL_RUNNING_ELECTRICITY,
            BIFUEL_RUNNING_ELECTRIC_AND_COMBUSTION_ENGINE,
            HYBRID_GASOLINE,
            HYBRID_ETHANOL,
            HYBRID_DIESEL,
            HYBRID_ELECTRIC,
            HYBRID_RUNNING_ELECTRIC_AND_COMBUSTION_ENGINE,
            HYBRID_REGENERATIVE,
            BIFUEL_RUNNING_DIESEL, 
        };

        enum PID {
            PIDS_SUPPORT_01_20                                = 0x00,
            MONITOR_STATUS_SINCE_DTCS_CLEARED                 = 0x01,
            FREEZE_DTC                                        = 0x02,
            FUEL_SYSTEM_STATUS                                = 0x03,
            CALCULATED_ENGINE_LOAD                            = 0x04,
            ENGINE_COOLANT_TEMPERATURE                        = 0x05,
            SHORT_TERM_FUEL_TRIM_BANK_1                       = 0x06,
            LONG_TERM_FUEL_TRIM_BANK_1                        = 0x07,
            SHORT_TERM_FUEL_TRIM_BANK_2                       = 0x08,
            LONG_TERM_FUEL_TRIM_BANK_2                        = 0x09,
            FUEL_PRESSURE                                     = 0x0a,
            INTAKE_MANIFOLD_ABSOLUTE_PRESSURE                 = 0x0b,
            ENGINE_RPM                                        = 0x0c,
            VEHICLE_SPEED                                     = 0x0d,
            TIMING_ADVANCE                                    = 0x0e,
            AIR_INTAKE_TEMPERATURE                            = 0x0f,
            MAF_AIR_FLOW_RATE                                 = 0x10,
            THROTTLE_POSITION                                 = 0x11,
            COMMANDED_SECONDARY_AIR_STATUS                    = 0x12,
            OXYGEN_SENSORS_PRESENT_IN_2_BANKS                 = 0x13,
            OXYGEN_SENSOR_1_SHORT_TERM_FUEL_TRIM              = 0x14,
            OXYGEN_SENSOR_2_SHORT_TERM_FUEL_TRIM              = 0x15,
            OXYGEN_SENSOR_3_SHORT_TERM_FUEL_TRIM              = 0x16,
            OXYGEN_SENSOR_4_SHORT_TERM_FUEL_TRIM              = 0x17,
            OXYGEN_SENSOR_5_SHORT_TERM_FUEL_TRIM              = 0x18,
            OXYGEN_SENSOR_6_SHORT_TERM_FUEL_TRIM              = 0x19,
            OXYGEN_SENSOR_7_SHORT_TERM_FUEL_TRIM              = 0x1a,
            OXYGEN_SENSOR_8_SHORT_TERM_FUEL_TRIM              = 0x1b,
            OBD_STANDARDS_THIS_VEHICLE_CONFORMS_TO            = 0x1c,
            OXYGEN_SENSORS_PRESENT_IN_4_BANKS                 = 0x1d,
            AUXILIARY_INPUT_STATUS                            = 0x1e,
            RUN_TIME_SINCE_ENGINE_START                       = 0x1f,

            PIDS_SUPPORT_21_40                                = 0x20,
            DISTANCE_TRAVELED_WITH_MIL_ON                     = 0x21,
            FUEL_RAIL_PRESSURE                                = 0x22,
            FUEL_RAIL_GAUGE_PRESSURE                          = 0x23,
            OXYGEN_SENSOR_1_FUEL_AIR_EQUIVALENCE_RATIO_V      = 0x24,
            OXYGEN_SENSOR_2_FUEL_AIR_EQUIVALENCE_RATIO_V      = 0x25,
            OXYGEN_SENSOR_3_FUEL_AIR_EQUIVALENCE_RATIO_V      = 0x26,
            OXYGEN_SENSOR_4_FUEL_AIR_EQUIVALENCE_RATIO_V      = 0x27,
            OXYGEN_SENSOR_5_FUEL_AIR_EQUIVALENCE_RATIO_V      = 0x28,
            OXYGEN_SENSOR_6_FUEL_AIR_EQUIVALENCE_RATIO_V      = 0x29,
            OXYGEN_SENSOR_7_FUEL_AIR_EQUIVALENCE_RATIO_V      = 0x2a,
            OXYGEN_SENSOR_8_FUEL_AIR_EQUIVALENCE_RATIO_V      = 0x2b,
            COMMANDED_EGR                                     = 0x2c,
            EGR_ERROR                                         = 0x2d,
            COMMANDED_EVAPORATIVE_PURGE                       = 0x2e,
            FUEL_TANK_LEVEL_INPUT                             = 0x2f,
            WARM_UPS_SINCE_CODES_CLEARED                      = 0x30,
            DISTANCE_TRAVELED_SINCE_CODES_CLEARED             = 0x31,
            EVAP_SYSTEM_VAPOR_PRESSURE                        = 0x32,
            ABSOLULTE_BAROMETRIC_PRESSURE                     = 0x33,
            OXYGEN_SENSOR_1_FUEL_AIR_EQUIVALENCE_RATIO        = 0x34,
            OXYGEN_SENSOR_2_FUEL_AIR_EQUIVALENCE_RATIO        = 0x35,
            OXYGEN_SENSOR_3_FUEL_AIR_EQUIVALENCE_RATIO        = 0x36,
            OXYGEN_SENSOR_4_FUEL_AIR_EQUIVALENCE_RATIO        = 0x37,
            OXYGEN_SENSOR_5_FUEL_AIR_EQUIVALENCE_RATIO        = 0x38,
            OXYGEN_SENSOR_6_FUEL_AIR_EQUIVALENCE_RATIO        = 0x39,
            OXYGEN_SENSOR_7_FUEL_AIR_EQUIVALENCE_RATIO        = 0x3a,
            OXYGEN_SENSOR_8_FUEL_AIR_EQUIVALENCE_RATIO        = 0x3b,
            CATALYST_TEMPERATURE_BANK_1_SENSOR_1              = 0x3c,
            CATALYST_TEMPERATURE_BANK_2_SENSOR_1              = 0x3d,
            CATALYST_TEMPERATURE_BANK_1_SENSOR_2              = 0x3e,
            CATALYST_TEMPERATURE_BANK_2_SENSOR_2              = 0x3f,

            PIDS_SUPPORT_41_60                                = 0x40,
            MONITOR_STATUS_THIS_DRIVE_CYCLE                   = 0x41,
            CONTROL_MODULE_VOLTAGE                            = 0x42,
            ABSOLUTE_LOAD_VALUE                               = 0x43,
            FUEL_AIR_COMMANDED_EQUIVALENCE_RATE               = 0x44,
            RELATIVE_THROTTLE_POSITION                        = 0x45,
            AMBIENT_AIR_TEMPERATURE                           = 0x46,
            ABSOLUTE_THROTTLE_POSITION_B                      = 0x47,
            ABSOLUTE_THROTTLE_POSITION_C                      = 0x48,
            ABSOLUTE_THROTTLE_POSITION_D                      = 0x49,
            ABSOLUTE_THROTTLE_POSITION_E                      = 0x4a,
            ABSOLUTE_THROTTLE_POSITION_F                      = 0x4b,
            COMMANDED_THROTTLE_ACTUATOR                       = 0x4c,
            TIME_RUN_WITH_MIL_ON                              = 0x4d,
            TIME_SINCE_TROUBLE_CODES_CLEARED                  = 0x4e,
            MAX_FUEL_AIR_EQUIV_RATIO                          = 0x4f,
            MAX_AIR_FLOW_RATE_FROM_MAF                        = 0x50,
            FUEL_TYPE                                         = 0x51,
            ETHANOL_FUEL_PERCENTAGE                           = 0x52,
            ABSOLUTE_EVAP_SYSTEM_VAPOR_PRESSURE               = 0x53,
            EVAP_SYSTEM_VAPOR_PRESSURE2                       = 0x54,
            SHORT_TERM_OXYGEN_TRIM_BANK_1                     = 0x55,
            LONG_TERM_OXYGEN_TRIM_BANK_1                      = 0x56,
            SHORT_TERM_OXYGEN_TRIM_BANK_2                     = 0x57,
            LONG_TERM_OXYGEN_TRIM_BANK_2                      = 0x58,
            FUEL_RAIL_ABSOLUTE_PRESSURE                       = 0x59,
            RELATIVE_ACCELERATOR_PEDAL_POSITTION              = 0x5a,
            HYBRID_BATTERY_PACK_REMAINING_LIFE                = 0x5b,
            ENGINE_OIL_TEMPERATURE                            = 0x5c,
            FUEL_INJECTION_TIMING                             = 0x5d,
            ENGINE_FUEL_RATE                                  = 0x5e,
            EMISSION_REQUIREMENT_TO_WHICH_VEHICLE_IS_DESIGNED = 0x5f,

			PIDS_SUPPORT_61_80                                = 0x60, 
			PIDS_SUPPORT_81_A0                                = 0x80, 
			PIDS_SUPPORT_A1_C0                                = 0xA0, 

            // more PIDs can be added from: https://en.wikipedia.org/wiki/OBD-II_PIDs
        };

        enum VehicleInfo {
            VEHICLE_ID_NUMBER                                 = 0x02,
            ECU_NAME                                          = 0x09,
        };

        enum ErrorCode {
            ERROR_CHECK,
            ERROR_TIMEOUT,
            ERROR_RESOURCE,
            ERROR_PARAMETER,
            ERROR_INVALID_VALUE,
            ERROR_NOT_SUPPORTED
        };
    };
    
} // namespace Project


#endif // PROJECT_OBD2_H