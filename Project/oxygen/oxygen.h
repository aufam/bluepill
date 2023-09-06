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

    struct OxygenValues {
        float concentrate = NAN;         ///< in %
        float flow = NAN;                ///< in lpm
        float temperature = NAN;         ///< in C
    };

    /// HCO oxygen sensor (concentrate, flow, temperature). UART 9600.
    /// see https://dbshare.delameta.com/sharing/smFpkXieEs
    class Oxygen {
        OxygenValues& values;

    public:
        periph::UART &uart;
#if PROJECT_OXYGEN_IS_USING_NOTIFIER
        etl::Event notifier;
#endif
        explicit constexpr Oxygen(OxygenValues &values, periph::UART &uart) : values(values), uart(uart) {}

        /// init uart and notifier
        void init();

        /// deinit uart and notifier
        void deinit();

        /// get concentrate in %
        const float& concentrate = values.concentrate;

        /// get flow rate in lpm
        const float& flow = values.flow;

        /// get temperature in degree Celcius
        const float& temperature = values.temperature;
    
    private:
        static void rxCallback(Oxygen *self, const uint8_t* buf, size_t len);
    };
}


#endif //PROJECT_OXYGEN_H
