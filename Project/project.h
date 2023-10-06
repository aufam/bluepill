#ifndef PROJECT_H
#define PROJECT_H

#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char blinkSymbols[16]; ///< blink symbols from default task. '0' is off, '1' is on
extern osThreadId_t defaultTaskHandle; ///< default task for led blink
void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}

#include "periph/all.h"

// periph instances
namespace Project::periph {
    inline ADCD adc1 { .hadc=hadc1 };
    inline PWM pwm1channel1 { .htim=htim1, .channel=TIM_CHANNEL_1 };
}

#endif

#endif // PROJECT_H
