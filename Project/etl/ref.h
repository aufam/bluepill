#ifndef ETL_REF_H
#define ETL_REF_H

#include "etl/utility.h"

namespace Project::etl {

    /// immutable object
    template <typename T>
    class ConstRef {
        T own;
        const T* ptr = nullptr;
    public:
        constexpr ConstRef() = default;
        constexpr ConstRef(const T& t) : own(t) {}
        constexpr ConstRef(const T* t) : ptr(t) {}
        constexpr const T& operator*()  const { return ptr ? *ptr : own; }
        constexpr const T* operator->() const { return ptr ? ptr : &own; }
    };

    /// mutable object
    template <typename T>
    class Ref {
        T own;
        const T* ptr = nullptr;
    public:
        constexpr Ref() = default;
        constexpr Ref(const T& t) : own(t) {}
        constexpr Ref(T&& t) : own(forward<T>(t)) {}
        constexpr Ref(T* t) : ptr(t) {}
        constexpr T& operator*()  { return ptr ? *ptr : own; }
        constexpr T* operator->() { return ptr ? ptr : &own; }
    };
}

#endif //ETL_REF_H
