#include "periph/adc.h"

using namespace Project::periph;

static ADCD* getter(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == adc1.hadc.Instance) 
        return &adc1;
    return nullptr;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    auto adc = getter(hadc);
    if (adc)
        adc->completeCallback();
}