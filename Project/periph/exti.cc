#include "periph/exti.h"
#include "cmsis_os2.h" // osKernelGetTickCount
#include "etl/python_keywords.h"

using namespace Project::Periph;
using Project::etl::enumerate;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

    static uint16_t pinNow;
    static uint32_t timeNow;

    auto pinPrev = pinNow;
    auto timePrev = timeNow;
    timeNow = osKernelGetTickCount();
    pinNow = GPIO_Pin;
    if (timeNow - timePrev < Exti::debounceDelay and pinNow == pinPrev) return;

    uint32_t b = 1;
    for (auto [i, callback] in enumerate(exti.callbacks))
        if ((b << i) & GPIO_Pin) callback();
}

void Exti::setCallback(uint16_t pin, Callback::Fn fn, void *arg) {
    uint32_t b = 1;
    for (auto [i, callback] in enumerate(exti.callbacks))
        if ((b << i) & pin) callback = { fn, arg };
}
