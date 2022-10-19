#ifndef PROJECT_ENCODER_H
#define PROJECT_ENCODER_H

#include "tim.h"
#include "os.h"

namespace Project::Periph {

    /// rotary encoder. requirements: TIMx encoder mode, TIMx global interrupt
    struct Encoder {
        /// callback function class
        struct Callback {
            typedef void (*Function)(void *);
            Function fn;
            void *arg;
        };

        TIM_HandleTypeDef &htim; ///< tim handler generated by cubeMX
        int16_t val = 0; ///< current value
        int16_t valPrev = 0; ///< previous value
        int16_t speed = 0;
        OS::TimerStatic timer;
        Callback incrementCB = {};
        Callback decrementCB = {};
        constexpr explicit Encoder(TIM_HandleTypeDef &htim) : htim(htim) {}

        /// set callback, init os timer, and start encoder
        void init(
                Callback::Function incrementCBFn = nullptr, void *incrementCBArg = nullptr,
                Callback::Function decrementCBFn = nullptr, void *decrementCBArg = nullptr
        );
        /// deinit os timer, and stop encoder
        void deinit();
        /// set increment callback
        /// @param incrementCBFn increment callback function pointer
        /// @param incrementCBArg increment callback function argument
        void setIncrementCB(Callback::Function incrementCBFn = nullptr, void *incrementCBArg = nullptr) {
            incrementCB.fn = incrementCBFn;
            incrementCB.arg = incrementCBArg;
        }
        /// set decrement callback
        /// @param decrementCBFn decrement callback function pointer
        /// @param decrementCBArg decrement callback function argument
        void setDecrementCB(Callback::Function decrementCBFn = nullptr, void *decrementCBArg = nullptr) {
            decrementCB.fn = decrementCBFn;
            decrementCB.arg = decrementCBArg;
        }
    };

    inline Encoder encoder1(htim1);

} // namespace Project


#endif // PROJECT_ENCODER_H