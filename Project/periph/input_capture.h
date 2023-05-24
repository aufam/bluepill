#ifndef PERIPH_INPUT_CAPTURE_H
#define PERIPH_INPUT_CAPTURE_H

#include "../../Core/Inc/tim.h"
#include "etl/function.h"

namespace Project::periph {

    /// input capture
    /// @note requirements: TIMx input capture mode, TIMx global interrupt
    struct InputCapture {
        using Callback = etl::Function<void(), void*>; ///< callback function class

        TIM_HandleTypeDef& htim; ///< TIM handler configured by cubeMX
        uint32_t channel;        ///< TIM_CHANNEL_x
        Callback callback = {};  ///< capture callback function

        /// default constructor
        constexpr InputCapture(TIM_HandleTypeDef& htim, uint32_t channel) : htim(htim), channel(channel) {}

        InputCapture(const InputCapture&) = delete; ///< disable copy constructor
        InputCapture(InputCapture&&) = delete;      ///< disable copy assignment

        InputCapture& operator=(const InputCapture&) = delete;  ///< disable move constructor
        InputCapture& operator=(InputCapture&&) = delete;       ///< disable move assignment

        /// set callback and start TIM IC interrupt
        /// @param fn callbcak callback function pointer
        /// @param arg callbcak callback function argument
        template <typename Arg>
        void init(void (*fn)(Arg*), Arg* arg) { 
            setCallback(fn, arg); 
            HAL_TIM_IC_Start_IT(&htim, channel);
        }

        /// set callback and start TIM IC interrupt
        /// @param fn callbcak callback function pointer
        void init(void (*fn)() = nullptr) { init<void>((void(*)(void*)) fn, nullptr);}

        /// stop TIM IC interrupt
        void deinit() { HAL_TIM_IC_Stop_IT(&htim, channel); setCallback(nullptr); }

        /// set callback
        /// @param fn callback function pointer
        /// @param arg callback function argument
        template <typename Arg>
        void setCallback(void (*fn)(Arg*), Arg* arg) { callback = { (void(*)(void*)) fn, (void*) arg }; }

        /// set callback
        /// @param fn callback function pointer
        void setCallback(void (*fn)()) { callback = { (void(*)(void*)) fn, nullptr }; }

        /// enable interrupt
        void enable() {
            switch (channel) {
                case TIM_CHANNEL_1: __HAL_TIM_ENABLE_IT(&htim, TIM_IT_CC1); break;
                case TIM_CHANNEL_2: __HAL_TIM_ENABLE_IT(&htim, TIM_IT_CC2); break;
                case TIM_CHANNEL_3: __HAL_TIM_ENABLE_IT(&htim, TIM_IT_CC3); break;
                case TIM_CHANNEL_4: __HAL_TIM_ENABLE_IT(&htim, TIM_IT_CC4); break;
                default: break;
            }
        }

        /// disable interrupt
        void disable() {
            switch (channel) {
                case TIM_CHANNEL_1: __HAL_TIM_DISABLE_IT(&htim, TIM_IT_CC1); break;
                case TIM_CHANNEL_2: __HAL_TIM_DISABLE_IT(&htim, TIM_IT_CC2); break;
                case TIM_CHANNEL_3: __HAL_TIM_DISABLE_IT(&htim, TIM_IT_CC3); break;
                case TIM_CHANNEL_4: __HAL_TIM_DISABLE_IT(&htim, TIM_IT_CC4); break;
                default: break;
            }
        }

        /// set capture polarity
        /// @param polarity TIM_INPUTCHANNELPOLARITY_xxx
        void setPolarity(uint32_t polarity) { __HAL_TIM_SET_CAPTUREPOLARITY(&htim, channel, polarity); }

        /// set counter TIMx->CNT
        /// @param value desired value
        void setCounter(uint32_t value) { __HAL_TIM_SET_COUNTER(&htim, value); }

        /// read captured value TIMx->CCRy
        uint32_t read() { return HAL_TIM_ReadCapturedValue(&htim, channel); }
    };

}

#endif //PERIPH_INPUT_CAPTURE_H
