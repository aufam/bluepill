#ifndef ETL_ARRAY_H
#define ETL_ARRAY_H

#include "etl/type_traits.h"

namespace Project::etl {

    /// static contiguous array
    template <class T, size_t N>
    struct Array {
        static_assert(N > 0, "Array size can't be 0");
        typedef T Type;
        T buffer[N];

        static constexpr size_t size() { return N; }
        [[nodiscard]] constexpr size_t len() const { return N; }

        constexpr T* data()   { return buffer; }
        constexpr T* begin()  { return buffer; }
        constexpr T* end()    { return buffer + N; }
        constexpr T& front()  { return buffer[0]; }
        constexpr T& back()   { return buffer[N - 1]; }

        constexpr const T* data()     const { return buffer; }
        constexpr const T* begin()    const { return buffer; }
        constexpr const T* end()      const { return buffer + N; }
        constexpr const T& front()    const { return buffer[0]; }
        constexpr const T& back()     const { return buffer[N - 1]; }

        constexpr T& operator [](size_t i) { return buffer[i % N]; }
        constexpr const T& operator [](size_t i) const { return buffer[i % N]; }
    };

    /// create array with variadic template function, array size is deduced
    template<typename T, typename... U>
    constexpr Array<enable_if_t<(is_same_v<T, U> && ...), T>, 1 + sizeof...(U)>
    array(const T& t, const U&... u) { return Array<T, 1 + sizeof...(U)>{t, u...}; }
}

#endif //ETL_ARRAY_H
