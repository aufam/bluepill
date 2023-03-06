#ifndef PERIPH_EXTI_H
#define PERIPH_EXTI_H

#include "main.h"
#include "etl/function.h"

namespace Project::Periph {

    /// external interrupt class
    struct Exti {
        /// callback function class
        using Callback = etl::Function<void(), void*>;

        Callback callbacks[16] = {}; ///< list of callback function of every GPIO pin
        static const uint32_t debounceDelay = 250; ///< button debounce delay in ticks
        constexpr Exti() = default;

        /// set exti callback of given GPIO pin
        /// @param pin GPIO_PIN_X see stm32fXxx_hal_gpio.h
        /// @param fn function pointer
        /// @param arg function argument
        void setCallback(uint16_t pin, Callback::Fn fn = nullptr, void *arg = nullptr);
    };

    inline Exti exti;

} // namespace Project

#endif // PERIPH_EXTI_H