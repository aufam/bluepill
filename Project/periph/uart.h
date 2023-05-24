#ifndef PERIPH_UART_H
#define PERIPH_UART_H

#include "../../Core/Inc/usart.h"
#include "etl/array.h"
#include "etl/string.h"
#include "etl/function.h"

namespace Project::periph {

    /// UART peripheral class.
    /// @note requirements: global interrupt, rx DMA
    struct UART {
        using RxCallback = etl::Function<void(const uint8_t*, size_t), void*>;  ///< rx callback function class
        using TxCallback = etl::Function<void(), void*>;                        ///< tx callback function class
        using Buffer = etl::Array<uint8_t, 64>;                                 ///< UART rx buffer class

        UART_HandleTypeDef &huart;  ///< UART handler configured by cubeMX
        RxCallback rxCallback = {}; ///< rx callback function
        TxCallback txCallback = {}; ///< tx callback function
        Buffer rxBuffer = {};       ///< rx buffer

        /// default constructor
        constexpr explicit UART(UART_HandleTypeDef &huart) : huart(huart) {}

        UART(const UART&) = delete; ///< disable copy constructor
        UART(UART&&) = delete;      ///< disable move constructor

        UART& operator=(const UART&) = delete;  ///< disable move constructor
        UART& operator=(UART&&) = delete;       ///< disable move assignment

        /// set callback and start receive to idle DMA
        /// @param rxFn receive callback function pointer
        /// @param rxArg receive callback function argument
        /// @param txFn transmit callback function pointer
        /// @param txArg transmit callback function argument
        template <typename RxArg, typename TxArg>
        void init(void (*rxFn)(RxArg*, const uint8_t*, size_t), RxArg* rxArg, void (*txFn)(TxArg*), TxArg* txArg) {
            setRxCallback(rxFn, rxArg);
            setTxCallback(txFn, txArg);
            HAL_UARTEx_ReceiveToIdle_DMA(&huart, rxBuffer.data(), rxBuffer.len());
            __HAL_DMA_DISABLE_IT(huart.hdmarx, DMA_IT_HT); // disable half complete callback
        }

        /// set callback and start receive to idle DMA
        /// @param rxFn receive callback function pointer
        /// @param txFn transmit callback function pointer
        void init(void (*rxFn)(const uint8_t*, size_t) = nullptr, void (*txFn)() = nullptr) {
            setRxCallback(rxFn);
            setTxCallback(txFn);
            HAL_UARTEx_ReceiveToIdle_DMA(&huart, rxBuffer.data(), rxBuffer.len());
            __HAL_DMA_DISABLE_IT(huart.hdmarx, DMA_IT_HT); // disable half complete callback
        }

        /// disable receive DMA
        void deinit() { HAL_UART_DMAStop(&huart); }

        /// set rx callback
        /// @param fn receive callback function pointer
        /// @param arg receive callback function argument
        template <typename Arg>
        void setRxCallback(void (*fn)(Arg*, const uint8_t*, size_t), Arg* arg) { 
            rxCallback = { (void (*)(void*, const uint8_t*, size_t)) fn, (void*) arg }; 
        }

        /// set rx callback
        /// @param fn receive callback function pointer
        void setRxCallback(void (*fn)(const uint8_t*, size_t)) { 
            auto wrapperFunc = +[](void* fn, const uint8_t* buf, size_t len) {
                auto f = (void(*)(const uint8_t*, size_t)) fn;
                if (f) f(buf, len);
            }; 
            rxCallback = { wrapperFunc, (void*) fn };
        }

        /// set tx callback
        /// @param fn receive callback function pointer
        /// @param arg receive callback function argument
        template <typename Arg>
        void setTxCallback(void (*fn)(Arg*), Arg* arg) { txCallback = { (void (*)(void*)) fn, (void*) arg }; }

        /// set tx callback
        /// @param fn receive callback function pointer
        void setTxCallback(void (*fn)()) { txCallback = { (void (*)(void*)) fn, nullptr }; }

        /// UART transmit blocking
        /// @param buf data buffer
        /// @param len buffer length
        /// @param timeout default = @ref HAL_MAX_DELAY
        /// @retval HAL_StatusTypeDef (see stm32fXxx_hal_def.h)
        int transmitBlocking(const void *buf, uint16_t len, uint32_t timeout = HAL_MAX_DELAY) {
            while (huart.gState != HAL_UART_STATE_READY);
            return HAL_UART_Transmit(&huart, (uint8_t *) buf, len, timeout);
        }

        /// UART transmit non blocking
        /// @param buf data buffer
        /// @param len buffer length
        /// @retval HAL_StatusTypeDef (see stm32fXxx_hal_def.h)
        int transmit(const void *buf, uint16_t len) { return HAL_UART_Transmit_IT(&huart, (uint8_t *) buf, len); }

        /// set UART baud rate
        /// @param baud desired baud rate
        /// @retval HAL_StatusTypeDef (see stm32fXxx_hal_def.h)
        int setBaudRate(uint32_t baud) { huart.Init.BaudRate = baud; return HAL_UART_Init(&huart); }

        /// get UART baud rate
        /// @retval baud rate
        [[nodiscard]] 
        uint32_t getBaudRate() const { return huart.Init.BaudRate; }

        /// write blocking operator for etl::string
        template <size_t N>
        UART& operator<<(const etl::String<N>& str) { transmitBlocking(str.data(), str.len()); return *this; }

        /// write blocking operator for traditional string
        UART& operator<<(const char *str) { transmitBlocking(str, strlen(str)); return *this; }
    };

    /// UART 1, baud rate 9600
    inline UART uart1 { huart1 };

    /// UART 2, baud rate 9600
    inline UART uart2 { huart2 };

} // namespace Project


#endif // PERIPH_UART_H