#ifndef PROJECT_ULTRASONIC_H
#define PROJECT_ULTRASONIC_H

#include "periph/input_capture.h"
#include "periph/gpio.h"
#include "etl/event.h"
#include <cmath>

namespace Project {

    /// generic distance measurement using ultrasonic time flight
    class Ultrasonic {
        using Event = etl::Event;
        static constexpr float factor = .034f / 2.f; ///< convert time in us to distance in cm
        static constexpr etl::Time timeout = etl::Time::ms2time(100); ///< timeout reading

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
        void init();

        /// reset trigger pin, deinit event and deinit input capture
        void deinit();

        /// delay microsecond
        void delayUs(uint32_t us);

        /// read distance in blocking mode
        /// @retval distance in cm
        float read();

        static void inputCaptureCallback(Ultrasonic* self);
    };

}

#endif //PROJECT_ULTRASONIC_H
