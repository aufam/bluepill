#ifndef PROJECT_H
#define PROJECT_H

#ifdef __cplusplus
#include "periph/adc.h"
#include "periph/can.h"
#include "periph/encoder.h"
#include "periph/exti.h"
#include "periph/flash.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/input_capture.h"
#include "periph/pwm.h"
#include "periph/rtc.h"
#include "periph/uart.h"

extern "C" {
#endif

extern char blinkSymbols[16]; ///< blink symbols from default task. '0' is off, '1' is on
extern osThreadId_t defaultTaskHandle; ///< default task for led blink
void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}
#endif

#endif // PROJECT_H
