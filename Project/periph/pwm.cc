#include "periph/pwm.h"

using namespace Project::periph;

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    PWM *pwm;
    if (htim->Instance == pwm3channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) pwm = &pwm3channel1;
    else return;

    pwm->halfCB();
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    PWM *pwm;
    if (htim->Instance == pwm3channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) pwm = &pwm3channel1;
    else return;

    pwm->fullCB();
}
