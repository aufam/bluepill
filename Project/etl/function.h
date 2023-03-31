#ifndef ETL_FUNCTION_H
#define ETL_FUNCTION_H

#include "etl/tuple.h"

namespace Project::etl {
    /// function class that holds function pointer and context (alternative of capture list)
    template <typename T, typename... C> struct Function;

    template <typename R, typename... Args, typename... C>
    struct Function<R(Args...), C...> {
        typedef R Result;
        typedef R (* Fn)(C..., Args...);
        typedef Tuple<C...> Context;

        Fn fn;                   ///< function pointer
        mutable Context context; ///< alternative of capture list in a lambda expression
        constexpr Function(Fn fn, C... context) : fn(fn), context{context...} {}

        Function& operator=(Fn function) { fn = function; return *this; }
        constexpr R operator()(Args... args) const { return fn ? invoke(index_sequence_for<C...>{}, args...) : R(); }

        explicit constexpr operator bool() const { return fn; }
        explicit constexpr operator Fn() const { return fn; }

    private:
        template <size_t... i>
        constexpr decltype(auto) invoke(index_sequence<i...>, Args... args) const { return fn(get<i>(context)..., args...); }
    };

    /// function class specialization for Context = void
    template <typename R, typename... Args>
    struct Function<R(Args...)> {
        typedef R Result;
        typedef R (* Fn)(Args...);
        typedef void Context;

        Fn fn;     ///< function pointer, default null
        constexpr Function(Fn fn = nullptr) : fn(fn) {}

        Function& operator=(Fn function) { fn = function; return *this; }
        constexpr R operator()(Args... args) const { return fn ? fn(args...) : R(); }

        explicit constexpr operator bool() const { return fn; }
        explicit constexpr operator Fn() const { return fn; }
    };

    /// function class specialization for Context = void*
    template <typename R, typename... Args>
    struct Function<R(Args...), void*> {
        typedef R Result;
        typedef R (* Fn)(void*, Args...);
        typedef void* Context;

        Fn fn;           ///< function pointer, default null
        Context context; ///< alternative of capture list, default null
        constexpr Function(Fn fn = nullptr, Context context = nullptr) : fn(fn), context(context) {}

        Function& operator=(Fn function) { fn = function; return *this; }
        constexpr R operator()(Args... args) const { return fn ? fn(context, args...) : R(); }

        explicit constexpr operator bool() const { return fn; }
        explicit constexpr operator Fn() const { return fn; }
    };

    /// create function without context
    template <typename Fn> auto
    function(Fn* fn = nullptr) { return Function<Fn>(fn); }

    /// create function with context
    template <typename ...C, typename R> auto
    function(R (*fn)(C...), C... context) { return Function<R(), C...>(fn, context...); }

    /// create function with context, capture by reference
    template <typename ...C, typename R> auto
    functionR(R (*fn)(C&...), C&... context) { return Function<R(), C&...>(fn, context...); }
}

#endif //ETL_FUNCTION_H
