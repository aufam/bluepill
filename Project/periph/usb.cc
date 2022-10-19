#include "periph/usb.h"

extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

namespace Project::Periph {

    void USBD::setRxCallback(USBD::Callback::Function rxCBFn, void *rxCBArg) {
        rxCallback.fn  = rxCBFn;
        rxCallback.arg = rxCBArg;
    }

    int USBD::transmit(const void *buf, uint16_t len) {
        return CDC_Transmit_FS((uint8_t *)buf, len);
    }

    USBD usb(*(USBD::Buffer *) UserRxBufferFS);

} // namespace Project

void CDC_ReceiveCplt_Callback(const uint8_t *pbuf, uint32_t len) {
    (void) pbuf;
    using namespace Project::Periph;
    auto &cb = usb.rxCallback;
    if (cb.fn) cb.fn(cb.arg, len);
}