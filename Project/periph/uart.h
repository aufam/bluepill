#ifndef PERIPH_UART_H
#define PERIPH_UART_H

#include "../../Core/Inc/usart.h"
#include "etl/array.h"
#include "etl/string.h"
#include "etl/function.h"

namespace Project::Periph {

    /// UART peripheral class.
    /// @note requirements: global interrupt, rx DMA
    struct UART {
        using RxCallback = etl::Function<void(const uint8_t*, size_t), void*>;
        using TxCallback = etl::Function<void(), void*>;
        using Buffer = etl::Array<uint8_t, 64>; ///< UART rx buffer type definition

        UART_HandleTypeDef &huart; ///< UART handler generated by cubeMX
        Buffer rxBuffer = {};
        RxCallback rxCallback = {};
        TxCallback txCallback = {};
        constexpr explicit UART(UART_HandleTypeDef &huart) : huart(huart) {}

        /// set callback and start receive to idle DMA
        /// @param rxCBFn receive callback function pointer
        /// @param rxCBArg receive callback function argument
        /// @param txCBFn transmit callback function pointer
        /// @param txCBArg transmit callback function argument
        void init(RxCallback::Fn rxCBFn = nullptr, void *rxCBArg = nullptr,
                  TxCallback::Fn txCBFn = nullptr, void *txCBArg = nullptr)
        {
            setRxCallback(rxCBFn, rxCBArg);
            setTxCallback(txCBFn, txCBArg);
            HAL_UARTEx_ReceiveToIdle_DMA(&huart, rxBuffer.data(), rxBuffer.len());
            __HAL_DMA_DISABLE_IT(huart.hdmarx, DMA_IT_HT); // disable half complete callback
        }

        /// disable receive DMA
        void deinit() { HAL_UART_DMAStop(&huart); }

        /// set rx callback
        /// @param rxCBFn receive callback function pointer
        /// @param rxCBArg receive callback function argument
        void setRxCallback(RxCallback::Fn rxCBFn, void *rxCBArg = nullptr) {
            rxCallback = { rxCBFn, rxCBArg };
        }

        /// set tx callback
        /// @param txCBFn transmit callback function pointer
        /// @param txCBArg transmit callback function argument
        void setTxCallback(TxCallback::Fn txCBFn, void *txCBArg = nullptr) {
            txCallback = { txCBFn, txCBArg };
        }

        /// UART transmit blocking
        /// @param buf data buffer
        /// @param len buffer length
        /// @retval HAL_StatusTypeDef (see stm32fXxx_hal_def.h)
        int transmitBlocking(void *buf, uint16_t len, uint32_t timeout = HAL_MAX_DELAY) {
            while (huart.gState != HAL_UART_STATE_READY);
            return HAL_UART_Transmit(&huart, reinterpret_cast<uint8_t*>(buf), len, timeout);
        }

        /// UART transmit non blocking
        /// @param buf data buffer
        /// @param len buffer length
        /// @retval HAL_StatusTypeDef (see stm32fXxx_hal_def.h)
        int transmit(void *buf, uint16_t len) {
            return HAL_UART_Transmit_IT(&huart, reinterpret_cast<uint8_t *>(buf), len);
        }

        void setBaudRate(uint32_t baud) {
            huart.Init.BaudRate = baud;
            HAL_UART_Init(&huart);
        }
        [[nodiscard]] uint32_t getBaudRate() const { return huart.Init.BaudRate; }

        /// write blocking operator
        template <size_t N>
        UART &operator << (const etl::String<N>& str) {
            transmitBlocking((void *)(str.data()), str.len());
            return *this;
        }
        UART &operator << (const char *str) {
            transmitBlocking((void *) str, strlen(str));
            return *this;
        }
    };

    /// UART 1, baud rate 9600
    inline UART uart1 { huart1 };

    /// UART 2, baud rate 9600
    inline UART uart2 { huart2 };

} // namespace Project


#endif // PERIPH_UART_H