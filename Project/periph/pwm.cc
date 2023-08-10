#include "periph/pwm.h"

using namespace Project::periph;

static PWM* selector(TIM_HandleTypeDef *htim) {
    if (htim->Instance == pwm1channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        return &pwm1channel1;
    return nullptr;
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    auto pwm = selector(htim);
    if (pwm)
        pwm->halfCB();
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    auto pwm = selector(htim);
    if (pwm)
        pwm->fullCB();
}
