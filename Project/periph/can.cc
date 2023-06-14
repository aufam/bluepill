#include "periph/can.h"

using namespace Project::periph;

#ifdef PERIPH_CAN_USE_FIFO0
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *) {
#endif
#ifdef PERIPH_CAN_USE_FIFO1
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *) {
#endif
    
    CAN::Message msg = {};
    HAL_CAN_GetRxMessage(&can.hcan, CAN::RX_FIFO, reinterpret_cast<CAN_RxHeaderTypeDef *>(&msg), msg.data);
    for (auto& cb : can.rxCallbackList) 
        cb(msg);
}
