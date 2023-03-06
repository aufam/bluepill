#include "periph/adc.h"

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    using namespace Project::Periph;
    ADC* adc;
    if (hadc->Instance == adc1.hadc.Instance) adc = &adc1;
    else return;

    adc->completeCallback();
}