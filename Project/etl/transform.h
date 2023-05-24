#ifndef ETL_TRANSFORM_H
#define ETL_TRANSFORM_H

#include "etl/iter.h"

namespace Project::etl {

    template <typename Sequence, typename UnaryFunction>
    class Transform {
        Sequence sequence;
        UnaryFunction fn;

    public:
        constexpr Transform(Sequence seq, UnaryFunction fn) : sequence(seq), fn(fn) {}

        constexpr Transform begin() const { return *this; }
        constexpr Transform end()   const { return *this; }

        constexpr explicit operator bool() const { return bool(sequence); }

        constexpr bool operator!=(const Transform&) const { return operator bool(); }
        
        constexpr void operator++() { ++sequence; }

        constexpr decltype(auto) operator*() { return fn(*sequence); }

        constexpr auto operator()() { 
            using R = remove_reference_t<decltype(fn(*sequence))>;
            auto valid = operator bool();
            R res = valid ? operator*() : R{};
            if (valid) operator++();
            return res;
        }
    };

    /// create transform object from iterator
    template <typename Iterator, typename UnaryFunction> constexpr auto
    transform(Iterator first, Iterator last, UnaryFunction fn, int step = 1) { return Transform(etl::iter(first, last, step), fn); }

    /// create transform object from a sequence
    template <typename Sequence, typename UnaryFunction> constexpr auto
    transform(Sequence&& seq, UnaryFunction fn) { return Transform(etl::iter(seq), fn); }

    /// iter specialization for transform object
    template <typename Sequence, typename UnaryFunction> constexpr auto
    iter(Transform<Sequence, UnaryFunction> t) { return t; }

    template <typename UnaryFunction>
    struct TransformFunction { UnaryFunction fn; };

    /// create transform function
    template <typename UnaryFunction> constexpr auto
    transform(UnaryFunction fn) { return TransformFunction<UnaryFunction> { fn }; }

    /// pipe operator to apply transform function to a sequence
    template <typename Sequence, typename UnaryFunction> constexpr auto
    operator|(Sequence&& seq, TransformFunction<UnaryFunction> fn) { return etl::transform(seq, fn.fn); }
} 

#endif // ETL_TRANSFORM_H
