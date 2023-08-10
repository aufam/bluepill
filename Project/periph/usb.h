#ifndef PERIPH_USB_H
#define PERIPH_USB_H

#include "usbd_cdc_if.h"
#include "etl/array.h"
#include "etl/string.h"
#include "etl//function.h"

namespace Project::periph {

    /// USB peripheral class
    struct USBD {
        /// callback function class
        using Callback = etl::Function<void(const uint8_t*, size_t), void*>;
        using Buffer = etl::Array<uint8_t, APP_RX_DATA_SIZE>; ///< USB rx buffer type definition

        Callback rxCallback = {};
        Buffer &rxBuffer;
        constexpr explicit USBD(Buffer &rxBuffer) : rxBuffer(rxBuffer) {}

        /// init usb, set rx callback
        /// @param rxCBFn receive callback function pointer
        /// @param rxCBArg receive callback function argument
        void init(Callback::Fn rxCBFn, void *rxCBArg = nullptr) { setRxCallback(rxCBFn, rxCBArg); }

        /// set rx callback
        /// @param fn receive callback function
        /// @param ctx receive callback function context
        template <typename Fn, typename Ctx>
        void setRxCallback(Fn&& fn, Ctx* ctx) { rxCallback = Callback(etl::forward<Fn>(fn), ctx); }

        /// set rx callback
        /// @param fn receive callback function
        template <typename Fn>
        void setRxCallback(Fn&& fn) { rxCallback = etl::forward<Fn>(fn); }

        /// USB transmit non blocking
        /// @param buf data buffer
        /// @param len buffer length
        /// @retval @ref USBD_StatusTypeDef (see usbd_def.h)
        int transmit(const void *buf, uint16_t len) { return CDC_Transmit_FS((uint8_t *)buf, len); }

        /// string transmit operator
        template <size_t N>
        USBD &operator <<(const etl::String<N>& str) { transmit(str.data(), str.len()); return *this; }
        USBD &operator <<(const char *str) { transmit(str, strlen(str)); return *this; }
    };

    extern USBD usb;

} // namespace Project

#endif // PERIPH_USB_H