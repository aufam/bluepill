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
    template <typename T, size_t N>
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
        constexpr iterator begin()  { return (iterator)(data()); }
        constexpr iterator end()    { return (iterator)(data() + N); }
        constexpr reference front() { return *begin(); }
        constexpr reference back()  { return N ? *(end() - 1) : *end(); }

        constexpr const_iterator data()   const { return traits::ptr(buf); }
        constexpr const_iterator begin()  const { return (const_iterator)(data()); }
        constexpr const_iterator end()    const { return (const_iterator)(data() + N); }
        constexpr const_reference front() const { return traits::ref(buf, 0); }
        constexpr const_reference back()  const { return traits::ref(buf, N - 1); }

        constexpr reference operator[](int i) { return traits::ref(buf, i); }
        constexpr const_reference operator[](int i) const { return traits::ref(buf, i); }

        /// slice operator
        constexpr Iter<iterator> operator()(int start, int stop, int step = 1)
        { return start < stop ? iter(&operator[](start), &operator[](stop), step) : iter(begin(), begin(), step); }

        /// slice operator
        constexpr Iter<const_iterator>operator()(int start, int stop, int step = 1) const
        { return start < stop ? iter(&operator[](start), &operator[](stop), step) : iter(begin(), begin(), step); }

        template <class Container>
        constexpr bool operator==(const Container& other) const { return compare_all(*this, other); }

        template <class Container>
        constexpr bool operator!=(const Container& other) const { return !operator==(other); }
    };

    /// create array with variadic template function, array size is deduced
    template <typename T, typename... U> constexpr Array<enable_if_t<(is_same_v<T, U> && ...), T>, 1 + sizeof...(U)>
    array(const T& t, const U&... u) { return Array<T, 1 + sizeof...(U)> { t, u... }; }

    /// create array with default constructed T 
    template <typename T, size_t N> constexpr auto
    array() { return Array<T, N> {}; }

    /// cast reference from any pointer
    template <typename T, size_t N, typename U> constexpr auto&
    array_cast(U* a) { return *reinterpret_cast<conditional_t<is_const_v<U>, const Array<T, N>*, Array<T, N>*>>(a); }

    /// cast reference from any type
    template <typename T = void, typename U, typename V = conditional_t<is_void_v<T>, remove_extent_t<U>, T>> constexpr auto&
    array_cast(U& a) { return array_cast<V, sizeof(U) / sizeof(V)>(&a); }

    /// get functions
    template <int i, typename T, size_t N> constexpr T&
    get(Array<T, N>& arr) { return array_traits<T, N>::ref(arr.buf, i); }

    template <int i, typename T, size_t N> constexpr T&&
    get(Array<T, N>&& arr) { return move(get<i>(arr)); }

    template <int i, typename T, size_t N> constexpr const T&
    get(const Array<T, N>& arr) { return array_traits<T, N>::ref(arr.buf, i); }

    template <int i, typename T, size_t N> constexpr const T&&
    get(const Array<T, N>&& arr) { return move(get<i>(arr)); }

    template <int Start, int End, typename T, size_t N> constexpr auto&
    get(const Array<T, N>& arr) { static_assert(End > Start); return array_cast<T, End - Start>(&arr[Start]); }

    template <int Start, int End, typename T, size_t N> constexpr auto&
    get(Array<T, N>& arr) { static_assert(End > Start); return array_cast<T, End - Start>(&arr[Start]); }

    /// swap specialization, avoid creating large temporary variable 
    template <typename T, typename U> enable_if_t<is_same_v<remove_extent_t<T>, remove_extent_t<U>>>
    swap(T& a, U& b) { swap_element(a, b); }

    /// type traits
    template <typename T, size_t N> struct is_array<Array<T, N>> : true_type {};
    template <typename T, size_t N> struct is_array<const Array<T, N>> : true_type {};
    template <typename T, size_t N> struct is_array<volatile Array<T, N>> : true_type {};
    template <typename T, size_t N> struct is_array<const volatile Array<T, N>> : true_type {};

    template <typename T, size_t N> struct remove_extent<Array<T, N>> { typedef T type; };
    template <typename T, size_t N> struct remove_extent<const Array<T, N>> { typedef T type; };
    template <typename T, size_t N> struct remove_extent<volatile Array<T, N>> { typedef T type; };
    template <typename T, size_t N> struct remove_extent<const volatile Array<T, N>> { typedef T type; };
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
