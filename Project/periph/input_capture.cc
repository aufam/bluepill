#include "periph/input_capture.h"
#include "periph/encoder.h"

using namespace Project::periph;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == encoder1.htim.Instance) encoder1.inputCaptureCallback();
    else return;
}
