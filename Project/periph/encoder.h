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

        /// set callback, and start encoder interrupt
        template <typename IncArg, typename DecArg>
        void init(void (*incFn)(IncArg*), IncArg* incArg, void (*decFn)(DecArg*), DecArg* decArg) {
            setIncrementCB(incFn, incArg);
            setDecrementCB(decFn, decArg);
            HAL_TIM_Encoder_Start_IT(&htim, TIM_CHANNEL_ALL);
        }

        /// set callback, and start encoder interrupt
        void init(void (*incFn)() = nullptr, void (*decFn)() = nullptr) {
            init<void, void>((void(*)(void*)) incFn, nullptr, (void(*)(void*)) decFn, nullptr);
        }

        /// disable encoder interrupt
        void deinit() { HAL_TIM_Encoder_Stop_IT(&htim, TIM_CHANNEL_ALL); }

        /// set increment callback
        /// @param fn increment callback function pointer
        /// @param arg increment callback function argument
        template <typename Arg>
        void setIncrementCB(void (*fn)(Arg*), Arg* arg) { incrementCB = { (void(*)(void*)) fn, (void*) arg }; }
        
        /// set increment callback
        /// @param fn increment callback function pointer
        void setIncrementCB(void (*fn)()) { incrementCB = { (void(*)(void*)) fn, nullptr }; }

        /// set decrement callback
        /// @param fn decrement callback function pointer
        /// @param arg decrement callback function argument
        template <typename Arg>
        void setDecrementCB(void (*fn)(Arg*), Arg* arg) { decrementCB = { (void(*)(void*)) fn, (void*) arg }; }
        
        /// set decrement callback
        /// @param fn decrement callback function pointer
        void setDecrementCB(void (*fn)()) { decrementCB = { (void(*)(void*)) fn, nullptr }; }

        void inputCaptureCallback() {
            uint16_t counter = htim.Instance->CNT;
            int cnt = counter / 4;
            if (cnt > val) incrementCB();
            if (cnt < val) decrementCB();
            val = static_cast<int16_t> (cnt);
        }
    };

    inline Encoder encoder1(htim1);

} // namespace Project


#endif // PROJECT_ENCODER_H