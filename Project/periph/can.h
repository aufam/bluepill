#ifndef PROJECT_PERIPH_CAN_H
#define PROJECT_PERIPH_CAN_H

#include "../../Core/Inc/can.h"

/// select fifo0 or fifo1 (default)
#define PROJECT_PERIPH_CAN_USE_FIFO1

namespace Project::Periph {

    /// CAN peripheral class
    /// @note requirements: CAN RXx interrupt
    struct CAN {
        enum {
#ifdef PROJECT_PERIPH_CAN_USE_FIFO0
            RX_FIFO = CAN_RX_FIFO0,
            IT_RX_FIFO = CAN_IT_RX_FIFO0_MSG_PENDING,
            FILTER_FIFO = CAN_FILTER_FIFO0,
#endif
#ifdef PROJECT_PERIPH_CAN_USE_FIFO1
            RX_FIFO = CAN_RX_FIFO1,
            IT_RX_FIFO = CAN_IT_RX_FIFO1_MSG_PENDING,
            FILTER_FIFO = CAN_FILTER_FIFO1,
#endif
        };

        struct Message {
            CAN_RxHeaderTypeDef rxHeader;
            uint8_t data[8];
        };

        struct Callbcak {
            typedef void (* Function)(void*, Message&);
            Function fn;
            void *arg;
        };

        CAN_HandleTypeDef &hcan;
        CAN_FilterTypeDef canFilter = {};
        CAN_TxHeaderTypeDef txHeader = {};
        uint32_t txMailbox = {};
        Callbcak rxCallback = {};
        constexpr explicit CAN(CAN_HandleTypeDef &hcan) : hcan(hcan) {}

        /// start CAN and activate notification at RX FIFO message pending
        /// @param txId ID of this CAN
        /// @param useExtId true: use extended ID, false: use standard ID (default)
        /// @param fn rx callback function pointer
        /// @param arg rx callback function argument
        void init(uint32_t txId, bool useExtId = false, Callbcak::Function fn = nullptr, void* arg = nullptr) {
            txHeader.RTR = CAN_RTR_DATA;
            txHeader.TransmitGlobalTime = DISABLE;
            setIdType(useExtId);
            setId(txId);
            setRxCallback(fn, arg);
            HAL_CAN_Start(&hcan);
            HAL_CAN_ActivateNotification(&hcan, IT_RX_FIFO);
        }

        void deinit() {
            HAL_CAN_Stop(&hcan);
            setRxCallback(nullptr);
        }

        void setRxCallback(Callbcak::Function fn, void* arg = nullptr) {
            rxCallback.fn = fn;
            rxCallback.arg = arg;
        }

        /// set rx ID filter by hardware
        /// @param rxId maximum rx ID allowed, has to be >= mask, default 0
        /// @param mask minimum rx ID allowed, has to be <= rxId, default 0
        void setFilter(uint32_t rxId = 0, uint32_t mask = 0) {
            canFilter.FilterActivation =
                    (rxId == 0 && mask == 0) || (mask > rxId) ?
                    CAN_FILTER_DISABLE : CAN_FILTER_ENABLE;

            if (isUsingExtId()) {
                // 18 bits, 3 bits offset, low half-word
                canFilter.FilterMaskIdLow  = (mask << 3) & 0xFFFFu;
                canFilter.FilterMaskIdHigh = (mask >> (18 - 5)) & 0b11111u;
                canFilter.FilterIdLow      = (rxId << 3) & 0xFFFFu;
                canFilter.FilterIdHigh     = (rxId >> (18 - 5)) & 0b11111u;
            } else {
                // 11 bits, left padding, high half-word
                canFilter.FilterMaskIdLow  = 0;
                canFilter.FilterMaskIdHigh = (mask << 5) & 0xFFFFu;
                canFilter.FilterIdLow      = 0;
                canFilter.FilterIdHigh     = (rxId << 5) & 0xFFFFu;
            }

            canFilter.FilterFIFOAssignment = FILTER_FIFO;
            canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
            canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
            canFilter.FilterBank = 10;
            canFilter.SlaveStartFilterBank = 0;

            HAL_CAN_ConfigFilter(&hcan, &canFilter);
        }

        /// set ID type
        /// @param useExtId true: use extended ID, false: use standard ID
        void setIdType(bool useExtId) {
            txHeader.IDE = useExtId ? CAN_ID_EXT : CAN_ID_STD;
        }

        void setId(uint32_t txId) {
            if (isUsingExtId()) txHeader.ExtId = txId;
            else txHeader.StdId = txId;
        }

        [[nodiscard]] bool isUsingExtId() const {
            return txHeader.IDE == CAN_ID_EXT;
        }

        /// CAN transmit blocking
        /// @param buf pointer to data buffer
        /// @param len buffer length, maximum 8 bytes, default 8
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int transmit(uint8_t *buf, uint16_t len = 8) {
            if (len > 8) len = 8;
            txHeader.DLC = len;
            return HAL_CAN_AddTxMessage(&hcan, &txHeader, buf, &txMailbox);
        }

        /// CAN transmit blocking with specific tx ID
        /// @param txId destination id
        /// @param buf pointer to data buffer
        /// @param len buffer length, maximum 8 bytes, default 8
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int transmit(uint32_t txId, uint8_t *buf, uint16_t len = 8) {
            setId(txId);
            if (len > 8) len = 8;
            txHeader.DLC = len;
            return HAL_CAN_AddTxMessage(&hcan, &txHeader, buf, &txMailbox);
        }
    };

    inline CAN can { hcan };

} // namespace Project


#endif // PROJECT_PERIPH_CAN_H