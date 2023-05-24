#ifndef PERIPH_EXTI_H
#define PERIPH_EXTI_H

#include "etl/function.h"

namespace Project::periph {

    /// external interrupt class
    struct Exti {
        using Callback = etl::Function<void(), void*>;  ///< callback function class
        static constexpr uint32_t debounceDelay = 250;  ///< button debounce delay in ticks

        uint16_t isUsingDebounceFilter = 0; ///< check each pin if it requires debounce filter
        Callback callbacks[16] = {};        ///< callback functions for every GPIO pin
        uint32_t counters[16] = {};         ///< counts how many times for each GPIO pin has been triggered

        constexpr Exti() = default; ///< default constructor

        Exti(const Exti&) = delete; ///< disable copy constructor
        Exti(Exti&&) = delete;      ///< disable move constructor

        Exti& operator=(const Exti&) = delete;  ///< disable copy assignment
        Exti& operator=(Exti&&) = delete;       ///< disable move assignment

        /// set exti callback of given GPIO pin
        /// @param pin GPIO_PIN_X see stm32fXxx_hal_gpio.h
        /// @param fn function pointer
        /// @param arg function argument
        /// @param useDebounceFilter true (default): use debounce filter, false: don't
        void setCallback(uint16_t pin, void (*fn)(void*), void* arg, bool useDebounceFilter = true);

        template <typename Arg>
        void setCallback(uint16_t pin, void (*fn)(Arg*), Arg* arg, bool useDebounceFilter = true) {
            setCallback(pin, (void (*)(void*)) fn, (void*) arg, useDebounceFilter);
        }

        void setCallback(uint16_t pin, void (*fn)(), bool useDebounceFilter = true) {
            setCallback(pin, (void (*)(void*)) fn, nullptr, useDebounceFilter);
        }

        /// get the number of how many times the pin is triggered
        uint32_t getCounter(uint16_t pin);

        /// set pin counter
        void setCounter(uint16_t pin, uint32_t cnt);

        /// set pin counter
        void resetCounter(uint16_t pin) { setCounter(pin, 0); }


    };

    inline Exti exti;

} // namespace Project

#endif // PERIPH_EXTI_H