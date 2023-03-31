#include "periph/exti.h"
#include "cmsis_os2.h" // osKernelGetTickCount

using namespace Project::Periph;
using Project::etl::enumerate;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint16_t static pinNow;
    uint32_t static timeNow;

    auto pinPrev = pinNow;
    auto timePrev = timeNow;
    timeNow = osKernelGetTickCount();
    pinNow = GPIO_Pin;
    if (timeNow - timePrev < Exti::debounceDelay and pinNow == pinPrev) return;

    uint32_t b = 1;
    for (auto [i, callback] : enumerate(exti.callbacks))
        if ((b << i) & GPIO_Pin) callback();
}

void Exti::setCallback(uint16_t pin, Callback::Fn fn, void *arg) {
    uint32_t b = 1;
    for (auto [i, callback] : enumerate(exti.callbacks))
        if ((b << i) & pin) callback = { fn, arg };
}
