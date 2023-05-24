#include "periph/adc.h"

using namespace Project::periph;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    ADCD* adc;
    if (hadc->Instance == adc1.hadc.Instance) adc = &adc1;
    else return;

    adc->completeCallback();
}