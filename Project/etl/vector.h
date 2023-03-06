#ifndef ETL_VECTOR_H
#define ETL_VECTOR_H

#include "etl/algorithm.h"

namespace Project::etl {

    /// dynamic contiguous array
    template <class T>
    class Vector {
    protected:
        T* buf;
        size_t nItems;
        Vector(T* buffer, size_t nItems) : buf(buffer), nItems(nItems) {}

        T* insert_(const Vector& other) const {
            auto temp = new T[nItems + other.nItems];
            copy(begin(), end(), temp);
            copy(other.begin(), other.end(), temp + nItems);
            return temp;
        }

        T* insert_(const T& other) const {
            auto temp = new T[nItems + 1];
            copy(begin(), end(), temp);
            temp[nItems] = other;
            return temp;
        }

    public:
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;

        /// empty constructor
        constexpr Vector() : buf(nullptr), nItems(0) {}

        /// variadic template function constructor
        template <class... Ts>
        Vector(Ts... items)
        : buf(new T[sizeof...(items)] {items... })
        , nItems(sizeof...(items)) {}

        /// copy constructor
        Vector(const Vector& v) : buf(new T[v.nItems]), nItems(v.nItems) {
            copy(v.begin(), v.end(), buf);
        }

        /// move constructor
        Vector(Vector&& v) noexcept : buf(move(v.buf)), nItems(move(v.nItems)) {
            v.buf = nullptr;
            v.nItems = 0;
        }

        /// copy assignment
        Vector& operator=(const Vector& other) {
            if (this == &other) return *this;
            nItems = other.nItems;
            buf = new T[nItems];
            copy(other.begin(), other.end(), buf);
            return *this;
        }

        /// move assignment
        Vector& operator=(Vector&& other) noexcept {
            buf = move(other.buf);
            nItems = move(other.nItems);
            other.buf = nullptr;
            other.nItems = 0;
            return *this;
        }

        virtual ~Vector() {
            delete [] buf;
            buf = nullptr;
            nItems = 0;
        }

        [[nodiscard]] size_t len() const { return nItems; }

        iterator data()   { return buf; }
        iterator begin()  { return buf; }
        iterator end()    { return buf + nItems; }
        reference front() { return buf[0]; }
        reference back()  { return buf[nItems - 1]; }

        const_iterator data()   const { return buf; }
        const_iterator begin()  const { return buf; }
        const_iterator end()    const { return buf + nItems; }
        const_reference front() const { return buf[0]; }
        const_reference back()  const { return buf[nItems - 1]; }

        /// get i-th item by dereference
        /// @warning make sure n items is not 0
        reference operator[](int i) {
            if (len() == 0) return *static_cast<T*>(nullptr);
            if (i < 0) i = len() + i; // allowing negative index
            return buf[i];
        }

        /// get i-th item by dereference
        /// @warning make sure n items is not 0
        const_reference operator[](int i) const {
            if (len() == 0) return *static_cast<T*>(nullptr);
            if (i < 0) i = len() + i; // allowing negative index
            return buf[i];
        }

        explicit operator bool() { return buf != nullptr; }

        Vector operator+(const Vector& other) const { return {insert_(other), nItems + other.nItems }; }
        Vector operator+(const T& other) const { return {insert_(other), nItems + 1 }; }

        void append(const Vector& other) {
            auto temp = insert_(other);
            delete [] buf;
            buf = temp;
            nItems += other.nItems;
        }
        void append(const T& other) {
            auto temp = insert_(other);
            delete [] buf;
            buf = temp;
            nItems++;
        }

        void remove(size_t index) {
            if (len() == 0) return;
            if (index >= len()) return;

            auto newBuffer = new T[len() - 1];
            size_t i = 0;
            for (auto& item : *this) {
                if (i == index) continue;
                newBuffer[i++] = item;
            }
            delete [] buf;
            buf = newBuffer;
            --nItems;
        }

        Vector& operator+=(const Vector& other) {
            append(other);
            return *this;
        }
        Vector& operator+=(const T& other) {
            append(other);
            return *this;
        }

        template <class Container>
        bool operator==(const Container& other) const { return compare_all(*this, other); }

        template <class Container>
        bool operator!=(const Container& other) const { return !operator==(other); }
    };

    /// create vector with variadic template function, type is deduced
    template <typename T, typename... U> Vector<enable_if_t<(is_same_v<T, U> && ...), T>>
    vector(const T& t, const U&...u) { return Vector<T>{t, u...}; }

}

#endif //ETL_VECTOR_H
