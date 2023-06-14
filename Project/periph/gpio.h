#ifndef PERIPH_GPIO_H
#define PERIPH_GPIO_H

#include "main.h"
#include "cmsis_os2.h"

#define GPIO_ACTIVE_HIGH Project::periph::GPIO::activeHigh
#define GPIO_ACTIVE_LOW  Project::periph::GPIO::activeLow

namespace Project::periph {

    struct GPIO {
        enum { activeLow, activeHigh };

        GPIO_TypeDef *port;     ///< GPIOx
        uint16_t pin;           ///< GPIO_PIN_x
        bool activeMode;        ///< activeLow or activeHigh

        constexpr GPIO(GPIO_TypeDef *port, uint16_t pin, bool activeMode)
        : port(port)
        , pin(pin)
        , activeMode(activeMode) {}

        /// init GPIO
        static void init(GPIO_TypeDef *port,                    ///< GPIOx
                         uint32_t pin,                          ///< GPIO_PIN_x
                         uint32_t mode,                         ///< GPIO_MODE_xxx
                         uint32_t pull = GPIO_NOPULL,           ///< @ref GPIO_NOPULL (default), @ref GPIO_PULLUP, @ref GPIO_PULLDOWN
                         uint32_t speed = GPIO_SPEED_FREQ_LOW   ///< @ref GPIO_SPEED_FREQ_LOW (default), @ref GPIO_SPEED_FREQ_MEDIUM, @ref GPIO_SPEED_FREQ_HIGH
        ) {
            if (port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
            if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
            if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
            if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
            if (port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();

            GPIO_InitTypeDef gpioInitStruct = { .Pin = pin,
                                                .Mode = mode,
                                                .Pull = pull,
                                                .Speed = speed };
            HAL_GPIO_Init(port, &gpioInitStruct);
        }

        /// init GPIO and turn off
        /// @param mode GPIO_MODE_xxx
        /// @param pull @ref GPIO_NOPULL (default), @ref GPIO_PULLUP, @ref GPIO_PULLDOWN
        /// @param speed @ref GPIO_SPEED_FREQ_LOW (default), @ref GPIO_SPEED_FREQ_MEDIUM, @ref GPIO_SPEED_FREQ_HIGH
        void init(uint32_t mode, uint32_t pull = GPIO_NOPULL, uint32_t speed = GPIO_SPEED_FREQ_LOW) const {
            init(port, pin, mode, pull, speed);
            if (mode == GPIO_MODE_OUTPUT_OD || mode == GPIO_MODE_OUTPUT_PP)
                off();
        }

        /// write pin high (true) or low (false)
        void write(bool highLow) const { HAL_GPIO_WritePin(port, pin, highLow ? GPIO_PIN_SET : GPIO_PIN_RESET); }

        /// toggle pin
        void toggle() const { HAL_GPIO_TogglePin(port, pin); }

        /// read pin
        /// @retval high (true) or low (false)
        [[nodiscard]] bool read() const { return HAL_GPIO_ReadPin(port, pin); }

        /// turn on
        /// @param ticks sleep for a while. default = 0
        void on(uint32_t ticks = 0) const {
            write(activeMode);
            osDelay(ticks);
        }

        /// turn off
        /// @param ticks sleep for a while. default = 0
        void off(uint32_t ticks = 0) const {
            write(!activeMode);
            osDelay(ticks);
        }

        [[nodiscard]] bool isOn() const { return !(read() ^ activeMode); }
        [[nodiscard]] bool isOff() const { return (read() ^ activeMode); }

    };

} // periph

#endif //PERIPH_GPIO_H
