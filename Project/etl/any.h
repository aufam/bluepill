#ifndef ETL_ANY_H
#define ETL_ANY_H

#include "etl/utility_basic.h"

namespace Project::etl {

    /// provide a dynamic storage for storing and manipulating value of any type
    /// @note if the type has custom destructor, you have to manually delete it by invoking detach<T>() 
    /// before destructing or reassigning this object
    class Any {
        uint8_t* ptr;
        size_t n;

    public:
        /// empty constructor
        Any() : ptr(nullptr), n(0) {}

        /// copy constructor
        Any(const Any& other) : ptr(new uint8_t[other.n]), n(other.n) {
            if (ptr == nullptr) {
                n = 0;
                return;
            }

            for (size_t i = 0; i < n; i++)
                ptr[i] = other.ptr[i];
        }

        /// copy assignment
        Any& operator=(const Any& other) {
            if (&other == this) return *this;
            detach();
            
            ptr = new uint8_t[other.n];
            n = other.n;
            if (ptr == nullptr) {
                n = 0;
                return *this;
            }

            for (size_t i = 0; i < n; i++)
                ptr[i] = other.ptr[i];
            return *this;
        }

        /// move constructor
        Any(Any&& other) : ptr(etl::exchange(other.ptr, nullptr)), n(etl::exchange(other.n, 0)) {}

        /// move assignment
        Any& operator=(Any&& other) {
            if (&other == this) return *this;
            detach();

            ptr = etl::exchange(other.ptr, nullptr);
            n = etl::exchange(other.n, 0);
            return *this;
        }

        /// construct from any type
        template <typename T, typename U = remove_const_volatile_ref_t<T>> explicit
        Any(T&& value, enable_if_t<!is_same_v<U, Any>>* = nullptr) : ptr(nullptr), n(0) {
            if constexpr (!is_same_v<U, None_t>) {
                ptr = reinterpret_cast<uint8_t *>(new U(etl::forward<U>(value)));
                n = sizeof(U);
                if (ptr == nullptr)
                    n = 0;
            }
        }

        /// assign from any type
        template <typename T, typename U = remove_const_volatile_ref_t<T>>
        enable_if_t<!is_same_v<U, Any>, Any&> operator=(T&& value) {
            if (reinterpret_cast<uint8_t*>(&value) == ptr) return *this;
            detach();

            if constexpr (!is_same_v<U, None_t>) {
                ptr = reinterpret_cast<uint8_t *>(new U(etl::forward<U>(value)));
                n = sizeof(U);
                if (ptr == nullptr)
                    n = 0;
            }
            return *this;
        }

        ~Any() { detach(); }

        size_t size() const { return n; }

        /// invoke deleter and reset
        template <typename T = void>
        enable_if_t<!is_reference_v<T>> detach() {
            if (ptr == nullptr) return;
            
            if constexpr (is_compound_v<T>)
                reinterpret_cast<T*>(ptr)->~T();
            else
                delete[] ptr;
            
            ptr = nullptr;
            n = 0;
        }

        /// cast to any type
        /// @note the type has to be the last type this object is assigned to
        template <typename T> const T& as() const { return *reinterpret_cast<const T*>(ptr); }
        template <typename T> T& as() { return *reinterpret_cast<T*>(ptr); }

        const void* get() const { return ptr; }
        void* get() { return ptr; }

        explicit operator bool() const { return bool(ptr); }
    };
}

#endif // ETL_ANY_H