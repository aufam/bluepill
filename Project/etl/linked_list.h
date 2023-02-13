#ifndef ETL_LINKED_LIST_H
#define ETL_LINKED_LIST_H

#ifndef ETL_LINKED_LIST_USE_MUTEX
#define ETL_LINKED_LIST_USE_MUTEX 1
#endif

#if ETL_LINKED_LIST_USE_MUTEX == 1
#include "etl/mutex.h"
#define ETL_LINKED_LIST_SCOPE_LOCK() MutexScope lock(mutex)
#else
#define ETL_LINKED_LIST_SCOPE_LOCK()
#endif

namespace Project::etl {

    /// doubly linked list. every item will be allocated to heap
    template <class T>
    struct LinkedList {
        typedef T Type;
        struct Node; ///< contains the item and pointer to next and prev items
        struct Iterator; ///< iterator class to modify increment, decrement, bool operator, and dereference of Node*

    private:
        mutable Iterator head = nullptr;
#if ETL_LINKED_LIST_USE_MUTEX == 1
        mutable Mutex mutex = {};
#endif
        template <class ...Ts>
        void construct_(T firstItem, Ts... items) {
            push(firstItem);
            if constexpr (sizeof...(Ts) > 0) construct_(items...);
        }

    public:
        template <class ...Ts>
        LinkedList(T firstItem, Ts... items) {
            init();
            construct_(firstItem, items...);
        }

        constexpr LinkedList() = default;

        virtual ~LinkedList() { deinit(); }

#if ETL_LINKED_LIST_USE_MUTEX == 1
        int init()   const { return mutex.init(); } /// init mutex
        int deinit() const { clear(); return mutex.deinit(); } /// clear all item and deinit mutex
#else
        int init()   const { return 1; }
        int deinit() const { clear(); return 1; } /// clear all item
#endif

        Iterator data()  const { return head; }
        Iterator begin() const { return head; }
        Iterator end()   const { return { nullptr }; }
        Iterator tail()  const { return head + (len() - 1); }

        /// @retval number of item
        [[nodiscard]] size_t len() const {
            size_t res = 0;
            for (const auto& _ : *this) res++;
            return res;
        }

        /// get the first item
        /// @warning make sure head is not null
        [[nodiscard]] T& front() { return *head; }
        [[nodiscard]] const T& front() const { return *head; }

        /// get the last item
        /// @warning make sure tail() is not null
        [[nodiscard]] T& back() { return *tail(); }
        [[nodiscard]] const T& back() const { return *tail(); }

        /// delete all items
        void clear() const {
            while (pop());
        }

        int push(const T& item) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            auto node = Iterator(item);
            if (!node) return 0;
            if (!head) {
                head = node;
                return 1;
            }
            return tail().insert(node);
        }

        int push(const T& item, size_t pos) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            auto node = Iterator(item);
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
            ETL_LINKED_LIST_SCOPE_LOCK();
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

        const LinkedList& operator <<(const T &item) const { push(item); return *this; }
        const LinkedList& operator >>(T &item)       const { pop(item); return *this; }

        explicit operator bool() const { return head; }

        /// get i-th item by dereference
        /// @warning if head + i = null, make new node and return last item
        [[nodiscard]] T& operator [](size_t i) {
            if (head + i != nullptr) return head[i];
            T dummy = {};
            pushBack(dummy);
            return back();
        }

        /// get i-th item by dereference
        /// @warning if head + i = null, make new node and return last item
        [[nodiscard]] const T& operator [](size_t i) const {
            if (head + i != nullptr) return head[i];
            T dummy = {};
            pushBack(dummy);
            return back();
        }
    };

    template <class T>
    struct LinkedList<T>::Node {
        T item;
        Node* next = nullptr;
        Node* prev = nullptr;
        explicit Node(const T& item) : item(item) {}
    };

    template <class T>
    struct LinkedList<T>::Iterator {
        Node* node;
        Iterator(Node* node) : node(node) {}
        explicit Iterator(const T& item) : node(new Node(item)) {}

        T* item() { return node ? &node->item : nullptr; }
        Iterator next() { return { node ? node->next : nullptr }; }
        Iterator prev() { return { node ? node->prev : nullptr }; }

        /// insert other iterator to the next/prev of this iterator
        /// @param other other iterator
        /// @param nextOrPref false: insert to next (default), true: insert to prev
        /// @retval 0: fail, 1: success
        /// @note other node and this node can't be null. other next and other prev have to be null
        int insert(Iterator other, bool nextOrPref = false) {
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
        int insertPrev(Iterator other) { return insert(other, true); }

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

        bool operator ==(const Iterator& other) const { return node == other.node; }
        bool operator !=(const Iterator& other) const { return node != other.node; }
        explicit operator bool() const { return node != nullptr; }

        Iterator operator +(int pos) const {
            Iterator res { node };
            if (pos > 0)
                for (; pos > 0 && res; res = res.next()) pos--;
            else if (pos < 0)
                for (; pos < 0 && res; res = res.prev()) pos++;
            return res;
        }
        Iterator operator -(int pos) const {
            return *this + (-pos);
        }

        Iterator& operator +=(int pos) {
            *this = *this + pos;
            return *this;
        }
        Iterator& operator -=(int pos) {
            *this = *this - pos;
            return *this;
        }

        Iterator& operator ++() {
            if (node) node = node->next;
            return *this;
        }
        Iterator operator ++(int) {
            Iterator res { node };
            if (node) node = node->next;
            return res;
        }

        Iterator& operator --() {
            if (node) node = node->prev;
            return *this;
        }
        Iterator operator --(int) {
            Iterator res { node };
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
