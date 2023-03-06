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
    template <typename Item, typename... Items> constexpr auto
    tuple(const Item& head, const Items&... items) { return Tuple<Item, Items...>{head, items...}; }

    /// obtain a reference to i-th item in a tuple
    template <size_t i, typename Item, typename... Items> constexpr auto&
    get(TupleImpl<i, Item, Items...>& tuple) { return tuple.TupleHead<i, Item>::item; }

    template <size_t i, typename Item, typename... Items> constexpr auto&&
    get(TupleImpl<i, Item, Items...>&& tuple) { return move(get(tuple)); }

    template <size_t i, typename Item, typename... Items> constexpr const auto&
    get(const TupleImpl<i, Item, Items...>& tuple) { return tuple.TupleHead<i, Item>::item; }

    template <size_t i, typename Item, typename... Items> constexpr const auto&&
    get(const TupleImpl<i, Item, Items...>&& tuple) { return move(get(tuple)); }

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
