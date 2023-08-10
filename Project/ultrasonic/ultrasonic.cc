#include "ultrasonic/ultrasonic.h"
#include "etl/keywords.h"

using namespace Project;

fun Ultrasonic::init() -> void {
    trigger.init(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
    event.init();
    inputCapture.init();
    inputCapture.setCallback(inputCaptureCallback, this);
}

fun Ultrasonic::deinit() -> void {
    trigger.off();
    inputCapture.deinit();
}

fun Ultrasonic::delayUs(uint32_t us) -> void  {
    __HAL_TIM_SET_COUNTER(&inputCapture.htim, 0);
    while (__HAL_TIM_GET_COUNTER(&inputCapture.htim) < us);
}

fun Ultrasonic::read() -> float {
    trigger.on();
    delayUs(10);
    trigger.off();

    inputCapture.enable();
    distance = NAN;
    event.waitFlags(inputCapture.channel, osFlagsWaitAny, timeout);
    inputCapture.disable();
    return distance;
}

fun Ultrasonic::inputCaptureCallback(Ultrasonic* self) -> void {
    auto& isRising = self->isRising;
    auto& valueRising = self->valueRising;
    auto& valueFalling = self->valueFalling;
    auto& channel = self->inputCapture.channel;
    auto* htim = &self->inputCapture.htim;

    if (isRising) {
        valueRising = HAL_TIM_ReadCapturedValue(htim, channel);
        isRising = false;
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_FALLING);
        return;
    }

    valueFalling = HAL_TIM_ReadCapturedValue(htim, channel);
    __HAL_TIM_SET_COUNTER(htim, 0);
    isRising = true;

    auto diff = valueFalling > valueRising ? valueFalling - valueRising : 0xFFFF - valueRising + valueFalling;
    self->distance = (float) diff * factor;

    __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_RISING);
    self->inputCapture.disable();
    self->event | channel;
}