#ifndef ETL_TUPLE_H
#define ETL_TUPLE_H

#include "etl/utility.h"

namespace Project::etl {
    /// contains the actual value for one item in the tuple.
    /// @tparam i allows the get function to find the value in O(1) time
    template <size_t i, typename Item> struct TupleHead { Item item; };

    /// acts as proxy for the final class that has an extra template parameter `i`.
    template <size_t i, typename... Items> struct TupleImpl;

    /// empty tuple
    template <size_t i> struct TupleImpl<i> {};

    /// recursive specialization
    template <size_t i, typename Item, typename... Items>
    struct TupleImpl<i, Item, Items...> : public TupleHead<i, Item>, public TupleImpl<i + 1, Items...> {};

    /// final class, fixed size container, which holds elements of possibly different types
    template <typename... Items> using
    Tuple = TupleImpl<0, Items...>;

    /// create tuple with variadic template function
    template <typename... Items> constexpr auto
    tuple(const Items&... items) { return Tuple<Items...>{items...}; }

    /// len specifier
    template <typename... T> constexpr size_t
    len(const Tuple<T...>&) { return sizeof...(T); }

    /// obtain a reference to i-th item in a tuple
    template <size_t i, typename Item, typename... Items> constexpr auto&
    get(TupleImpl<i, Item, Items...>& t) { return t.TupleHead<i, Item>::item; }

    template <size_t i, typename Item, typename... Items> constexpr auto&&
    get(TupleImpl<i, Item, Items...>&& t) { return move(get(t)); }

    template <size_t i, typename Item, typename... Items> constexpr const auto&
    get(const TupleImpl<i, Item, Items...>& t) { return t.TupleHead<i, Item>::item; }

    template <size_t i, typename Item, typename... Items> constexpr const auto&&
    get(const TupleImpl<i, Item, Items...>&& t) { return move(get(t)); }

    template <size_t... i, typename T> auto
    tuple_slice(T& t, integer_sequence<size_t, i...>) { return tuple(get<i>(t)...); }

    template <size_t Start, size_t End, typename... Args> auto
    get(const Tuple<Args...>& t) {
        static_assert(Start <= End, "Start cannot be greater than End");
        static_assert(End <= sizeof...(Args), "End cannot be greater than number or Args");
        return tuple_slice(t, make_range_sequence<Start, End>());
    }

    /// type_traits
    template <typename T> struct is_tuple : false_type {};
    template <typename... T> struct is_tuple<Tuple<T...>> : true_type {};
    template <typename... T> struct is_tuple<const Tuple<T...>> : true_type {};
    template <typename... T> struct is_tuple<volatile Tuple<T...>> : true_type {};
    template <typename... T> struct is_tuple<const volatile Tuple<T...>> : true_type {};
    template <typename T> inline constexpr bool is_tuple_v = is_tuple<T>::value;

    template <typename X, typename Y> struct is_pair<Tuple<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<const Tuple<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<volatile Tuple<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<const volatile Tuple<X, Y>> : true_type {};

    template <typename X, typename Y, typename Z> struct is_triple<Tuple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const Tuple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<volatile Tuple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const volatile Tuple<X, Y, Z>> : true_type {};

    template <typename T, typename U, size_t i, size_t N> constexpr bool
    tuple_eq(const T& t, const U& u) {
        if constexpr (i >= N) return true;
        else return get<i>(t) == get<i>(u) && tuple_eq<T, U, i + 1, N>(t, u);
    }

    /// compare operator
    template <typename... T, typename... U> constexpr bool
    operator==(const Tuple<T...>& t, const Tuple<U...>& u) {
        static_assert(sizeof...(T) == sizeof...(U), "tuple objects can only be compared if they have equal sizes.");
        return tuple_eq<Tuple<T...>, Tuple<U...>, 0, sizeof...(T)>(t, u);
    }

    template <typename... T, typename... U> constexpr bool
    operator!=(const Tuple<T...>& t, const Tuple<U...>& u) { return !operator==(t, u); }

    template <typename X1, typename Y1, typename X2, typename Y2> constexpr bool
    operator==(const Tuple<X1, Y1>& t, const Pair<X2, Y2>& p) { return get<0>(t) == p.x && get<1>(t) == p.y; }

    template <typename X1, typename Y1, typename X2, typename Y2> constexpr bool
    operator!=(const Tuple<X1, Y1>& t, const Pair<X2, Y2>& p) { return !operator==(t, p); }

    template <typename X1, typename Y1, typename X2, typename Y2> constexpr bool
    operator==(const Pair<X1, Y1>& p, const Tuple<X2, Y2>& t) { return get<0>(t) == p.x && get<1>(t) == p.y; }

    template <typename X1, typename Y1, typename X2, typename Y2> constexpr bool
    operator!=(const Pair<X1, Y1>& p, const Tuple<X2, Y2>& t) { return !operator==(p, t); }

    template <typename X1, typename Y1, typename Z1, typename X2, typename Y2, typename Z2> constexpr bool
    operator==(const Tuple<X1, Y1, Z1>& t, const Triple<X2, Y2, Z2>& p)
    { return get<0>(t) == p.x && get<1>(t) == p.y && get<2>(t) == p.z; }

    template <typename X1, typename Y1, typename Z1, typename X2, typename Y2, typename Z2> constexpr bool
    operator!=(const Tuple<X1, Y1, Z1>& t, const Triple<X2, Y2, Z2>& p) { return !operator==(t, p); }

    template <typename X1, typename Y1, typename Z1, typename X2, typename Y2, typename Z2> constexpr bool
    operator==(const Triple<X1, Y1, Z1>& p, const Tuple<X2, Y2, Z2>& t)
    { return get<0>(t) == p.x && get<1>(t) == p.y && get<2>(t) == p.z; }

    template <typename X1, typename Y1, typename Z1, typename X2, typename Y2, typename Z2> constexpr bool
    operator!=(const Triple<X1, Y1, Z1>& t, const Tuple<X2, Y2, Z2>& p) { return !operator==(t, p); }
}

// some specializations to enable structure binding
#include <utility>
namespace std {

    template<typename... Items>
    struct tuple_size<Project::etl::Tuple<Items...>> : public Project::etl::integral_constant<size_t, sizeof...(Items)> {};

    template<size_t i, typename Item, typename... Items>
    struct tuple_element<i, Project::etl::Tuple<Item, Items...>> : tuple_element<i - 1, Project::etl::Tuple<Items...>> {};

    template<typename Item, typename... Items>
    struct tuple_element<0, Project::etl::Tuple<Item, Items...>> { typedef Item type; };

    template<size_t i>
    struct tuple_element<i, Project::etl::Tuple<>> { static_assert(i < tuple_size<Project::etl::Tuple<>>::value); };
}

#endif //ETL_TUPLE_H
