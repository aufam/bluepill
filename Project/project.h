#ifndef PROJECT_H
#define PROJECT_H

#ifdef __cplusplus
#include "etl/array.h"
#include "etl/linked_list.h"
#include "etl/mutex.h"
#include "etl/queue.h"
#include "etl/string.h"
#include "etl/thread.h"
#include "etl/time.h"
#include "etl/timer.h"
#include "etl/vector.h"

#include "oled/oled.h"

#include "periph/adc.h"
#include "periph/can.h"
#include "periph/encoder.h"
#include "periph/exti.h"
#include "periph/flash.h"
#include "periph/gpio.h"
#include "periph/pwm.h"
#include "periph/rtc.h"
#include "periph/uart.h"
#include "periph/usb.h"

extern "C" {
#endif

extern char blinkSymbols[16]; ///< blink symbols from default task. '0' is off, '1' is on
extern osThreadId_t defaultTaskHandle; ///< default task for led blink
void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}
#endif

#endif // PROJECT_H
