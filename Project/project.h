#ifndef PROJECT_H
#define PROJECT_H

#ifdef __cplusplus
#include "oled/oled.h"
#include "periph/adc.h"
#include "periph/encoder.h"
#include "periph/can.h"
#include "periph/exti.h"
#include "periph/rtc.h"
#include "periph/uart.h"
#include "periph/usb.h"
#include "buffer.h"
#include "obd2.h"
#include "os.h"
#include "fstring.h"
extern "C" {
#endif

void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}
#endif

#endif // PROJECT_H