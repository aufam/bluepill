#ifndef PROJECT_ENCODER_H
#define PROJECT_ENCODER_H

#include "../../Core/Inc/tim.h"
#include "etl/function.h"

namespace Project::periph {

    /// rotary encoder using TIM
    /// @note requirements: TIMx encoder mode, TIMx global interrupt
    struct Encoder {
        using Callback = etl::Function<void(), void*>; ///< callback function class

        TIM_HandleTypeDef &htim;    ///< TIM handler configured by cubeMX
        int16_t val = 0;            ///< current value
        Callback incrementCB = {};
        Callback decrementCB = {};

        /// default constructor
        constexpr explicit Encoder(TIM_HandleTypeDef &htim) : htim(htim) {}

        Encoder(const Encoder&) = delete; ///< disable copy constructor
        Encoder(Encoder&&) = delete;      ///< disable move constructor

        Encoder& operator=(const Encoder&) = delete;  ///< disable copy assignment
        Encoder& operator=(Encoder&&) = delete;       ///< disable move assignment

        /// start encoder interrupt
        void init() { HAL_TIM_Encoder_Start_IT(&htim, TIM_CHANNEL_ALL); }

        /// disable encoder interrupt
        void deinit() { HAL_TIM_Encoder_Stop_IT(&htim, TIM_CHANNEL_ALL); }

        /// set increment callback
        /// @param fn increment callback function
        /// @param ctx increment callback function context
        template <typename Fn, typename Ctx>
        void setIncrementCB(Fn&& fn, Ctx* ctx) { incrementCB = Callback(etl::forward<Fn>(fn), ctx); }
        
        /// set increment callback
        /// @param fn increment callback function
        template <typename Fn>
        void setIncrementCB(Fn&& fn) { incrementCB = etl::forward<Fn>(fn); }

        /// set decrement callback
        /// @param fn decrement callback function
        /// @param ctx decrement callback function context
        template <typename Fn, typename Arg>
        void setDecrementCB(Fn&& fn, Arg* ctx) { decrementCB = Callback(etl::forward<Fn>(fn), ctx); }
        
        /// set decrement callback
        /// @param fn decrement callback function
        template <typename Fn>
        void setDecrementCB(Fn&& fn) { incrementCB = etl::forward<Fn>(fn); }

        void inputCaptureCallback() {
            uint16_t counter = htim.Instance->CNT;
            int cnt = counter / 4;
            if (cnt > val) incrementCB();
            if (cnt < val) decrementCB();
            val = static_cast<int16_t> (cnt);
        }
    };

} // namespace Project


#endif // PROJECT_ENCODER_H