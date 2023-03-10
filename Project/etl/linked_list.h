#ifndef ETL_LINKED_LIST_H
#define ETL_LINKED_LIST_H

#include "etl/utility.h"

namespace Project::etl {

    /// doubly linked list. every item will be allocated to heap
    template <class T>
    struct LinkedList {
        struct iterator; ///< modify increment, decrement, bool operator, and dereference of Node*
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;

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
        constexpr LinkedList() : head(nullptr) {}

        /// variadic template function constructor
        template <class ...Ts>
        LinkedList(T firstItem, Ts... items) : head(nullptr) { construct_(firstItem, items...); }

        /// copy constructor
        LinkedList(const LinkedList& l) : head(nullptr) {
            for (auto& item : l) push(item);
        }

        /// move constructor
        LinkedList(LinkedList&& l) noexcept : head(move(l.head)) {
            l.head = nullptr;
        }

        /// copy assignment
        LinkedList& operator=(const LinkedList& other) {
            for (auto i : range(other.len())) operator[](i) = other[i];
            return *this;
        }

        /// move assignment
        LinkedList& operator=(LinkedList&& other) noexcept {
            clear();
            head = move(other.head);
            other.head = nullptr;
            return *this;
        }

        virtual ~LinkedList() { clear(); }

        iterator data()  const { return head; }
        iterator begin() const { return head; }
        iterator end()   const { return {nullptr }; }
        iterator tail()  const { return head + (len() - 1); }

        /// @retval number of item
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
        const_reference front() const { return *head; }

        /// get the last item
        /// @warning make sure tail() is not null
        reference back() { return *tail(); }
        const_reference back() const { return *tail(); }

        /// get i-th item by dereference
        /// @warning if head + i = null, it will make new node and return its item
        /// @warning if i less than 0, it will iterate backward from the tail
        reference operator [](int i) {
            auto p = i >= 0 ? head + i : tail() + (i + 1);
            if (p) return *p;
            T dummy = {};
            i >= 0 ? pushBack(dummy) : pushFront(dummy);
            return i >= 0 ? back() : front();
        }

        /// get i-th item by dereference
        /// @warning if head + i = null, it will make new node and return its item
        /// @warning if i less than 0, it will iterate backward from the tail
        const_reference operator [](int i) const {
            auto p = i >= 0 ? head + i : tail() + (i + 1);
            if (p) return *p;
            T dummy = {};
            i >= 0 ? pushBack(dummy) : pushFront(dummy);
            return i >= 0 ? back() : front();
        }

        explicit operator bool() const { return head; }

        /// push operator
        const LinkedList& operator <<(const T &item) const { push(item); return *this; }
        const LinkedList& operator >>(T &item)       const { pop(item); return *this; }

        template <class Container>
        bool operator==(const Container& other) const { return compare_all(*this, other); }

        template <class Container>
        bool operator!=(const Container& other) const { return !operator==(other); }

        int push(const T& item) const {
            auto node = iterator(item);
            if (!node) return 0;
            if (!head) {
                head = node;
                return 1;
            }
            return tail().insert(node);
        }

        int push(const T& item, size_t pos) const {
            auto node = iterator(item);
            if (!node) return 0;
            if (pos == 0) {
                head.insertPrev(node);
                head = node;
                return 1;
            }
            int res = (head + (pos - 1)).insert(node);
            if (res == 0) node.erase();
            return res;
        }

        int pushBack(const T& item)  const { return push(item); }
        int pushFront(const T& item) const { return push(item, 0); }

        int pop(T& item, size_t pos = 0) const {
            auto node = head + pos;
            if (node) item = *node;
            if (pos == 0) head = head.next();
            return node.erase();
        }

        int pop()             const { T dummy = {}; return pop(dummy); }
        int pop(size_t pos)   const { T dummy = {}; return pop(dummy, pos); }
        int popBack()         const { T dummy = {}; return pop(dummy, len() - 1); }
        int popFront()        const { T dummy = {}; return pop(dummy); }
        int popBack(T& item)  const { return pop(item, len() - 1); }
        int popFront(T& item) const { return pop(item); }
    };

    /// create linked list with variadic template function, type is deduced
    template<typename T, typename... U> LinkedList<enable_if_t<(is_same_v<T, U> && ...), T>>
    list(const T& t, const U&...u) { return LinkedList<T>{t, u...}; }

    template <class T>
    struct LinkedList<T>::Node {
        T item;
        Node* next = nullptr;
        Node* prev = nullptr;
        explicit Node(const T& item) : item(item) {}
    };

    template <class T>
    struct LinkedList<T>::iterator {
        Node* node;
        iterator(Node* node) : node(node) {}
        explicit iterator(const T& item) : node(new Node(item)) {}

        T* item() { return node ? &node->item : nullptr; }
        iterator next() { return {node ? node->next : nullptr }; }
        iterator prev() { return {node ? node->prev : nullptr }; }

        /// insert other iterator to the next/prev of this iterator
        /// @param other other iterator
        /// @param nextOrPref false: insert to next (default), true: insert to prev
        /// @retval 0: fail, 1: success
        /// @note other node and this node can't be null. other next and other prev have to be null
        int insert(iterator other, bool nextOrPref = false) {
            if (!node || !other || other.next() || other.prev()) return 0;
            auto nx = next().node;
            auto pv = prev().node;
            if (!nextOrPref) {
                other.node->prev = node;
                node->next = other.node;
                other.node->next = nx;
                if (nx) nx->prev = other.node;
                return 1;
            }
            other.node->prev = pv;
            if (pv) pv->next = other.node;
            other.node->next = node;
            node->prev = other.node;
            return 1;
        }
        int insertPrev(iterator other) { return insert(other, true); }

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

        /// dereference operator
        /// @warning make sure node is not null
        T& operator *() { return node->item; }
        const T& operator *() const { return node->item; }

        bool operator ==(const iterator& other) const { return node == other.node; }
        bool operator !=(const iterator& other) const { return node != other.node; }
        explicit operator bool() const { return node != nullptr; }

        iterator operator +(int pos) const {
            iterator res {node };
            if (pos > 0)
                for (; pos > 0 && res; res = res.next()) pos--;
            else if (pos < 0)
                for (; pos < 0 && res; res = res.prev()) pos++;
            return res;
        }
        iterator operator -(int pos) const {
            return *this + (-pos);
        }

        iterator& operator +=(int pos) {
            *this = *this + pos;
            return *this;
        }
        iterator& operator -=(int pos) {
            *this = *this - pos;
            return *this;
        }

        iterator& operator ++() {
            if (node) node = node->next;
            return *this;
        }
        iterator operator ++(int) {
            iterator res {node };
            if (node) node = node->next;
            return res;
        }

        iterator& operator --() {
            if (node) node = node->prev;
            return *this;
        }
        iterator operator --(int) {
            iterator res {node };
            if (node) node = node->prev;
            return res;
        }

        /// get i-th item by dereference
        /// @warning make sure node + i is not null
        T& operator[](int i) const {
            auto it = (*this) + i;
            return *it;
        }
    };

}

#endif //ETL_LINKED_LIST_H
