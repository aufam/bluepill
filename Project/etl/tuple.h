#ifndef ETL_TUPLE_H
#define ETL_TUPLE_H

namespace Project::etl {
    /// contains the actual value for one item in the tuple.
    /// @tparam i allows the get function to find the value in O(1) time
    template <size_t i, typename Item> struct xTupleLeaf { Item item; };

    /// acts as proxy for the final class that has an extra template parameter `i`.
    template <size_t i, typename... Items> struct xTupleImpl;

    /// base case: empty tuple
    template <size_t i> struct xTupleImpl<i> {};

    /// recursive specialization
    template <size_t i, typename HeadItem, typename... TailItems>
    struct xTupleImpl<i, HeadItem, TailItems...>
            : public xTupleLeaf<i, HeadItem>
            , public xTupleImpl<i + 1, TailItems...> {};

    /// obtain a reference to i-th item in a tuple
    template <size_t i, typename HeadItem, typename... TailItems> auto&
    get(xTupleImpl<i, HeadItem, TailItems...>& tuple) { return tuple.xTupleLeaf<i, HeadItem>::item; }

    template <size_t i, typename HeadItem, typename... TailItems> auto&&
    get(xTupleImpl<i, HeadItem, TailItems...>&& tuple) { return move(tuple.xTupleLeaf<i, HeadItem>::item); }

    template <size_t i, typename HeadItem, typename... TailItems> constexpr auto&
    get(const xTupleImpl<i, HeadItem, TailItems...>& tuple) { return tuple.xTupleLeaf<i, HeadItem>::item; }

    /// fixed size container, which holds elements of possibly different types
    template <typename... Items> using Tuple = xTupleImpl<0, Items...>;

    /// create tuple with variadic template function
    template <typename HeadItem, typename... Items> constexpr Tuple<HeadItem, Items...>
    tuple(const HeadItem& head, const Items&... items) { return Tuple<HeadItem, Items...>{head, items...}; }

}

#endif //ETL_TUPLE_H
