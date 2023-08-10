#include "periph/usb.h"

using namespace Project::periph;
extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
USBD Project::periph::usb(*(USBD::Buffer *) UserRxBufferFS);

void CDC_ReceiveCplt_Callback(const uint8_t *pbuf, uint32_t len) {
    (void) pbuf;
    usb.rxCallback(usb.rxBuffer.data(), len);
}