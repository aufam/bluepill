#ifndef PROJECT_TYPES_H
#define PROJECT_TYPES_H

#include "main.h"

namespace Project::BLDC {

    enum MC_FAULT_CODE : int {
        FAULT_CODE_NONE = 0,
        FAULT_CODE_OVER_VOLTAGE,
        FAULT_CODE_UNDER_VOLTAGE,
        FAULT_CODE_DRV,
        FAULT_CODE_ABS_OVER_CURRENT,
        FAULT_CODE_OVER_TEMP_FET,
        FAULT_CODE_OVER_TEMP_MOTOR,
        FAULT_CODE_GATE_DRIVER_OVER_VOLTAGE,
        FAULT_CODE_GATE_DRIVER_UNDER_VOLTAGE,
        FAULT_CODE_MCU_UNDER_VOLTAGE,
        FAULT_CODE_BOOTING_FROM_WATCHDOG_RESET,
        FAULT_CODE_ENCODER_SPI,
        FAULT_CODE_ENCODER_SINCOS_BELOW_MIN_AMPLITUDE,
        FAULT_CODE_ENCODER_SINCOS_ABOVE_MAX_AMPLITUDE,
        FAULT_CODE_FLASH_CORRUPTION,
        FAULT_CODE_HIGH_OFFSET_CURRENT_SENSOR_1,
        FAULT_CODE_HIGH_OFFSET_CURRENT_SENSOR_2,
        FAULT_CODE_HIGH_OFFSET_CURRENT_SENSOR_3,
        FAULT_CODE_UNBALANCED_CURRENTS,
        FAULT_CODE_BRK,
        FAULT_CODE_RESOLVER_LOT,
        FAULT_CODE_RESOLVER_DOS,
        FAULT_CODE_RESOLVER_LOS,
        FAULT_CODE_FLASH_CORRUPTION_APP_CFG,
        FAULT_CODE_FLASH_CORRUPTION_MC_CFG,
        FAULT_CODE_ENCODER_NO_MAGNET,
        FAULT_CODE_ENCODER_MAGNET_TOO_STRONG
    };

    struct Values {
        int id;                     ///< controller id
        float inVoltage;            ///< in volt
        float duty;                 ///< in range [-1.0, 1.0]
        float inCurrent;            ///< in ampere
        float outCurrent;           ///< in ampere
        float mosfetTemp;           ///< in C
        float erpm;                 ///< in rpm
        int tachometer;             ///< relative position in step
        MC_FAULT_CODE faultCode;

        enum MASK : uint32_t {
            MASK_ID             = 1 << 17,
            MASK_IN_VOLTAGE     = 1 << 8,
            MASK_MOSFET_TEMP    = 1 << 0,
            MASK_OUT_CURRENT    = 1 << 2,
            MASK_IN_CURRENT     = 1 << 3,
            MASK_ERPM           = 1 << 7,
            MASK_DUTY           = 1 << 6,
            MASK_TACHOMETER     = 1 << 13,
            MASK_FAULT_CODE     = 1 << 15,
        };
    };

    enum COMM_PACKET_ID : uint8_t {
        COMM_FW_VERSION = 0,
        COMM_JUMP_TO_BOOTLOADER,
        COMM_ERASE_NEW_APP,
        COMM_WRITE_NEW_APP_DATA,
        COMM_GET_VALUES,
        COMM_SET_DUTY,
        COMM_SET_CURRENT,
        COMM_SET_CURRENT_BRAKE,
        COMM_SET_RPM,
        COMM_SET_POS,
        COMM_SET_HANDBRAKE,
        COMM_SET_DETECT,
        COMM_SET_SERVO_POS,
        COMM_SET_MCCONF,
        COMM_GET_MCCONF,
        COMM_GET_MCCONF_DEFAULT,
        COMM_SET_APPCONF,
        COMM_GET_APPCONF,
        COMM_GET_APPCONF_DEFAULT,
        COMM_SAMPLE_PRINT,
        COMM_TERMINAL_CMD,
        COMM_PRINT,
        COMM_ROTOR_POSITION,
        COMM_EXPERIMENT_SAMPLE,
        COMM_DETECT_MOTOR_PARAM,
        COMM_DETECT_MOTOR_R_L,
        COMM_DETECT_MOTOR_FLUX_LINKAGE,
        COMM_DETECT_ENCODER,
        COMM_DETECT_HALL_FOC,
        COMM_REBOOT,
        COMM_ALIVE,
        COMM_GET_DECODED_PPM,
        COMM_GET_DECODED_ADC,
        COMM_GET_DECODED_CHUK,
        COMM_FORWARD_CAN,
        COMM_SET_CHUCK_DATA,
        COMM_CUSTOM_APP_DATA,
        COMM_NRF_START_PAIRING,
        COMM_GPD_SET_FSW,
        COMM_GPD_BUFFER_NOTIFY,
        COMM_GPD_BUFFER_SIZE_LEFT,
        COMM_GPD_FILL_BUFFER,
        COMM_GPD_OUTPUT_SAMPLE,
        COMM_GPD_SET_MODE,
        COMM_GPD_FILL_BUFFER_INT8,
        COMM_GPD_FILL_BUFFER_INT16,
        COMM_GPD_SET_BUFFER_INT_SCALE,
        COMM_GET_VALUES_SETUP,
        COMM_SET_MCCONF_TEMP,
        COMM_SET_MCCONF_TEMP_SETUP,
        COMM_GET_VALUES_SELECTIVE,
        COMM_GET_VALUES_SETUP_SELECTIVE,
        COMM_EXT_NRF_PRESENT,
        COMM_EXT_NRF_ESB_SET_CH_ADDR,
        COMM_EXT_NRF_ESB_SEND_DATA,
        COMM_EXT_NRF_ESB_RX_DATA,
        COMM_EXT_NRF_SET_ENABLED,
        COMM_DETECT_MOTOR_FLUX_LINKAGE_OPENLOOP,
        COMM_DETECT_APPLY_ALL_FOC,
        COMM_JUMP_TO_BOOTLOADER_ALL_CAN,
        COMM_ERASE_NEW_APP_ALL_CAN,
        COMM_WRITE_NEW_APP_DATA_ALL_CAN,
        COMM_PING_CAN,
        COMM_APP_DISABLE_OUTPUT,
        COMM_TERMINAL_CMD_SYNC,
        COMM_GET_IMU_DATA,
        COMM_BM_CONNECT,
        COMM_BM_ERASE_FLASH_ALL,
        COMM_BM_WRITE_FLASH,
        COMM_BM_REBOOT,
        COMM_BM_DISCONNECT,
        COMM_BM_MAP_PINS_DEFAULT,
        COMM_BM_MAP_PINS_NRF5X,
        COMM_ERASE_BOOTLOADER,
        COMM_ERASE_BOOTLOADER_ALL_CAN,
        COMM_PLOT_INIT,
        COMM_PLOT_DATA,
        COMM_PLOT_ADD_GRAPH,
        COMM_PLOT_SET_GRAPH,
        COMM_GET_DECODED_BALANCE,
        COMM_BM_MEM_READ,
        COMM_WRITE_NEW_APP_DATA_LZO,
        COMM_WRITE_NEW_APP_DATA_ALL_CAN_LZO,
        COMM_BM_WRITE_FLASH_LZO,
        COMM_SET_CURRENT_REL,
        COMM_CAN_FWD_FRAME,
        COMM_SET_BATTERY_CUT,
        COMM_SET_BLE_NAME,
        COMM_SET_BLE_PIN,
        COMM_SET_CAN_MODE,
        COMM_GET_IMU_CALIBRATION,
        COMM_GET_MCCONF_TEMP,

        // Custom configuration for hardware
        COMM_GET_CUSTOM_CONFIG_XML,
        COMM_GET_CUSTOM_CONFIG,
        COMM_GET_CUSTOM_CONFIG_DEFAULT,
        COMM_SET_CUSTOM_CONFIG,

        // BMS commands
        COMM_BMS_GET_VALUES,
        COMM_BMS_SET_CHARGE_ALLOWED,
        COMM_BMS_SET_BALANCE_OVERRIDE,
        COMM_BMS_RESET_COUNTERS,
        COMM_BMS_FORCE_BALANCE,
        COMM_BMS_ZERO_CURRENT_OFFSET,

        // FW updates commands for different HW types
        COMM_JUMP_TO_BOOTLOADER_HW,
        COMM_ERASE_NEW_APP_HW,
        COMM_WRITE_NEW_APP_DATA_HW,
        COMM_ERASE_BOOTLOADER_HW,
        COMM_JUMP_TO_BOOTLOADER_ALL_CAN_HW,
        COMM_ERASE_NEW_APP_ALL_CAN_HW,
        COMM_WRITE_NEW_APP_DATA_ALL_CAN_HW,
        COMM_ERASE_BOOTLOADER_ALL_CAN_HW,

        COMM_SET_ODOMETER,

        // Power switch commands
        COMM_PSW_GET_STATUS,
        COMM_PSW_SWITCH,

        COMM_BMS_FWD_CAN_RX,
        COMM_BMS_HW_DATA,
        COMM_GET_BATTERY_CUT,
        COMM_BM_HALT_REQ,
        COMM_GET_QML_UI_HW,
        COMM_GET_QML_UI_APP,
        COMM_CUSTOM_HW_DATA,
        COMM_QMLUI_ERASE,
        COMM_QMLUI_WRITE,

        // IO Board
        COMM_IO_BOARD_GET_ALL,
        COMM_IO_BOARD_SET_PWM,
        COMM_IO_BOARD_SET_DIGITAL,

        COMM_BM_MEM_WRITE,
        COMM_BMS_BLNC_SELFTEST,
        COMM_GET_EXT_HUM_TMP,
        COMM_GET_STATS,
        COMM_RESET_STATS,
    };
    
    enum CAN_PACKET_ID : uint8_t {
        CAN_PACKET_SET_DUTY = 0,
        CAN_PACKET_SET_CURRENT,
        CAN_PACKET_SET_CURRENT_BRAKE,
        CAN_PACKET_SET_RPM,
        CAN_PACKET_SET_POS,
        CAN_PACKET_FILL_RX_BUFFER,
        CAN_PACKET_FILL_RX_BUFFER_LONG,
        CAN_PACKET_PROCESS_RX_BUFFER,
        CAN_PACKET_PROCESS_SHORT_BUFFER,
        CAN_PACKET_STATUS,
        CAN_PACKET_SET_CURRENT_REL,
        CAN_PACKET_SET_CURRENT_BRAKE_REL,
        CAN_PACKET_SET_CURRENT_HANDBRAKE,
        CAN_PACKET_SET_CURRENT_HANDBRAKE_REL,
        CAN_PACKET_STATUS_2,
        CAN_PACKET_STATUS_3,
        CAN_PACKET_STATUS_4,
        CAN_PACKET_PING,
        CAN_PACKET_PONG,
        CAN_PACKET_DETECT_APPLY_ALL_FOC,
        CAN_PACKET_DETECT_APPLY_ALL_FOC_RES,
        CAN_PACKET_CONF_CURRENT_LIMITS,
        CAN_PACKET_CONF_STORE_CURRENT_LIMITS,
        CAN_PACKET_CONF_CURRENT_LIMITS_IN,
        CAN_PACKET_CONF_STORE_CURRENT_LIMITS_IN,
        CAN_PACKET_CONF_FOC_ERPMS,
        CAN_PACKET_CONF_STORE_FOC_ERPMS,
        CAN_PACKET_STATUS_5,
        CAN_PACKET_POLL_TS5700N8501_STATUS,
        CAN_PACKET_CONF_BATTERY_CUT,
        CAN_PACKET_CONF_STORE_BATTERY_CUT,
        CAN_PACKET_SHUTDOWN,
        CAN_PACKET_IO_BOARD_ADC_1_TO_4,
        CAN_PACKET_IO_BOARD_ADC_5_TO_8,
        CAN_PACKET_IO_BOARD_ADC_9_TO_12,
        CAN_PACKET_IO_BOARD_DIGITAL_IN,
        CAN_PACKET_IO_BOARD_SET_OUTPUT_DIGITAL,
        CAN_PACKET_IO_BOARD_SET_OUTPUT_PWM,
        CAN_PACKET_BMS_V_TOT,
        CAN_PACKET_BMS_I,
        CAN_PACKET_BMS_AH_WH,
        CAN_PACKET_BMS_V_CELL,
        CAN_PACKET_BMS_BAL,
        CAN_PACKET_BMS_TEMPS,
        CAN_PACKET_BMS_HUM,
        CAN_PACKET_BMS_SOC_SOH_TEMP_STAT,
        CAN_PACKET_PSW_STAT,
        CAN_PACKET_PSW_SWITCH,
        CAN_PACKET_BMS_HW_DATA_1,
        CAN_PACKET_BMS_HW_DATA_2,
        CAN_PACKET_BMS_HW_DATA_3,
        CAN_PACKET_BMS_HW_DATA_4,
        CAN_PACKET_BMS_HW_DATA_5,
        CAN_PACKET_BMS_AH_WH_CHG_TOTAL,
        CAN_PACKET_BMS_AH_WH_DIS_TOTAL,
        CAN_PACKET_UPDATE_PID_POS_OFFSET,
        CAN_PACKET_POLL_ROTOR_POS,
        CAN_PACKET_BMS_BOOT,
        CAN_PACKET_MAKE_ENUM_32_BITS = 0xFF,
    };
}

#endif // PROJECT_TYPES_H