#include "periph/uart.h"

using namespace Project::periph;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    UART *uart;
    if (huart->Instance == uart1.huart.Instance) uart = &uart1;
    else return;

    uart->rxCallback(uart->rxBuffer.data(), Size);
    uart->init();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    UART *uart;
    if (huart->Instance == uart1.huart.Instance) uart = &uart1;
    else return;

    uart->txCallback();
}