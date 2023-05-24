#ifndef ETL_RANGE_H
#define ETL_RANGE_H

#include "etl/utility_basic.h"

namespace Project::etl {

    /// python-like range
    template <typename T, typename U = remove_unsigned_t<T>>
    class Range {
        T first, last;
        U step;

    public:
        constexpr Range(T first, T last, U step) : first(first), last(last), step(step) {}

        [[nodiscard]] 
        constexpr size_t len() const { return operator bool() ? (last - first) / step : 0; }
        
        constexpr Range begin() const { return *this; }
        constexpr Range end()   const { return *this; }

        constexpr explicit operator bool() const { return step == U(0) ? false : step < U(0) ? first > last : first < last; }
        constexpr bool operator!=(const Range&) const { return operator bool(); }
        constexpr void operator++() { first += step; }

        constexpr T operator*() { return first; }

        constexpr T operator()() { 
            auto valid = operator bool();
            auto res = valid ? first : T();
            if (valid) operator++();
            return res;
        }

        void setStep(U s) { step = s; }
    };

    /// create range object
    template <typename T> constexpr enable_if_t<is_arithmetic_v<T>, Range<T>>
    range(T last) { return Range(T(0), last, remove_unsigned_t<T>(1)); }

    /// create range object
    template <typename T> constexpr enable_if_t<is_arithmetic_v<T>, Range<T>>
    range(T first, T last, remove_unsigned_t<T> step = 1) { return Range(first, last, step); }

    /// iter specialization for range object
    template <typename T> constexpr auto
    iter(Range<T> r) { return r; }
} 

#endif // ETL_RANGE_H
