#ifndef ETL_ARRAY_H
#define ETL_ARRAY_H

#include "etl/algorithm.h"

namespace Project::etl {

    template <typename T, size_t N> 
    struct array_traits {
        static_assert(N > 0, "Array size can't be zero");
        typedef T type[N];
        static constexpr T* ptr(const type& buf) { return const_cast<T*>(buf); }
        static constexpr T& ref(const type& buf, int i) {
            if (i < 0) i = N + i; // allowing negative index
            return const_cast<T&>(buf[i]);
        }
    };

    template <typename T, size_t N> using array_traits_t = typename array_traits<T, N>::type;

    /// static contiguous array
    template <class T, size_t N>
    struct Array {
        typedef T value_type;
        typedef array_traits<T, N> traits;
        typedef array_traits_t<T, N> traits_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;

        traits_type buf;

        static constexpr size_t size() { return N; }
        [[nodiscard]] constexpr size_t len() const { return N; }

        constexpr iterator data()   { return traits::ptr(buf); }
        constexpr iterator begin()  { return (T*)(data()); }
        constexpr iterator end()    { return (T*)(data() + N); }
        constexpr reference front() { return *begin(); }
        constexpr reference back()  { return N ? *(end() - 1) : *end(); }

        constexpr const_iterator data()   const { return traits::ptr(buf); }
        constexpr const_iterator begin()  const { return (const T*)(data()); }
        constexpr const_iterator end()    const { return (const T*)(data() + N); }
        constexpr const_reference front() const { return traits::ref(buf, 0); }
        constexpr const_reference back()  const { return traits::ref(buf, N - 1); }

        constexpr reference operator [](int i) { return traits::ref(buf, i); }
        constexpr const_reference operator [](int i) const { return traits::ref(buf, i); }
    };

    /// create array with variadic template function, array size is deduced
    template<typename T, typename... U> constexpr Array<enable_if_t<(is_same_v<T, U> && ...), T>, 1 + sizeof...(U)>
    array(const T& t, const U&... u) { return Array<T, 1 + sizeof...(U)>{t, u...}; }

    /// cast reference from iterator
    template <class T, size_t N> constexpr Array<T, N>&
    array_cast(T* a) { return *reinterpret_cast<Array<T, N>*>(a); }

    /// cast const reference from iterator
    template <class T, size_t N> constexpr const Array<T, N>&
    array_cast(const T* a) { return *reinterpret_cast<const Array<T, N>*>(a); }

    /// cast reference from traditional array
    template <class T, size_t N> constexpr Array<T, N>&
    array_cast(T (&a)[N]) { return *reinterpret_cast<Array<T, N>*>(a); }

    /// cast const reference from traditional array
    template <class T, size_t N> constexpr const Array<T, N>&
    array_cast(const T (&a)[N]) { return *reinterpret_cast<const Array<T, N>*>(a); }

    /// get functions
    template<int i, typename T, size_t N> constexpr T&
    get(Array<T, N>& arr) { return array_traits<T, N>::ref(arr.buf, i); }

    template<int i, typename T, size_t N> constexpr T&&
    get(Array<T, N>&& arr) { return move(get<i>(arr)); }

    template<int i, typename T, size_t N> constexpr const T&
    get(const Array<T, N>& arr) { return array_traits<T, N>::ref(arr.buf, i); }

    template<int i, typename T, size_t N> constexpr const T&&
    get(const Array<T, N>&& arr) { return move(get<i>(arr)); }

}

//// some specializations to enable structure binding
#include <utility>
namespace std {

    template <typename T, size_t N>
    struct tuple_size<Project::etl::Array<T, N>> : public Project::etl::integral_constant<size_t, N> {};

    template <size_t i, typename T, size_t N>
    struct tuple_element<i, Project::etl::Array<T, N>> { static_assert(i < N); typedef T type; };
}
#endif //ETL_ARRAY_H
