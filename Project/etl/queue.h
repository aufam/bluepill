#ifndef ETL_QUEUE_H
#define ETL_QUEUE_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/array.h"

namespace Project::etl {

    /// FreeRTOS queue interface
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    template <typename T>
    class QueueInterface {
    protected:
        osMessageQueueId_t id; ///< queue pointer

    public:
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;

        /// default constructor
        explicit constexpr QueueInterface(osMessageQueueId_t id) : id(id) {}

        /// move constructor
        QueueInterface(QueueInterface&& q) : id(etl::exchange(q.id, nullptr)) {}

        /// move assignment
        QueueInterface& operator=(QueueInterface&& q) { 
            if (this == &q) return *this;
            detach(); 
            id = etl::exchange(q.id, nullptr); 
            return *this;
        }

        /// default destructor
        ~QueueInterface() { detach(); }

        QueueInterface(const QueueInterface&) = delete;               ///< disable copy constructor
        QueueInterface& operator=(const QueueInterface&) = delete;    ///< disable copy assignment
        
        /// return true if id is not null
        explicit operator bool() { return (bool) id; }

        /// get queue pointer
        osMessageQueueId_t get() { return id; }

        /// push an item to the queue
        /// @param[in] item the item
        /// @param[in] timeout default timeImmediate
        /// @param[in] prio priority level, default 0 (lowest)
        /// @return osStatus
        /// @note can be called from ISR if timeout == 0
        osStatus_t push(const_reference item, etl::Time timeout = etl::timeImmediate, uint8_t prio = 0) { 
            return osMessageQueuePut(id, &item, prio, timeout.tick); 
        }
        
        /// pop first item out from the queue
        /// @param[out] item first item from queue
        /// @param[in] timeout default timeImmediate
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @return osStatus
        /// @note can be called from ISR if timeout == 0
        osStatus_t pop(reference item, etl::Time timeout = etl::timeImmediate, uint8_t* prio = nullptr) { 
            return osMessageQueueGet(id, &item, prio, timeout.tick); 
        }

        /// pop first item out from the queue
        /// @param[in] timeout wait in tick, default 0
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @return the first item
        /// @note can be called from ISR if timeout == 0
        value_type pop(etl::Time timeout = etl::timeImmediate, uint8_t *prio = nullptr) {
            value_type item = {};
            pop(item, timeout, prio);
            return item;
        }

        /// get the capacity
        /// @note cannot be called from ISR
        uint32_t size() { return osMessageQueueGetCapacity(id); }

        /// get the item size in bytes
        /// @note cannot be called from ISR
        uint32_t itemSize() { return osMessageQueueGetMsgSize(id); }

        /// get the number of items
        /// @note can be called from ISR
        uint32_t len() { return osMessageQueueGetCount(id); }

        /// get the remaining space
        /// @note can be called from ISR
        uint32_t rem() { return osMessageQueueGetSpace(id); }

        /// reset the queue
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t clear() { return osMessageQueueReset(id); }

        /// delete resource and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { return osMessageQueueDelete(etl::exchange(id, nullptr)); }

        /// return pointer of the data
        /// @note can be called from ISR
        iterator data() { 
            auto head = reinterpret_cast<StaticQueue_t*>(id);
            return reinterpret_cast<iterator>(head->pvDummy1[0]); 
        }

        /// return pointer of the first item
        /// @note can be called from ISR
        iterator begin() { return data(); }

        /// return pointer of one past the last item 
        /// @note can be called from ISR
        iterator end() { return begin() + len(); }

        /// return reference of first item 
        /// @note can be called from ISR
        /// @note the number of items has to be more than 0
        reference front() { return *begin(); }

        /// return reference of last item 
        /// @note can be called from ISR
        /// @note the number of items has to be more than 0
        reference back() { return *(begin() + (len() - 1)); }

        /// get the i-th item without removing from the queue
        /// @note can be called from ISR
        /// @note the number of items has to be more than 0
        reference operator[](int i) {
            auto l = len();
            if (l == 0) return *begin();
            if (i < 0) i = l + i; // allowing negative index
            return *(begin() + i);
        }

        /// push operator
        /// @param[in] item the first item
        /// @note can be called from ISR
        QueueInterface<T>& operator<<(const_reference item) { push(item); return *this; }

        /// pop operator
        /// @param[out] item the first item
        /// @note can be called from ISR
        QueueInterface<T>& operator>>(reference item) { pop(item); return *this; }

    };

    /// FreeRTOS static queue
    /// @tparam T item type
    /// @tparam N maximum number of items
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    template <class T, size_t N>
    class Queue : public QueueInterface<T> {
        StaticQueue_t controlBlock = {};
        Array<T, N> buffer = {};

    public:
        /// default constructor
        constexpr Queue() : QueueInterface<T>(nullptr) {}

        Queue(const Queue&) = delete; ///< disable copy constructor
        Queue(Queue&& t) = delete;    ///< disable move constructor

        Queue& operator=(const Queue&) = delete;  ///< disable copy assignment
        Queue& operator=(Queue&&) = delete;       ///< disable move assignment

        /// initiate queue
        /// @param name as null terminated string
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(const char *name = nullptr) {
            if (this->id) return osError;
            osMessageQueueAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.mq_mem = &buffer;
            attr.mq_size = sizeof(buffer);
            this->id = osMessageQueueNew(N, sizeof(T), &attr);
            return osOK;
        }

        /// deinit queue
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return this->detach(); }
    };

    /// create dynamic queue 
    /// @tparam T item type
    /// @param capacity maximum number of items
    /// @param name as null terminated string
    /// @return queue object
    /// @note cannot be called from ISR
    template <typename T>
    auto make_queue(uint32_t capacity, const char* name = nullptr) { 
        osMessageQueueAttr_t attr = {};
        attr.name = name;
        return QueueInterface<T>(osMessageQueueNew(capacity, sizeof(T), &attr)); 
    }

}

#endif //ETL_QUEUE_H
