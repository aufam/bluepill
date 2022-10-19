#include "periph/adc.h"

namespace Project::Periph {

    void ADC::init() {
        HAL_ADCEx_Calibration_Start(&hadc);
        HAL_ADC_Start_DMA(&hadc, buf, N_CHANNEL);
    }

    void ADC::deinit() {
        HAL_ADC_Stop_DMA(&hadc);
    }

    uint32_t ADC::operator [](size_t index) {
        if (index >= N_CHANNEL) index = N_CHANNEL - 1;
        return buf[index];
    }

    float ADC::getTemp() {
        float V25 = 1.43;
        float Avg_Slope = 0.0043;
        float VSENSE = float(buf[INDEX_TEMP]) * 3.3f / 4096.0f;
        return (V25 - VSENSE) / Avg_Slope + 25;
    }

    float ADC::getVref() {
        return float(buf[0]) * 3.3f / 4096.0f;
    }

} // namespace Project
