
#ifndef ETL_FUNCTION_H
#define ETL_FUNCTION_H

namespace Project::etl {
    /// function class that holds function pointer and context (alternative of capture list)
    template <typename T> struct Function;

    /// base function structure
    template <class R, class... Args>
    struct Function<R(Args...)> {
        typedef R Result;
        typedef R (* Fn)(void*, Args...);

        Fn fn = nullptr;            ///< function pointer, default null
        void *context = nullptr;    ///< alternative of capture list, default null

        Function& operator = (Fn function) { fn = function; return *this; }
        explicit operator Fn () { return fn; }
        constexpr R operator ()(Args... args) const { if (fn) return fn(context, args...); return R{};}
        explicit constexpr operator bool () const { return fn; }
    };

    /// void function structure
    template <class... Args>
    struct Function<void(Args...)> {
        typedef void Result;
        typedef void (* Fn)(void*, Args...);

        Fn fn = nullptr;            ///< function pointer, default null
        void *context = nullptr;    ///< alternative of capture list, default null

        Function& operator = (Fn function) { fn = function; return *this; }
        explicit operator Fn () { return fn; }
        constexpr void operator ()(Args... args) const { if (fn) fn(context, args...);}
        explicit constexpr operator bool () const { return fn; }
    };
}

#endif //ETL_FUNCTION_H
