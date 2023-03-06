#ifndef ETL_VECTOR_H
#define ETL_VECTOR_H

#include "etl/type_traits.h"

namespace Project::etl {

    /// dynamic contiguous array
    template <class T>
    class Vector {
    protected:
        T* buffer;
        size_t nItems;
        Vector(T* buffer, size_t nItems) : buffer(buffer), nItems(nItems) {}

        T* newInsert(const Vector& other) const {
            auto temp = new T[nItems + other.nItems];
            memcpy(temp, buffer, nItems * sizeof (T));
            memcpy(temp + nItems, other.buffer, other.nItems * sizeof (T));
            return temp;
        }

        T* newInsert(const T& other) const {
            auto temp = new T[nItems + 1];
            memcpy(temp, buffer, nItems * sizeof (T));
            memcpy(temp + nItems, &other, sizeof (T));
            return temp;
        }

    public:
        typedef T Type;

        template <class... Ts>
        Vector(Ts... items)
        : buffer(new T[sizeof...(items)] { items... })
        , nItems(sizeof...(items)) {}
        constexpr Vector() : buffer(nullptr), nItems(0) {}

        virtual ~Vector() {
            delete [] buffer;
            buffer = nullptr;
            nItems = 0;
        }

        [[nodiscard]] size_t len() const { return nItems; }

        T* data()   { return buffer; }
        T* begin()  { return buffer; }
        T* end()    { return buffer + nItems; }
        T& front()  { return buffer[0]; }
        T& back()   { return buffer[nItems - 1]; }

        const T* data()     const { return buffer; }
        const T* begin()    const { return buffer; }
        const T* end()      const { return buffer + nItems; }
        const T& front()    const { return buffer[0]; }
        const T& back()     const { return buffer[nItems - 1]; }

        T& operator [](size_t i) { return buffer[i]; }
        const T& operator [](size_t i) const { return buffer[i]; }
        explicit operator bool () { return buffer != nullptr; }

        Vector operator + (const Vector& other) const {
            return { newInsert(other), nItems + other.nItems };
        }
        Vector operator + (const T& other) const {
            return { newInsert(other), nItems + 1 };
        }

        void append(const Vector& other) {
            auto temp = newInsert(other);
            delete [] buffer;
            buffer = temp;
            nItems += other.nItems;
        }
        void append(const T& other) {
            auto temp = newInsert(other);
            delete [] buffer;
            buffer = temp;
            nItems++;
        }

        void remove(size_t index) {
            if (index >= this->len()) return;

            auto buf = new T[this->len() - 1];
            size_t i = 0;
            for (auto& item : *this) {
                if (i == index) continue;
                buf[i++] = item;
            }

            delete [] buffer;
            buffer = buf;
            --nItems;
        }

        Vector& operator += (const Vector& other) {
            append(other);
            return *this;
        }
        Vector& operator += (const T& other) {
            append(other);
            return *this;
        }
    };

    /// create vector with variadic template function, Type is deduced
    template<typename T, typename... U> Vector<enable_if_t<(is_same_v<T, U> && ...), T>>
    vector(const T& t, const U&...u) { return Vector<T>{t, u...}; }
}

#endif //ETL_VECTOR_H
