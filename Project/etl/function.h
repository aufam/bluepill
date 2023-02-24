
#ifndef ETL_FUNCTION_H
#define ETL_FUNCTION_H

namespace Project::etl {
    /// function class that holds function pointer and context (alternative of capture list)
    template <typename T> struct Function;

    template <class R, class... Args>
    struct Function<R(Args...)> {
        typedef R Result;
        typedef R (* Fn)(void*, Args...);

        Fn fn = nullptr;            ///< function pointer, default null
        void *context = nullptr;    ///< alternative of capture list, default null
        constexpr Function(Fn fn = nullptr, void *context = nullptr) : fn(fn), context(context) {}

        Function& operator = (Fn function) { fn = function; return *this; }
        constexpr R operator ()(Args... args) const { return fn ? fn(context, args...) : R(); }

        explicit constexpr operator bool () const { return fn; }
        explicit operator Fn () const { return fn; }
    };
}

#endif //ETL_FUNCTION_H
