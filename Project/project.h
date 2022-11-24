#ifndef PROJECT_H
#define PROJECT_H

#ifdef __cplusplus
#include "eeprom/eeprom.h"

#include "etl/array.h"
#include "etl/linked_list.h"
#include "etl/mutex.h"
#include "etl/queue.h"
#include "etl/string.h"
#include "etl/thread.h"
#include "etl/timer.h"

#include "OBD2/obd2.h"
#include "oled/oled.h"

#include "periph/adc.h"
#include "periph/encoder.h"
#include "periph/flash.h"
#include "periph/can.h"
#include "periph/exti.h"
#include "periph/gpio.h"
#include "periph/rtc.h"
#include "periph/uart.h"
#include "periph/usb.h"

/// user defined literal
constexpr size_t operator ""_s(unsigned long long s)  { return s * 1000; } ///< second to ms
constexpr size_t operator ""_s(double long s) { return size_t(s * 1000); } ///< second to ms
constexpr size_t operator ""_m(unsigned long long m)  { return m * 60_s; } ///< minute to ms
constexpr size_t operator ""_m(double long m) { return size_t(m * 60_s); } ///< minute to ms
constexpr size_t operator ""_h(unsigned long long h)  { return h * 60_m; } ///< hour to ms
constexpr size_t operator ""_h(double long h) { return size_t(h * 60_m); } ///< hour to ms

extern "C" {
#endif

extern char blinkSymbols[16]; ///< blink symbols from default task. '0' is off, '1' is on
extern osThreadId_t defaultTaskHandle; ///< default task for led blink
void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}
#endif

#endif // PROJECT_H
