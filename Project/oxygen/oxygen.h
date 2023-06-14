#ifndef PROJECT_OXYGEN_H
#define PROJECT_OXYGEN_H

#include "periph/uart.h"
#include "etl/numerics.h"

#ifndef PROJECT_OXYGEN_IS_USING_NOTIFIER
#define PROJECT_OXYGEN_IS_USING_NOTIFIER 1
#endif

#if PROJECT_OXYGEN_IS_USING_NOTIFIER
#include "etl/event.h"
#endif

namespace Project {

    /// HCO oxygen sensor (concentrate, flow, temperature). UART 9600.
    /// see https://dbshare.delameta.com/sharing/smFpkXieEs
    class Oxygen {
        float concentrate = NAN;         ///< in %
        float flow = NAN;                ///< in lpm
        float temperature = NAN;         ///< in C

    public:
        periph::UART &uart;
#if PROJECT_OXYGEN_IS_USING_NOTIFIER
        etl::Event notifier {};
#endif
        explicit constexpr Oxygen(periph::UART &uart) : uart(uart) {}

        /// init uart and notifier
        void init();

        /// deinit uart and notifier
        void deinit();

        float getConcentrate()  { return concentrate; }
        float getFlow()         { return flow; }
        float getTemperature()  { return temperature; }
    
    private:
        static void rxCallback(Oxygen *self, const uint8_t* buf, size_t len);
    };
}


#endif //PROJECT_OXYGEN_H
