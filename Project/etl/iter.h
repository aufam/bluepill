#ifndef ETL_ITER_H
#define ETL_ITER_H

#include "etl/utility_basic.h"

namespace Project::etl {
    
    /// python-like iter
    template <typename Iterator>
    class Iter {
        Iterator first, last;
        int step;

    public:
        constexpr Iter(Iterator first, Iterator last, int step = 1) : first(first), last(last), step(step) {}

        [[nodiscard]] 
        constexpr size_t len() const { return step == 0 ? 0 : step > 0 ? (last - first) / step : (first - last) / (-step); }

        constexpr Iter begin() const { return *this; }
        constexpr Iter end()   const { return *this; }

        constexpr decltype(auto) operator[](int i) { 
            if (i < 0) i = len() + i; // allowing negative index
            return *(first + (step * i)); 
        }

        constexpr decltype(auto) operator[](int i) const { 
            if (i < 0) i = len() + i; // allowing negative index
            return *(first + (step * i)); 
        }

        constexpr explicit operator bool() const { return step != 0 && first != last; }

        constexpr bool operator!=(const Iter&) const { return operator bool(); }

        constexpr void operator++() {
            if (step == 0) return;
            int inc = step > 0 ? 1 : -1;
            for (int i = 0; i != step; i += inc) {
                if (first == last) return;
                first = first + inc;
            }
        }

        constexpr decltype(auto) operator*() { return *first; }

        constexpr auto operator()() {
            using R = remove_reference_t<decltype(*first)>;
            auto valid = operator bool();
            R res = valid ? *first : R{};
            if (valid) operator++();
            return res;
        }
    };

    /// create iter object from iterator
    template <typename Iterator> constexpr auto
    iter(Iterator first, Iterator last, int step = 1) { return Iter(first, last, step); }

    /// create iter object from a container
    template <typename Container> constexpr auto
    iter(Container&& cont, int step = 1) 
    { return step >= 0 ? Iter(etl::begin(cont), etl::end(cont), step) : Iter(etl::end(cont) - 1, etl::begin(cont) - 1, step); }

    /// create reversed iter object from a container
    template <typename Container> constexpr auto
    reversed(Container&& cont) { return etl::iter(cont, -1); }

    /// iter specialization for iter object
    template <typename Iterator> constexpr auto
    iter(Iter<Iterator> iterator) { return iterator; }
}

#endif // ETL_MOVE_H