#include "periph/can.h"

/// every received message will be pushed to rx queue
#ifdef PERIPH_CAN_USE_FIFO0
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
#endif
#ifdef PERIPH_CAN_USE_FIFO1
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *) {
#endif
    using namespace Project::Periph;
    CAN::Message msg = {};
    HAL_CAN_GetRxMessage(&can.hcan, CAN::RX_FIFO, reinterpret_cast<CAN_RxHeaderTypeDef *>(&msg), msg.data);
    can.rxCallback(msg);
}
