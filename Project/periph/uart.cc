#include "periph/uart.h"

using namespace Project::periph;

static UART* selector(UART_HandleTypeDef *huart) {
    if (huart->Instance == uart1.huart.Instance)
        return &uart1;
    return nullptr;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    auto uart = selector(huart);
    if (uart) {
        uart->rxCallback(uart->rxBuffer.data(), Size);
        uart->init();
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    auto uart = selector(huart);
    if (uart) 
        uart->txCallback();
}