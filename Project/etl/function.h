
#ifndef ETL_FUNCTION_H
#define ETL_FUNCTION_H

namespace Project::etl {
    /// function class that holds function pointer and context (alternative of capture list)
    template <typename T, typename C = void> struct Function;

    template <class R, class... Args, class C>
    struct Function<R(Args...), C> {
        typedef R Result;
        typedef R (* Fn)(C, Args...);

        Fn fn;     ///< function pointer, default null
        C context; ///< alternative of capture list, default null
        constexpr Function(Fn fn, C context) : fn(fn), context(context) {}

        Function& operator=(Fn function) { fn = function; return *this; }
        constexpr R operator()(Args... args) const { return fn ? fn(context, args...) : R(); }

        explicit constexpr operator bool() const { return fn; }
        explicit constexpr operator Fn() const { return fn; }
    };

    /// context = void specialization
    template <class R, class... Args>
    struct Function<R(Args...), void> {
        typedef R Result;
        typedef R (* Fn)(Args...);

        Fn fn;     ///< function pointer, default null
        constexpr Function(Fn fn = nullptr) : fn(fn) {}

        Function& operator=(Fn function) { fn = function; return *this; }
        constexpr R operator()(Args... args) const { return fn ? fn(args...) : R(); }

        explicit constexpr operator bool() const { return fn; }
        explicit constexpr operator Fn() const { return fn; }
    };

    /// context = void* specialization
    template <class R, class... Args>
    struct Function<R(Args...), void*> {
        typedef R Result;
        typedef R (* Fn)(void*, Args...);

        Fn fn;         ///< function pointer, default null
        void* context; ///< alternative of capture list, default null
        constexpr Function(Fn fn = nullptr, void* context = nullptr) : fn(fn), context(context) {}

        Function& operator=(Fn function) { fn = function; return *this; }
        constexpr R operator()(Args... args) const { return fn ? fn(context, args...) : R(); }

        explicit constexpr operator bool() const { return fn; }
        explicit constexpr operator Fn() const { return fn; }
    };
}

#endif //ETL_FUNCTION_H
