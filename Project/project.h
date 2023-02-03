#ifndef PROJECT_H
#define PROJECT_H

#ifdef __cplusplus
#include "etl/array.h"
#include "etl/linked_list.h"
#include "etl/mutex.h"
#include "etl/queue.h"
#include "etl/string.h"
#include "etl/thread.h"
#include "etl/time.h"
#include "etl/timer.h"
#include "etl/vector.h"

#include "oled/oled.h"

#include "periph/adc.h"
#include "periph/can.h"
#include "periph/encoder.h"
#include "periph/exti.h"
#include "periph/flash.h"
#include "periph/gpio.h"
#include "periph/pwm.h"
#include "periph/rtc.h"
#include "periph/uart.h"
#include "periph/usb.h"

namespace Project {
    /// find the minimum number given a series of numbers with variadic function
    template <class T1, class T2, class... Tn>
    constexpr auto min(const T1& val1, const T2& val2, const Tn&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 < val2 ? val1 : val2;
        else return min(min(val1, val2), vals...);
    }

    /// find the maximum number given a series of numbers with variadic function
    template <class T1, class T2, class... Tn>
    constexpr auto max(const T1& val1, const T2& val2, const Tn&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 > val2 ? val1 : val2;
        else return max(max(val1, val2), vals...);
    }

    /// absolute number
    template <class T> constexpr T absolute(T x) { return x < 0 ? -x : x; }

    /// truncate a number
    /// @tparam T number type
    /// @param x given number
    /// @param low lower limit
    /// @param upp upper limit
    /// @return truncated number
    template <class T> constexpr T truncate(T x, T low, T upp) { return x > upp ? upp : x < low ? low : x; }

    /// simple pair struct
    template <class X, class Y> struct Pair { X x; Y y; };

    /// interpolate x given [x1, y1] and [x2, y2]
    /// @tparam X type of x
    /// @tparam Y type of y
    /// @param trim truncate the result to range (y1, y2). default = true
    /// @{
    template <class X, class Y>
    constexpr Y interpolate(const X& x, const X& x1, const X& x2, const Y& y1, const Y& y2, bool trim = true) {
        Y res = y1 + (Y) ((float) (y2 - y1) * (float) (x - x1) / (float) (x2 - x1));
        return trim ? truncate(res, min(y1, y2), max(y1, y2)) : res;
    }
    template <class X, class Y>
    constexpr Y interpolate(const X& x, const Pair<X,Y>& p1, const Pair<X,Y>& p2, bool trim = true) {
        return interpolate(x, p1.x, p2.x, p1.y, p2.y, trim);
    }
}

extern "C" {
#endif

extern char blinkSymbols[16]; ///< blink symbols from default task. '0' is off, '1' is on
extern osThreadId_t defaultTaskHandle; ///< default task for led blink
void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}
#endif

#endif // PROJECT_H
