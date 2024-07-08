#ifndef PROJECT_H
#define PROJECT_H

#include "etl/async.h"
#include "etl/mutex.h"
#include "periph/all.h"
#include "oled/oled.h"
#include "wizchip/ethernet.h"

extern "C" {
    extern char blinkSymbols[16];
    extern int blinkIsRunning;
    extern uint32_t blinkDelay;
    void panic(const char* msg);
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

namespace Project {
    extern etl::Tasks tasks;
    extern etl::Mutex mutex;
    extern etl::String<128> f;
    extern Oled oled;
    extern wizchip::Ethernet ethernet;
    class App;
}

class Project::App {
    typedef void(*function_t)();
    static function_t functions[16];
    static const char* names[16];
    static int cnt;

public:
    App(const char* name, function_t test);
    static void run(const char* filter = "*");
};

#define APP(name) \
    static void unit_app_function_##name(); \
    static ::Project::App unit_app_##name(#name, unit_app_function_##name); \
    static void unit_app_function_##name()

#endif // PROJECT_H
