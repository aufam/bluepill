#ifndef PROJECT_H
#define PROJECT_H

#ifdef __cplusplus
#include "etl/array.h"
#include "etl/linked_list.h"
#include "etl/mutex.h"
#include "etl/queue.h"
#include "etl/string.h"
#include "etl/thread.h"
#include "etl/timer.h"

#include "periph/adc.h"
#include "periph/encoder.h"
#include "periph/can.h"
#include "periph/exti.h"
#include "periph/rtc.h"
#include "periph/uart.h"
#include "periph/usb.h"

#include "eeprom.h"
#include "oled/oled.h"
#include "obd2.h"
extern "C" {
#endif

void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}
#endif

#endif // PROJECT_H