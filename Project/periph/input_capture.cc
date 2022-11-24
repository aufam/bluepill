#include "periph/input_capture.h"
#include "periph/encoder.h"

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    using namespace Project::Periph;
    if (htim->Instance == encoder1.htim.Instance) encoder1.inputCaptureCallback();
    else return;
}
