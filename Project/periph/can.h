#ifndef PERIPH_CAN_H
#define PERIPH_CAN_H

#include "Core/Inc/can.h"
#include "etl/function.h"
#include "etl/linked_list.h"

/// select rx fifo buffer that is configured by CubeMX
#define PERIPH_CAN_USE_FIFO1

namespace Project::periph {

    /// CAN peripheral class
    /// @note requirements: CAN RXx interrupt
    struct CAN {
        enum {
#ifdef PERIPH_CAN_USE_FIFO0
            RX_FIFO = CAN_RX_FIFO0,
            IT_RX_FIFO = CAN_IT_RX_FIFO0_MSG_PENDING,
            FILTER_FIFO = CAN_FILTER_FIFO0,
#endif
#ifdef PERIPH_CAN_USE_FIFO1
            RX_FIFO = CAN_RX_FIFO1,
            IT_RX_FIFO = CAN_IT_RX_FIFO1_MSG_PENDING,
            FILTER_FIFO = CAN_FILTER_FIFO1,
#endif
        };

        /// received message class
        struct Message : CAN_RxHeaderTypeDef { uint8_t data[8]; };

        /// callback function class
        using Callback = etl::Function<void(Message &), void*>;

        /// callback list
        using CallbackList = etl::LinkedList<Callback>;

        CAN_HandleTypeDef &hcan;            ///< CAN handler configured by CubeMX
        CAN_FilterTypeDef canFilter = {}; 
        CAN_TxHeaderTypeDef txHeader = {};
        uint32_t txMailbox = {};
        CallbackList rxCallbackList = {};

        /// default constructor
        constexpr explicit CAN(CAN_HandleTypeDef &hcan) : hcan(hcan) {}

        CAN(const CAN&) = delete; ///< disable copy constructor
        CAN& operator=(const CAN&) = delete;  ///< disable copy assignment

        /// start CAN and activate notification at RX FIFO message pending
        /// @param useExtId true: use extended ID, false: use standard ID (default)
        void init(bool useExtId = false) {
            txHeader.RTR = CAN_RTR_DATA;
            txHeader.TransmitGlobalTime = DISABLE;
            setIdType(useExtId);
            setFilter();
            HAL_CAN_Stop(&hcan);
            HAL_CAN_Start(&hcan);
            HAL_CAN_ActivateNotification(&hcan, IT_RX_FIFO);
        }

        /// stop CAN 
        void deinit() { HAL_CAN_Stop(&hcan); }

        /// set rx callback
        /// @param fn rx callback function
        /// @param ctx rx callback function context
        template <typename Fn, typename Ctx>
        void setRxCallback(Fn&& fn, Ctx* ctx) {
            auto callback = Callback(etl::forward<Fn>(fn), ctx);
            if (etl::find(rxCallbackList, callback)) // return if callback is already in the list
                return;
            rxCallbackList.push(callback);
        }

        /// set rx callback
        /// @param fn rx callback function pointer
        template <typename Fn>
        void setRxCallback(Fn&& fn) {
            auto callback = Callback(etl::forward<Fn>(fn));
            if (etl::find(rxCallbackList, callback)) // return if callback is already in the list
                return;
            rxCallbackList.push(callback);
        }

        /// remove rx callback from the list
        /// @param fn rx callback function
        /// @param ctx rx callback function context
        template <typename Fn, typename Ctx>
        void detachRxCallback(Fn&& fn, Ctx* ctx) {
            auto callback = Callback(etl::forward<Fn>(fn), ctx);
            auto it = etl::find(rxCallbackList, callback);
            if (it) rxCallbackList.pop_at(it - rxCallbackList.begin());
        }

        /// remove rx callback from the list
        /// @param fn rx callback function
        template <typename Fn>
        void detachRxCallback(Fn&& fn) {
            auto callback = Callback(etl::forward<Fn>(fn));
            auto it = etl::find(rxCallbackList, callback);
            if (it) it.erase();
        }

        /// set filter by hardware
        /// @param filter default = 0
        /// @param mask default = 0
        /// @example filter = 0b1100, mask = 0b1111 -> allowed rx id: only 0b1100
        /// @example filter = 0b1100, mask = 0b1110 -> allowed rx id: 0b1100 and 0b1101
        void setFilter(uint32_t filter = 0, uint32_t mask = 0) {
            canFilter.FilterActivation = CAN_FILTER_ENABLE;

            if (isUsingExtId()) {
                // 18 bits, 3 bits offset, low half-word
                canFilter.FilterMaskIdLow  = (mask << 3) & 0xFFFFu;
                canFilter.FilterMaskIdHigh = (mask >> (18 - 5)) & 0b11111u;
                canFilter.FilterIdLow      = (filter << 3) & 0xFFFFu;
                canFilter.FilterIdHigh     = (filter >> (18 - 5)) & 0b11111u;
            } else {
                // 11 bits, left padding, high half-word
                canFilter.FilterMaskIdLow  = 0;
                canFilter.FilterMaskIdHigh = (mask << 5) & 0xFFFFu;
                canFilter.FilterIdLow      = 0;
                canFilter.FilterIdHigh     = (filter << 5) & 0xFFFFu;
            }

            canFilter.FilterFIFOAssignment = FILTER_FIFO;
            canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
            canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
            canFilter.FilterBank = 10;
            canFilter.SlaveStartFilterBank = 0;

            HAL_CAN_ConfigFilter(&hcan, &canFilter);
        }

        /// set tx ID type, standard or extended
        /// @param useExtId true: use extended ID, false: use standard ID
        void setIdType(bool useExtId) {
            txHeader.IDE = useExtId ? CAN_ID_EXT : CAN_ID_STD;
        }

        /// set tx ID
        void setId(uint32_t txId) {
            if (isUsingExtId()) txHeader.ExtId = txId;
            else txHeader.StdId = txId;
        }

        [[nodiscard]] bool isUsingExtId() const {
            return txHeader.IDE == CAN_ID_EXT;
        }

        /// CAN transmit non blocking
        /// @param buf pointer to data buffer
        /// @param len buffer length, maximum 8 bytes, default 8
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int transmit(const uint8_t* buf, uint16_t len = 8) {
            if (len > 8) len = 8;
            txHeader.DLC = len;
            return HAL_CAN_AddTxMessage(&hcan, &txHeader, const_cast<uint8_t*>(buf), &txMailbox);
        }

        /// CAN transmit non blocking with specific tx ID
        /// @param txId destination id
        /// @param buf pointer to data buffer
        /// @param len buffer length, maximum 8 bytes, default 8
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int transmit(uint32_t txId, const uint8_t* buf, uint16_t len = 8) {
            setId(txId);
            return transmit(buf, len);
        }

        /// CAN transmit non blocking with specific tx ID and set ID type
        /// @param useExtId true: use extended ID, false: use standard ID
        /// @param txId destination id
        /// @param buf pointer to data buffer
        /// @param len buffer length, maximum 8 bytes, default 8
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int transmit(bool useExtId, uint32_t txId, const uint8_t* buf, uint16_t len = 8) {
            setIdType(useExtId);
            setId(txId);
            return transmit(buf, len);
        }
    };

    inline CAN can { hcan };

} // namespace Project


#endif // PERIPH_CAN_H