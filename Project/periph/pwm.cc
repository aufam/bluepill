#include "periph/pwm.h"

using namespace Project::periph;

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    PWM *pwm;
    if (htim->Instance == pwm8channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) 
        pwm = &pwm8channel1;
    else if (htim->Instance == pwm8channel2.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) 
        pwm = &pwm8channel2;
    else if (htim->Instance == pwm12channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) 
        pwm = &pwm12channel1;
    else if (htim->Instance == pwm12channel2.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) 
        pwm = &pwm12channel2;
    else return;

    pwm->halfCB();
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    PWM *pwm;
    if (htim->Instance == pwm8channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) 
        pwm = &pwm8channel1;
    else if (htim->Instance == pwm8channel2.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) 
        pwm = &pwm8channel2;
    else if (htim->Instance == pwm12channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) 
        pwm = &pwm12channel1;
    else if (htim->Instance == pwm12channel2.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) 
        pwm = &pwm12channel2;
    else return;

    pwm->fullCB();
}
