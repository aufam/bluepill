#ifndef PROJECT_PERIPH_CAN_H
#define PROJECT_PERIPH_CAN_H

#include "../../Core/Inc/can.h"
#include "os.h"

namespace Project::Periph {

    /// CAN peripheral class. requirements: CAN RX0 interrupt
    struct CAN {
        /// CAN receive message class
        struct Msg {
            CAN_RxHeaderTypeDef rxHeader;
            uint8_t rxBuffer[8];
        };
        using Queue = OS::QueueStatic<Msg, 1>; ///< CAN receive queue type definition

        CAN_HandleTypeDef &hcan;
        CAN_FilterTypeDef canFilter = {};
        CAN_TxHeaderTypeDef txHeader = {};
        uint32_t txMailbox = {};
        Queue rxQueue = {};
        constexpr explicit CAN(CAN_HandleTypeDef &hcan) : hcan(hcan) {}

        /// start CAN and activate notification at RX FIFO0 message pending
        /// @param txId ID of this CAN
        /// @param useExtId true: use extended ID, false: use standard ID
        void init(uint32_t txId, bool useExtId = false);
        /// set rx ID filter by hardware
        /// @param rxId maximum rx ID allowed, has to be >= mask
        /// @param mask minimum rx ID allowed, has to be <= rxId
        void setFilter(uint32_t rxId = 0, uint32_t mask = 0);
        /// set ID type
        /// @param useExtId true: use extended ID, false: use standard ID
        void setIdType(bool useExtId);
        void setId(uint32_t txId);
        bool isUsingExtId() const;
        /// CAN transmit non blocking
        /// @param buf pointer to data buffer
        /// @param len buffer length, maximum 8 bytes
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int transmit(uint8_t *buf, uint16_t len = 8);
    };

    inline CAN can { hcan };
    
} // namespace Project


#endif // PROJECT_PERIPH_CAN_H