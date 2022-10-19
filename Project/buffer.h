#ifndef PROJECT_BUFFER_H
#define PROJECT_BUFFER_H

#include "main.h"
#include "os.h"
#include <cstring> // memcpy

namespace Project {

    /// Buffer class
    template <class T, size_t N>
    struct Buffer {
        typedef T Type;
        T buf[N];
        constexpr Buffer() : buf{} {}

        static constexpr size_t len()       { return N; }
        constexpr T *data()                { return buf; }
        constexpr T *begin()                { return buf; }
        constexpr T *end()                  { return buf + N; }
        constexpr T &operator [](size_t i)  { i = min(i, N); return buf[i]; }

        void fill(const T &val, size_t n = N) {
            n = min(n, N);
            for (size_t i = 0; i < n; i++) buf[i] = val;
        }

    protected:
        static constexpr size_t min(size_t a, size_t b) { return a < b ? a : b; }
    };

    /// Circular buffer class
    template <class T, size_t N>
    struct BufferCirc : public Buffer<T, N> {
        size_t indexWrite;
        size_t indexRead;
        constexpr BufferCirc() : Buffer<T, N>(), indexWrite(0), indexRead(0) {}

        size_t numberOfWrite() {
            if (indexRead > indexWrite) return indexRead - indexWrite;
            return N + indexRead - indexWrite;
        }

        size_t numberOfRead() {
            if (indexRead <= indexWrite) return indexWrite - indexRead;
            return N + indexWrite - indexRead;
        }

        size_t write(const T *items, size_t nItems) {
            size_t n = numberOfWrite();
            nItems = this->min(n, nItems);
            if (nItems == 0) return 0;

            n = this->min((N - indexWrite), nItems);
            memcpy(this->buf + indexWrite, items, n * sizeof(T));
            indexWrite += n;
            nItems -= n;

            if (nItems > 0) {
                memcpy(this->buf, items + n, nItems * sizeof(T));
                indexWrite = nItems;
            }

            if (indexWrite >= N) indexWrite = 0;
            return n + nItems;
        }

        size_t read(T *items, size_t nItems) {
            size_t n = numberOfRead();
            nItems = this->min(n, nItems);
            if (nItems == 0) return 0;

            n = this->min((N - indexRead), nItems);
            memcpy(items, this->buf + indexRead, n * sizeof(T));
            indexRead += n;
            nItems -= n;

            if (nItems > 0) {
                memcpy(items + n, this->buf, nItems * sizeof(T));
                indexRead = nItems;
            }

            if (indexRead >= N) indexRead = 0;
            return n + nItems;
        }

        BufferCirc<T, N> &operator <<(const T& item) {
            write(&item, 1);
            return *this;
        }

        BufferCirc<T, N> &operator >>(T& item) {
            read(&item, 1);
            return *this;
        }
    };

    /// Double buffer class
    template <class T, size_t N>
    struct BufferDouble : public Buffer<T, N> {
        volatile bool flag;
        T *ptr; // free buffer
        // busy buffer
        T *busyPtr() { return ptr == half() ? this->begin() : half(); }

        explicit constexpr BufferDouble()
                : Buffer<T, N>()
                , flag(false)
                , ptr(half())
        {}

        static constexpr size_t halfLen() { return N / 2; }
        constexpr T *half() { return this->buf + halfLen(); }

        void init() { lock(); }

        // switch the ptr and return the opposite (busy pointer)
        T *switchPtr(bool firstHalf) {
            ptr = firstHalf ? this->begin() : this->half();
            unlock();
            return busyPtr();
        }

        T *switchPtr() {
            ptr = busyPtr();
            unlock();
            return busyPtr();
        }

        // get available pointer (free pointer)
        T *getPtr() {
            while(flag);
            lock();
            return ptr;
        }

        void lock() { flag = true; }
        void unlock() { flag = false; }

    };

} // namespace Project


#endif // PROJECT_BUFFER_H