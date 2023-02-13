#ifndef ETL_UTILITY_H
#define ETL_UTILITY_H

#include "etl/type_traits.h"

namespace Project::etl {

    /// moves to rvalue
    template <typename T> constexpr remove_reference_t<T>&&
    move(T&& t) { return static_cast<remove_reference_t<T>&&>(t); }

    /// forward an lvalue
    template <typename T> constexpr T&&
    forward(remove_reference_t<T>& t) { return static_cast<T&&>(t); }

    /// forward an rvalue
    template <typename T> constexpr T&&
    forward(remove_reference_t<T>&& t) {
        static_assert(! is_lvalue_reference_v<T>, "Invalid rvalue to lvalue conversion");
        return static_cast<T&&>(t);
    }

    /// pointer to first element
    template <typename Container> constexpr auto
    begin(Container& cont) -> decltype(cont.begin()) { return cont.begin(); }
    template <typename Container> constexpr auto
    begin(const Container& cont) -> decltype(cont.begin()) { return cont.begin(); }
    template <typename T, size_t N> constexpr T*
    begin(T (&arr)[N]) { return arr; }

    /// pointer to one past the last element
    template <typename Container> constexpr auto
    end(Container& cont) -> decltype(cont.end()) { return cont.end(); }
    template <typename Container> constexpr auto
    end(const Container& cont) -> decltype(cont.end()) { return cont.end(); }
    template <typename T, size_t N> constexpr T*
    end(T (&arr)[N]) { return arr + N; }

    /// pair of values with possible different types
    template <class X, class Y = X> struct Pair { X x; Y y; } ;
    /// triple of values with possible different types
    template <class X, class Y = X, class Z = Y> struct Triple { X x; Y y; Z z; };

    /// python-like len
    template <typename Container> constexpr auto
    len(Container& cont) -> decltype(cont.len()) { return cont.len(); }
    template <typename Container> constexpr auto
    len(const Container& cont) -> decltype(cont.len()) { return cont.len(); }
    template <typename T, size_t N> constexpr auto
    len(T (&arr)[N]) { return N; }

    /// python-like next
    template <typename Generator> constexpr auto
    next(Generator& gen) -> decltype(gen()) { return gen(); }
    template <typename Generator> constexpr auto
    next(const Generator& gen) -> decltype(gen()) { return gen(); }

    /// python-like enumerate
    template <typename Iterator>
    class enumerate_ {
        Iterator first, last;
        int cnt;

    public:
        constexpr enumerate_(Iterator first, Iterator last, int cnt = 0)
        : first(first), last(last), cnt(cnt) {}

        constexpr enumerate_& begin() { return *this; }
        constexpr enumerate_& end()   { return *this; }
        constexpr bool operator != (const enumerate_&) { return first != last; }
        constexpr void operator ++ ()   { ++first; ++cnt; }
        constexpr auto operator * ()    { return Pair<int, decltype(*first)>{cnt, *first }; }
        constexpr auto operator () ()   { auto res = *(*this); if (*this != end()) ++(*this); return res; }
    };

    template <typename Iterator> constexpr auto
    enumerate(Iterator first, Iterator last, int cnt = 0) { return enumerate_(first, last, cnt); }
    template <typename Container> constexpr auto
    enumerate(Container& cont, int cnt = 0) { return enumerate_(begin(cont), end(cont), cnt); }

    /// python-like zip
    template <typename Iterator1, typename Iterator2>
    class zip_ {
        Pair<Iterator1, Iterator2> first, last;

    public:
        constexpr zip_(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
        : first { first1, first2 }, last { last1, last2 } {}

        constexpr zip_& begin() { return *this; }
        constexpr zip_& end()   { return *this; }
        constexpr bool operator != (const zip_&) { return first.x != last.x && first.y != last.y; }
        constexpr void operator ++ ()   { ++first.x; ++first.y; }
        constexpr auto operator * ()    { return Pair<decltype(*first.x), decltype(*first.y)> { *first.x, *first.y }; }
        constexpr auto operator () ()   { auto res = *(*this); if (*this != end()) ++(*this); return res; }
    };

    template <typename Iterator1, typename Iterator2> constexpr auto
    zip(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) { return zip_(first1, last1, first2, last2); }
    template <typename Container1, typename Container2> constexpr auto
    zip(Container1& cont1, Container2& cont2) { return zip_(begin(cont1), end(cont1), begin(cont2), end(cont2)); }

    /// python-like range
    class range_ {
        int first, last;
        unsigned int step;
        bool reverse;

    public:
        constexpr range_(int first, int last, unsigned int step = 1)
        : first(first), last(last), step(step), reverse(first > last) {}

        constexpr range_& begin() { return *this; }
        constexpr range_& end()   { return *this; }
        constexpr bool operator != (const range_&) const { return reverse ? first > last : first < last; }
        constexpr void operator ++ ()       { first += (reverse ? -int(step) : int(step)); }
        constexpr int operator * () const   { return first; }
        constexpr int operator () ()        { auto res = *(*this); if (*this != end()) ++(*this); return res; }
    };

    constexpr auto range(int last) { return range_(0, last, 1); }
    constexpr auto range(int first, int last, unsigned int step = 1) { return range_(first, last, step); }

}

#endif //ETL_UTILITY_H
