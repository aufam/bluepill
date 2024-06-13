#ifndef PROJECT_H
#define PROJECT_H

#include "etl/async.h"
#include "etl/mutex.h"
#include "etl/heap.h"
#include "periph/all.h"
#include "oled/oled.h"
#include "wizchip/ethernet.h"

extern "C" {
    extern char blinkSymbols[16];           ///< blink symbols from default task. '0' is off, '1' is on
    extern int blinkIsRunning;              ///< default task running flag
    extern uint32_t blinkDelay;

    void panic(const char* msg);
}

namespace Project {
    extern etl::Tasks tasks;
    extern etl::Mutex mutex;
    extern etl::String<128> f;
    extern Oled oled;
    extern wizchip::Ethernet ethernet;
}

namespace Project::periph {
    extern ADCD adc1;
    extern Encoder encoder1;
    extern I2C i2c2;
    extern PWM pwm3channel1;
    extern UART uart1;
    extern UART uart2;

    #ifdef F103_USE_CAN
    extern CAN can;
    #endif
}

#endif // PROJECT_H
