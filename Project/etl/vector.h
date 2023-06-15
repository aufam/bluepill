#ifndef ETL_VECTOR_H
#define ETL_VECTOR_H

#include "etl/algorithm.h"

namespace Project::etl {

    /// dynamic contiguous arrays
    template <class T>
    class Vector {
    protected:
        T* buf;
        size_t nItems, capacity;

        Vector(T* buffer, size_t nItems, size_t capacity) 
            : buf(buffer)
            , nItems(buffer ? nItems : 0)
            , capacity(buffer ? capacity : 0) {}

    public:
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;

        /// empty constructor
        constexpr Vector() : Vector(nullptr, 0, 0) {}

        /// construct and set the capacity
        Vector(size_t capacity) : Vector(new T[capacity], 0, capacity) {}

        /// construct from initializer list
        Vector(std::initializer_list<T> items) : Vector(new T[items.size()], items.size(), items.size()) {
            if (buf)
                etl::copy(items.begin(), items.end(), begin());
        }

        /// copy constructor
        Vector(const Vector& v) : Vector(v.copy_alloc_(v.capacity), v.nItems, v.capacity) {}

        /// move constructor
        Vector(Vector&& v) noexcept : Vector(v.buf, v.nItems, v.capacity) { v.reset_(); }

        /// copy assignment
        Vector& operator=(const Vector& other) {
            if (this == &other) return *this;

            bool valid = bool(buf) && bool(other.buf);
            if (other.nItems > nItems)
                valid = reserve(other.nItems);
            
            if (valid) {
                etl::copy(other.begin(), other.end(), buf);
                nItems = other.nItems;
            }
            return *this;
        }

        /// move assignment
        Vector& operator=(Vector&& other) noexcept {
            if (this == &other) return *this;
            reset_delete_(other.buf, other.nItems, other.capacity);
            other.reset_();
            return *this;
        }

        /// destructor
        virtual ~Vector() noexcept { reset_delete_(); }

        [[nodiscard]] size_t len() const { return nItems; }     ///< returns the number of items
        [[nodiscard]] size_t size() const { return capacity; }  ///< returns the capacity

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
        /// @warning it will throw error null dereference if index is not valid
        reference operator[](int i) { return is_valid_index_(i) ? buf[i] : *static_cast<iterator>(nullptr); }
        const_reference operator[](int i) const { return is_valid_index_(i) ? buf[i] : *static_cast<const_iterator>(nullptr); }

        /// return true if buf is not null
        explicit operator bool() { return buf != nullptr; }

        /// create new vector by adding another container
        template <typename Container>
        Vector operator+(const Container& other) const { 
            auto newCapacity = nItems + etl::len(other);
            auto temp = copy_alloc_(newCapacity);
            etl::copy(etl::begin(other), etl::end(other), temp + nItems);
            return { temp, newCapacity, newCapacity }; 
        }

        /// create new vector by adding an item
        Vector operator+(const_reference other) const { 
            auto newCapacity = nItems + 1;
            auto temp = copy_alloc_(newCapacity);
            temp[nItems] = other;
            return { temp, newCapacity, newCapacity }; 
        }

        /// add an item to the vector
        void append(const_reference other) {
            auto newCapacity = nItems + 1;
            if (capacity < newCapacity) 
                reserve(newCapacity);
            buf[nItems] = other;
            ++nItems;
        }
        
        Vector& operator+=(const_reference other) { append(other); return *this; }

        /// add all items in a container to the vector
        template <typename Container>
        void append(const Container& other) {
            auto newCapacity = nItems + etl::len(other);
            if (capacity < newCapacity) 
                reserve(newCapacity);
            etl::copy(etl::begin(other), etl::end(other), end());
            nItems += etl::len(other);
        }

        template <typename Container>
        Vector& operator+=(const Container& other) { append(other); return *this; }

        /// insert new item given the index
        void insert(int index, const_reference item) {
            index = index >= 0 ? etl::min(index, int(nItems)) :
                    int(nItems) + etl::max(index, -int(nItems));

            auto newCapacity = nItems + 1;
            if (capacity < newCapacity)
                reserve(newCapacity);

            auto [x, y] = etl::pair(end(), end() - 1);
            for (; x != &buf[index]; --x, --y) 
                *x = *y; // shift

            buf[index] = item;
            ++nItems;
        }

        /// insert new sequence given the index
        template <typename Container>
        void insert(int index, const Container& other) {
            index = index >= 0 ? etl::min(index, int(nItems)) :
                    int(nItems) + etl::max(index, -int(nItems));

            auto newCapacity = nItems + etl::len(other);
            if (capacity < newCapacity)
                reserve(newCapacity);

            auto [x, y] = etl::pair(begin() + newCapacity - 1, end() - 1);
            for (; x != &buf[index]; --x, --y) 
                *x = *y; // shift

            etl::copy(etl::begin(other), etl::end(other), begin() + index);
            nItems += etl::len(other);
        }

        /// remove an item given the index
        bool remove_at(int index) {
            if (!is_valid_index_(index)) 
                return false;

            for (; index < int(nItems) - 1; ++index)
                buf[index] = buf[index + 1];

            --nItems;
            return true;
        }

        /// remove the first item found in the vector
        bool remove(const_reference x) {
            int index = etl::find(begin(), end(), x) - begin();
            return remove_at(index);
        }

        /// set n items to 0, capacity remains the same
        void clear() { nItems = 0; }

        /// set new capacity, return true if success
        bool reserve(size_t newCapacity) {
            if (newCapacity == 0) {
                reset_delete_();
                return true;
            }
            else if (newCapacity == capacity) {
                return true;
            }

            auto newBuf = copy_alloc_(newCapacity);
            if (newBuf == nullptr) 
                return false;

            reset_delete_(newBuf, etl::min(nItems, newCapacity), newCapacity);
            return true;
        }

        /// shrink the capacity to fit the number of items
        bool shrink() { return reserve(nItems); }

        /// expand the number of items to fit the capacity
        void expand() { nItems = capacity; }

        /// slice operator
        Iter<iterator> operator()(int start, int stop, int step = 1) { 
            return (start < stop && step > 0) || (start > stop && step < 0) ? 
                etl::iter(&operator[](start), &operator[](stop), step) : // valid index
                etl::iter(begin(), begin()); // invalid index
        }
        Iter<const_iterator> operator()(int start, int stop, int step = 1) const { 
            return (start < stop && step > 0) || (start > stop && step < 0) ? 
                etl::iter(&operator[](start), &operator[](stop), step) : // valid index
                etl::iter(begin(), begin()); // invalid index
        }

        template <class Container>
        bool operator==(const Container& other) const { return etl::compare_all(*this, other); }

        template <class Container>
        bool operator!=(const Container& other) const { return !operator==(other); }
    
    protected:
        iterator copy_alloc_(size_t newCapacity) const {
            auto temp = new T[newCapacity];
            etl::copy(buf, buf + etl::min(newCapacity, nItems), temp);
            return temp;
        }

        void reset_(iterator ptr = nullptr, size_t n = 0, size_t cap = 0) {
            buf = ptr;
            nItems = n;
            capacity = cap;
        }

        void reset_delete_(iterator ptr = nullptr, size_t n = 0, size_t cap = 0) {
            delete[] buf;
            reset_(ptr, n, cap);
        }

        bool is_valid_index_(int& index) const {
            if (nItems == 0) return false;
            if (index < 0) index = int(nItems) + index; // allowing negative index
            if (index < 0 || size_t(index) >= nItems) return false; // out of range
            return true;
        }
    };

    /// create vector with variadic template function, the type can be implicitly or explicitly specified
    template <typename T = void, typename U, typename... Us, typename R = conditional_t<is_void_v<T>, U, T>> auto
    vector(const U& val, const Us&...vals) { return Vector<R> { static_cast<R>(val), static_cast<R>(vals)... }; }

    /// create vector from initializer list
    template <typename T> auto
    vector(std::initializer_list<T> items) { return Vector<T>(items); }

    /// create empty vector, capacity is 0
    template <typename T> constexpr auto
    vector() { return Vector<T>(); }

    /// create empty vector and set the capacity
    template <typename T> auto
    vector_reserve(size_t capacity) { return Vector<T>(capacity); }

    /// type traits
    template <typename T> struct is_vector : false_type {};
    template <typename T> struct is_vector<Vector<T>> : true_type {};
    template <typename T> struct is_vector<const Vector<T>> : true_type {};
    template <typename T> struct is_vector<volatile Vector<T>> : true_type {};
    template <typename T> struct is_vector<const volatile Vector<T>> : true_type {};
    template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

    template <typename T> struct remove_extent<Vector<T>> { typedef T type; };
    template <typename T> struct remove_extent<const Vector<T>> { typedef T type; };
    template <typename T> struct remove_extent<volatile Vector<T>> { typedef T type; };
    template <typename T> struct remove_extent<const volatile Vector<T>> { typedef T type; };
}

#endif //ETL_VECTOR_H
