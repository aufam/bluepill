#include "periph/uart.h"

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    using namespace Project::Periph;
    UART *uart;
    if (huart->Instance == uart1.huart.Instance) uart = &uart1;
    else if (huart->Instance == uart2.huart.Instance) uart = &uart2;
    else return;

    uart->rxCallback(uart->rxBuffer.data(), Size);
    HAL_UARTEx_ReceiveToIdle_DMA(&uart->huart, uart->rxBuffer.begin(), UART::Buffer::size());
    __HAL_DMA_DISABLE_IT(uart->huart.hdmarx, DMA_IT_HT);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    using namespace Project::Periph;
    UART *uart;
    if (huart->Instance == uart1.huart.Instance) uart = &uart1;
    else if (huart->Instance == uart2.huart.Instance) uart = &uart2;
    else return;

    uart->txCallback();
}