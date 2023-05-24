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
        constexpr Ref(T&& t) : own(etl::forward<T>(t)) {}
        constexpr Ref(T* t) : ptr(t) {}
        constexpr T& operator*()  { return ptr ? *ptr : own; }
        constexpr T* operator->() { return ptr ? ptr : &own; }
    };

    template <typename T>
    class unique_ptr {
        T* ptr;
    
    public:
        unique_ptr() noexcept : ptr{nullptr} {}
        explicit unique_ptr(T* ptr) noexcept : ptr{ptr} {}

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr(unique_ptr&& other) noexcept : ptr{other.release()} {}
        unique_ptr& operator=(unique_ptr&& other) noexcept { if (this != &other) reset(other.release()); return *this;}
        
        ~unique_ptr() noexcept { if (ptr) delete ptr; }

        explicit operator bool() const noexcept { return static_cast<bool>(ptr); }

        T* operator->() noexcept { return ptr; }
        const T* operator->() const noexcept { return ptr; }

        T& operator*() noexcept { return *ptr; }
        const T& operator*() const noexcept { return *ptr; }

        T* get() noexcept { return ptr; }
        const T* get() const noexcept { return ptr; }
        
        void reset(T* other = nullptr) noexcept { T* old = etl::exchange(ptr, other); if (old) delete old; }

        [[nodiscard("return pointer has to be handled by the caller")]]
        T* release() noexcept { return etl::exchange(ptr, nullptr); }
    };

    template <typename T, typename... Args> auto
    make_unique(Args&&... args) { return unique_ptr<T>(new T(etl::forward<Args>(args)...)); }

    template <typename T>
    class shared_ptr {
        T* ptr;
        int* cnt;

        void delete_() {
            if (ptr && --*cnt == 0) {
                delete ptr;
                delete cnt;
            } 
        }
        void inc_() { if (cnt) ++*cnt; }
        void assign_(const shared_ptr& other) { ptr = other.ptr; cnt = other.cnt; } 

    public:
        shared_ptr() noexcept : ptr{nullptr}, cnt{nullptr} {}
        explicit shared_ptr(T* ptr) noexcept : ptr{ptr}, cnt{new int(1)} {}
        
        shared_ptr(const shared_ptr& other) noexcept : ptr{other.ptr}, cnt{other.cnt} { 
            inc_(); 
        }
        shared_ptr& operator=(const shared_ptr& other) noexcept {
            if (this == &other) return *this;
            delete_();
            assign_(other);
            inc_();
            return *this;
        }

        shared_ptr(shared_ptr&& other) noexcept : ptr{other.ptr}, cnt{other.cnt} { 
            other.assign_(shared_ptr()); 
        }
        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this == &other) return *this;
            delete_();
            assign_(other);
            other.assign_(shared_ptr()); 
            return *this;
        }

        ~shared_ptr() { delete_(); }

        explicit operator bool() noexcept { return static_cast<bool>(ptr); }
        
        T* operator->() noexcept { return ptr; }
        const T* operator->() const noexcept { return ptr; }

        T& operator*() noexcept { return *ptr; }
        const T& operator*() const noexcept { return *ptr; }

        T* get() noexcept { return ptr; }
        const T* get() const noexcept { return ptr; }
    };

    template <typename T, typename... Args> auto
    make_shared(Args&&... args) { return shared_ptr<T>(new T(etl::forward<Args>(args)...)); }
}

#endif //ETL_REF_H
