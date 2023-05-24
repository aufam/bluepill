#ifndef PROJECT_ULTRASONIC_H
#define PROJECT_ULTRASONIC_H

#include "periph/input_capture.h"
#include "periph/gpio.h"
#include "etl/queue.h"
#include <cmath>

namespace Project {

    /// generic distance measurement using ultrasonic time flight
    class Ultrasonic {
        using Event = etl::Event;
        static constexpr float factor = .034f / 2.f; ///< convert time in us to distance in cm
        static constexpr uint32_t timeout = 100;     ///< timeout reading in ms

        inline static Event event = {};

        uint32_t valueRising = 0, valueFalling = 0;
        bool isRising = true;
        float distance = NAN;

    public:
        periph::InputCapture& inputCapture;
        periph::GPIO trigger;

        /// default constructor
        constexpr Ultrasonic(periph::InputCapture& inputCapture, const periph::GPIO& trigger)
        : inputCapture(inputCapture)
        , trigger(trigger) {}

        Ultrasonic(const Ultrasonic&) = delete; ///< disable copy constructor
        Ultrasonic(Ultrasonic&&) = delete;      ///< disable copy assignment

        Ultrasonic& operator=(const Ultrasonic&) = delete;  ///< disable move constructor
        Ultrasonic& operator=(Ultrasonic&&) = delete;       ///< disable move assignment

        /// reset trigger pin, init event and input capture
        void init() {
            trigger.init(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
            event.init();
            inputCapture.init(inputCaptureCallback, this);
        }

        /// reset trigger pin, deinit event and deinit input capture
        void deinit() {
            trigger.off();
            inputCapture.deinit();
        }

        /// delay microsecond
        void delayUs(uint32_t us) {
            __HAL_TIM_SET_COUNTER(&inputCapture.htim, 0);
            while (__HAL_TIM_GET_COUNTER(&inputCapture.htim) < us);
        }

        /// read distance in blocking mode
        /// @retval distance in cm
        float read() {
            trigger.on();
            delayUs(10);
            trigger.off();

            inputCapture.enable();
            distance = NAN;
            event.waitFlags(inputCapture.channel, osFlagsWaitAny, timeout);
            inputCapture.disable();
            return distance;
        }

        static void inputCaptureCallback(Ultrasonic* self) {
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
            auto diff = valueFalling > valueRising ? valueFalling - valueRising : 0xFFFF - valueRising + valueFalling;
            self->distance = (float) diff * factor;
            isRising = true;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_RISING);
            self->inputCapture.disable();
            self->event | channel;
        }
    };

}

#endif //PROJECT_ULTRASONIC_H
