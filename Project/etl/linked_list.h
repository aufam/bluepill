#ifndef ETL_LINKED_LIST_H
#define ETL_LINKED_LIST_H

#include "etl/algorithm.h"

namespace Project::etl {

    /// doubly linked list. every item will be allocated to heap
    template <class T>
    struct LinkedList {
        struct iterator; ///< modify increment, decrement, bool operator, and dereference of Node*
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef const iterator const_iterator;

    private:
        struct Node; ///< contains the item and pointer to next and prev items
        mutable iterator head;

        template <class ...Ts>
        void construct_(T firstItem, Ts... items) {
            push(firstItem);
            if constexpr (sizeof...(Ts) > 0) construct_(items...);
        }

    public:
        /// empty constructor
        constexpr LinkedList() : head((Node*)nullptr) {}

        /// variadic template function constructor
        template <class ...Ts>
        LinkedList(T firstItem, Ts... items) : head((Node*)nullptr) { construct_(firstItem, items...); }

        /// construct from initializer list
        LinkedList(std::initializer_list<T> items) : head((Node*)nullptr) {
            for (auto& item : items) push(item);
        }

        /// copy constructor
        LinkedList(const LinkedList& l) : head((Node*)nullptr) {
            for (auto& item : l) push(item);
        }

        /// move constructor
        LinkedList(LinkedList&& l) noexcept : head(etl::move(l.head)) {
            l.head = (Node*) nullptr;
        }

        /// copy assignment
        LinkedList& operator=(const LinkedList& other) {
            for (auto i : etl::range(other.len())) operator[](i) = other[i];
            return *this;
        }

        /// move assignment
        LinkedList& operator=(LinkedList&& other) noexcept {
            clear();
            head = etl::move(other.head);
            other.head = (Node*) nullptr;
            return *this;
        }

        virtual ~LinkedList() { clear(); }

        iterator data()  { return head; }
        iterator begin() { return head; }
        iterator end()   { return { (Node*)nullptr }; }
        iterator tail()  { return head + (len() - 1); }

        const_iterator data()  const { return head; }
        const_iterator begin() const { return head; }
        const_iterator end()   const { return { (Node*)nullptr }; }
        const_iterator tail()  const { return head + (len() - 1); }

        /// @retval number of items
        size_t len() const {
            size_t res = 0;
            for (const auto& _ [[maybe_unused]] : *this) res++;
            return res;
        }

        /// delete all items
        void clear() const { while (pop()); }

        /// get the first item
        /// @warning make sure head is not null
        reference front() { return *head; }

        /// get the first item
        /// @warning make sure head is not null
        const_reference front() const { return *head; }

        /// get the last item
        /// @warning make sure tail() is not null
        reference back() { return *tail(); }

        /// get the last item
        /// @warning make sure tail() is not null
        const_reference back() const { return *tail(); }

        /// get i-th item by dereference
        /// @warning if head + i = null, it will make new node and return its item
        /// @warning if i less than 0, it will iterate backward from the tail
        reference operator[](int i) {
            auto p = i >= 0 ? head + i : tail() + (i + 1);
            if (p) return *p;
            T dummy = {};
            i >= 0 ? pushBack(dummy) : pushFront(dummy);
            return i >= 0 ? back() : front();
        }

        /// get i-th item by dereference
        /// @warning if head + i = null, it will make new node and return its item
        /// @warning if i less than 0, it will iterate backward from the tail
        const_reference operator[](int i) const {
            auto p = i >= 0 ? head + i : tail() + (i + 1);
            if (p) return *p;
            T dummy = {};
            i >= 0 ? pushBack(dummy) : pushFront(dummy);
            return i >= 0 ? back() : front();
        }

        explicit operator bool() const { return (bool) head; }

        /// slice operator
        Iter<iterator> operator()(int start, int stop, int step = 1) { 
            auto b = start >= 0 ? head + start : tail() + (start + 1);
            auto e = stop >= 0 ? head + stop : tail() + (stop + 1);
            return etl::iter(b, e, step);
        }

        /// slice operator
        Iter<const_iterator> operator()(int start, int stop, int step = 1) const { 
            auto b = start >= 0 ? head + start : tail() + (start + 1);
            auto e = stop >= 0 ? head + stop : tail() + (stop + 1);
            return etl::iter(b, e, step);
        }

        /// push operator
        const LinkedList& operator<<(const_reference item) const { push(item); return *this; }

        /// pop operator
        const LinkedList& operator>>(reference item) const { pop(item); return *this; }

        /// compare operator
        template <class Container>
        bool operator==(const Container& other) const { return etl::compare_all(*this, other); }

        /// compare operator
        template <class Container>
        bool operator!=(const Container& other) const { return !operator==(other); }

        /// push an item at the back of the list
        int push(const_reference item) const {
            auto node = iterator(item); // create new node
            if (!node) 
                return 0; 

            if (!head) { 
                head = node; // if the linked list is empty, set the head to the new node
                return 1;
            }

            return tail().insert(node); // insert the node at the the back
        }

        /// push an item to the list at a specific position
        int push(const_reference item, size_t pos) const {
            auto node = iterator(item); // create new node
            if (!node) 
                return 0;

            if (pos == 0) {
                head.insertPrev(node);  // if pos is 0, insert the new node at the beginning
                head = node;
                return 1;
            }

            int res = (head + (pos - 1)).insert(node); // find the node at the desired position and insert the new node after it
            if (res == 0) 
                node.erase(); // erase the new node if insertion failed

            return res;
        }

        /// push an item at the back
        int pushBack(const_reference item)  const { return push(item); }

        /// push an item at the front
        int pushFront(const_reference item) const { return push(item, 0); }

        /// removes an item from the list at a specific position and retrieves its value
        int popAt(reference item, size_t pos) const {
            auto node = head + pos;
            if (node) 
                item = *node;
            
            if (pos == 0 && head) 
                head = head.next();
            
            return node.erase();
        }

        /// removes an item from the list at a specific position
        int popAt(size_t pos) const { 
            auto node = head + pos;
            if (pos == 0 && head) 
                head = head.next();
            
            return node.erase();
         }

        /// removes the first item from the list and retrieves its value
        int pop(reference item) const { return popAt(item, 0); }

        /// removes the first item from the list
        int pop() const { return popAt(0); }

        /// removes the first item from the list
        int popBack() const { return popAt(len() - 1); }

        /// removes the first item from the list
        int popFront() const { return popAt(0); }
        
        /// removes the first item from the list
        int popBack(T& item)  const { return popAt(item, len() - 1); }

        /// removes the first item from the list
        int popFront(T& item) const { return popAt(item, 0); }
    };

    /// create linked list with variadic template function, type is deduced
    template <typename T = void, typename U, typename... Us, typename R = conditional_t<is_void_v<T>, U, T>> auto
    list(const U& val, const Us&... vals) { return LinkedList<R> { static_cast<R>(val), static_cast<R>(vals)... }; }

    /// create linked list from initializer list
    template <typename T> auto
    list(std::initializer_list<T> items) { return LinkedList<T>(items); }

    /// empty linked list
    template <typename T> auto 
    list() { return LinkedList<T> {}; }

    /// iter specialization for linked list
    template <typename T> auto
    iter(LinkedList<T>& l, int step = 1) { return Iter(step >= 0 ? l.begin() : l.tail(), l.end(), step); }

    /// iter specialization for linked list
    template <typename T> auto
    iter(const LinkedList<T>& l, int step = 1) { return Iter(step >= 0 ? l.begin() : l.tail(), l.end(), step); }

    /// reversed specialization for linked list
    template <typename T> auto
    reversed(LinkedList<T>& l) { return etl::iter(l, -1); }

    /// reversed specialization for linked list
    template <typename T> auto
    reversed(const LinkedList<T>& l) { return etl::iter(l, -1); }

    template <class T>
    struct LinkedList<T>::Node {
        T item;
        mutable Node* next = nullptr;
        mutable Node* prev = nullptr;
        explicit Node(const T& item) : item(item) {}
    };

    template <class T>
    struct LinkedList<T>::iterator {
        Node* node;

        /// construct from pointer of node
        iterator(Node* node) : node(node) {}
        
        /// construct from pointer of node
        explicit iterator(const T& item) : node(new Node(item)) {}

        T* item() { return node ? &node->item : nullptr; }
        iterator next() { return { node ? node->next : nullptr }; }
        iterator prev() { return { node ? node->prev : nullptr }; }

        const T* item() const { return node ? &node->item : nullptr; }
        const iterator next() const { return { node ? node->next : nullptr }; }
        const iterator prev() const { return { node ? node->prev : nullptr }; }
        
        /// insert another iterator to the next/prev position of this iterator
        /// @param other other iterator
        /// @param nextOrPref false: insert to next (default), true: insert to prev
        /// @return 0: fail, 1: success
        /// @note other node and this node can't be null. other node's next and other node's prev have to be null
        int insert(iterator other, bool nextOrPref = false) const {
            if (!node || !other || other.next() || other.prev()) 
                return 0; // check requirements

            auto nx = next().node;
            auto pv = prev().node;

            // insert next
            if (!nextOrPref) {
                other.node->prev = node;
                node->next = other.node;
                other.node->next = nx;
                if (nx) nx->prev = other.node;
                return 1;
            }

            // insert prev
            other.node->prev = pv;
            if (pv) pv->next = other.node;
            other.node->next = node;
            node->prev = other.node;
            return 1;
        }

        /// insert other iterator to the prev position of this iterator
        int insertPrev(iterator other) const { return insert(other, true); }

        /// detach this iterator from its next and prev iterator
        /// @retval 1: success, 0: already detached or this node = null
        int detach() {
            auto nx = next().node;
            auto pv = prev().node;
            if (nx) nx->prev = pv;
            if (pv) pv->next = nx;
            return nx || pv;
        }

        /// detach and delete this iterator
        int erase() {
            int res = detach();
            delete node;
            node = nullptr;
            return res;
        }

        /// get i-th item by dereference
        /// @warning make sure node + i is not null
        T& operator[](int i) const {
            auto it = (*this) + i;
            return *it;
        }

        /// return true if the node is not null
        explicit operator bool() const { return node != nullptr; }

        /// arrow operator to access the item's member
        T* operator->() { return item(); }

        /// arrow operator to access the item's member
        const T* operator->() const { return item(); }

        /// dereference operator
        /// @warning make sure node is not null
        T& operator*() { return node->item; }
        
        /// dereference operator
        /// @warning make sure node is not null
        const T& operator*() const { return node->item; }

        /* comparison operators */

        bool operator==(iterator other) const { return node == other.node; }
        bool operator!=(iterator other) const { return node != other.node; }

        /* arithmetic operators */

        iterator operator+(int pos) const {
            iterator res { node };
            if (pos > 0)
                for (; pos > 0 && res; res = res.next()) pos--;
            else if (pos < 0)
                for (; pos < 0 && res; res = res.prev()) pos++;
            return res;
        }
        
        iterator operator-(int pos) const {
            return *this + (-pos);
        }

        iterator& operator+=(int pos) {
            *this = *this + pos;
            return *this;
        }
        
        iterator& operator-=(int pos) {
            *this = *this - pos;
            return *this;
        }

        iterator& operator++() {
            if (node) node = node->next;
            return *this;
        }
        
        iterator operator++(int) {
            iterator res { node };
            if (node) node = node->next;
            return res;
        }

        iterator& operator--() {
            if (node) node = node->prev;
            return *this;
        }
        
        iterator operator--(int) {
            iterator res { node };
            if (node) node = node->prev;
            return res;
        }

        size_t operator-(iterator other) const {
            size_t i = 0;
            if (node == nullptr) {
                for (; other; ++other, ++i);
                return i;
            }
            for (auto p = *this; p && p != other; --p, ++i);
            return i;
        }
    };

    /// type traits
    template <typename T> struct is_linked_list : false_type {};
    template <typename T> struct is_linked_list<LinkedList<T>> : true_type {};
    template <typename T> struct is_linked_list<const LinkedList<T>> : true_type {};
    template <typename T> struct is_linked_list<volatile LinkedList<T>> : true_type {};
    template <typename T> struct is_linked_list<const volatile LinkedList<T>> : true_type {};
    template <typename T> inline constexpr bool is_linked_list_v = is_linked_list<T>::value;

    template <typename T> struct remove_extent<LinkedList<T>> { typedef T type; };
    template <typename T> struct remove_extent<const LinkedList<T>> { typedef T type; };
    template <typename T> struct remove_extent<volatile LinkedList<T>> { typedef T type; };
    template <typename T> struct remove_extent<const volatile LinkedList<T>> { typedef T type; };
}

#endif //ETL_LINKED_LIST_H
