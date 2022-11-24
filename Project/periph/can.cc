#include "periph/can.h"

/// every received message will be pushed to rx queue
#ifdef PROJECT_PERIPH_CAN_USE_FIFO0
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
#endif
#ifdef PROJECT_PERIPH_CAN_USE_FIFO1
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
#endif
    using namespace Project::Periph;
    CAN::Message msg = {};
    HAL_CAN_GetRxMessage(&can.hcan, CAN::RX_FIFO, &msg.rxHeader, msg.data);
    if (can.rxCallback.fn) can.rxCallback.fn(can.rxCallback.arg, msg);
}
