#ifndef ETL_FILTER_H
#define ETL_FILTER_H

#include "etl/iter.h"

namespace Project::etl {

    template <typename Sequence, typename UnaryPredicate>
    class Filter {
        Sequence sequence;
        UnaryPredicate fn;

    public:
        constexpr Filter(Sequence seq, UnaryPredicate fn) : sequence(seq), fn(fn) {}

        constexpr Filter begin() const { return *this; }
        constexpr Filter end()   const { return *this; }

        constexpr explicit operator bool() const { return bool(sequence); }

        constexpr bool operator!=(const Filter&) const { return operator bool(); }
        
        constexpr void operator++() { ++sequence; }

        constexpr decltype(auto) operator*() { 
            while (operator bool()) {
                if (fn(*sequence)) return *sequence;
                ++sequence;
            }
            return *sequence; 
        }

        constexpr auto operator()() {   
            using R = remove_reference_t<decltype(*sequence)>;
            auto valid = operator bool();
            R res = valid ? operator*() : R{};
            if (valid) operator++();
            return res;
        }
    };

    /// create filter object from iterator
    template <typename Iterator, typename UnaryPredicate> constexpr auto
    filter(Iterator first, Iterator last, UnaryPredicate fn, int step = 1) { return Filter(etl::iter(first, last, step), fn); }

    /// create filter object from a sequence
    template <typename Sequence, typename UnaryPredicate> constexpr auto
    filter(Sequence&& seq, UnaryPredicate fn) { return Filter(etl::iter(seq), fn); }

    /// iter specialization for filter object
    template <typename Sequence, typename UnaryPredicate> constexpr auto
    iter(Filter<Sequence, UnaryPredicate> t) { return t; }

    template <typename UnaryPredicate>
    struct FilterFunction { UnaryPredicate fn; };

    /// create filter function
    template <typename UnaryPredicate> constexpr auto
    filter(UnaryPredicate fn) { return FilterFunction<UnaryPredicate> { fn }; }

    /// pipe operator to apply filter function to a sequence
    template <typename Sequence, typename UnaryPredicate> constexpr auto
    operator|(Sequence&& seq, FilterFunction<UnaryPredicate> fn) { return etl::filter(seq, fn.fn); }
} 

#endif // ETL_FILTER_H