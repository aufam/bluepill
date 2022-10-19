#include "periph/can.h"

namespace Project::Periph {

    void CAN::init(uint32_t txId, bool useExtId) {
        txHeader.RTR = CAN_RTR_DATA;
        txHeader.TransmitGlobalTime = DISABLE;
        setIdType(useExtId);
        setId(txId);
        HAL_CAN_Start(&hcan);
        HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
        rxQueue.init();
    }

    void CAN::setIdType(bool useExtId) {
        txHeader.IDE = useExtId ? CAN_ID_EXT : CAN_ID_STD;
    }
    
    void CAN::setId(uint32_t txId) {
        if (isUsingExtId()) txHeader.ExtId = txId;
        else txHeader.StdId = txId;
    }
    
    void CAN::setFilter(uint32_t rxId, uint32_t mask) {
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

        canFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
        canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
        canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
        canFilter.FilterBank = 10;
        canFilter.SlaveStartFilterBank = 0;

        HAL_CAN_ConfigFilter(&hcan, &canFilter);
    }

    bool CAN::isUsingExtId() const {
        return txHeader.IDE == CAN_ID_EXT;
    }

    int CAN::transmit(uint8_t *buf, uint16_t len) {
        if (len > 8) len = 8;
        txHeader.DLC = len;
        return HAL_CAN_AddTxMessage(&hcan, &txHeader, buf, &txMailbox);
    }

} // namespace Project

/// every message received will be pushed to rx queue
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    using namespace Project::Periph;
    CAN::Msg msg = {};
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &msg.rxHeader, msg.rxBuffer);
    if (hcan->Instance == can.hcan.Instance) can.rxQueue << msg;
}
