#ifndef PROJECT_H
#define PROJECT_H

#ifdef __cplusplus
extern "C" {
#endif

extern char blinkSymbols[16]; ///< blink symbols from default task. '0' is off, '1' is on
extern osThreadId_t defaultTaskHandle; ///< default task for led blink
void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}

#include "periph/all.h"

namespace Project::periph {
    ADCD adc1 {.hadc=hadc1};
    PWM pwm1channel1 {.htim=htim1, .channel=TIM_CHANNEL_1};
}

#endif
#endif // PROJECT_H
