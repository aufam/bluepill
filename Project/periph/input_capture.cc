#include "periph/input_capture.h"
#include "periph/encoder.h"

using namespace Project::periph;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    InputCapture* ic;
    if (htim->Instance == inputCaptureTim3Channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) 
        ic = &inputCaptureTim3Channel1;
    else if (htim->Instance == inputCaptureTim3Channel2.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) 
        ic = &inputCaptureTim3Channel2;
    else if (htim->Instance == inputCaptureTim3Channel3.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) 
        ic = &inputCaptureTim3Channel3;
    else if (htim->Instance == inputCaptureTim3Channel4.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) 
        ic = &inputCaptureTim3Channel4;
    else return;

    ic->callback();
}
