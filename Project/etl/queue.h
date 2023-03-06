#ifndef ETL_QUEUE_H
#define ETL_QUEUE_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/array.h"

namespace Project::etl {

    /// FreeRTOS message queue
    /// @tparam T item type
    /// @tparam N maximum number of item
    /// @note requires cmsis os v2
    template <class T, size_t N>
    class Queue {
        mutable Array<T, N> buffer;
        mutable StaticQueue_t controlBlock;
    public:
        mutable osMessageQueueId_t id;

        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;

        /// empty constructor
        constexpr Queue() : buffer{}, controlBlock{}, id(nullptr) {}

        /// copy constructor
        constexpr Queue(const Queue& q)
        : buffer(q.buffer)
        , controlBlock(q.controlBlock)
        , id(q.id ? &controlBlock : nullptr) {}

        /// copy assignment
        constexpr Queue& operator=(const Queue& other) {
            copy(other.buffer, buffer);
            controlBlock = other.controlBlock;
            id = other.id ? &controlBlock : nullptr;
            return *this;
        }

        /// move constructor
        constexpr Queue(Queue&& q) noexcept
        : buffer(move(q.buffer))
        , controlBlock(move(controlBlock))
        , id(q.id ? &controlBlock : nullptr) { q.id = nullptr; }

        /// move assignment
        constexpr Queue& operator=(Queue&& other) noexcept {
            buffer = move(other.buffer);
            controlBlock = move(other.controlBlock);
            id = other.id ? &controlBlock : nullptr;
            return *this;
        }

        /// initiate queue
        /// @param name string name, default null
        /// @retval @ref osOK: success, @ref osError: failed (already initiated)
        osStatus_t init(const char *name = nullptr) const {
            if (id) return osError;
            osMessageQueueAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.mq_mem = &buffer;
            attr.mq_size = sizeof(buffer);
            id = osMessageQueueNew(N, sizeof(T), &attr);
            return osOK;
        }

        /// deinit queue
        /// @retval @ref osOK: success, @ref osError: failed (already initiated)
        osStatus_t deinit() const {
            if (id == nullptr) return osError;
            if (osMessageQueueDelete(id) == osOK) id = nullptr;
            return osOK;
        }

        auto clear()    const { return osMessageQueueReset(id); }        ///< clear all items
        auto rem()      const { return osMessageQueueGetSpace(id); }     ///< remaining space
        auto len()      const { return osMessageQueueGetCount(id); }     ///< number of items
        auto size()     const { return osMessageQueueGetCapacity(id); }  ///< maximum number of item
        auto itemSize() const { return osMessageQueueGetMsgSize(id); }   ///< item size in bytes

        iterator data()   { return buffer; }
        iterator begin()  { return buffer; }
        iterator end()    { return buffer + len(); }
        reference front() { return buffer[0]; }            ///< get first item, no pop
        reference back()  { return buffer[len() - 1]; }    ///< get last item, no pop

        const_iterator data()   const { return buffer; }
        const_iterator begin()  const { return buffer; }
        const_iterator end()    const { return buffer + len(); }
        const_reference front() const { return buffer[0]; }            ///< get first item, no pop
        const_reference back()  const { return buffer[len() - 1]; }    ///< get last item, no pop

        /// get the i-th item without removing from the queue
        reference operator [](int i) {
            if (len() == 0) return buffer[0];
            if (i < 0) i = len() + i; // allowing negative index
            return buffer[i];
        }

        /// get the i-th item without removing from the queue
        const_reference operator [](int i) const {
            if (len() == 0) return buffer[0];
            if (i < 0) i = len() + i; // allowing negative index
            return buffer[i];
        }

        explicit operator bool() const { return len() > 0; } ///< return true if len > 0

        Queue& operator<<(const T &item) { push(item); return *this; }             ///< push operator, no wait
        Queue& operator>>(T &item) { pop(item); return *this; }                    ///< pop operator, no wait

        const Queue& operator<<(const T &item) const { push(item); return *this; }
        const Queue& operator>>(T &item) const { pop(item); return *this; }

        /// push an item to the queue
        /// @param[in] item the item
        /// @param[out] timeout in tick, default 0
        /// @param[in] prio priority level, default 0 (lowest)
        /// @retval osStatusXxx
        osStatus_t push(const T &item, uint32_t timeout = 0, uint8_t prio = 0) const {
            return osMessageQueuePut(id, &item, prio, timeout);
        }

        /// pop first item from the queue
        /// @param[out] item first item from queue
        /// @param[out] timeout in tick, default 0
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @retval osStatusXxx
        osStatus_t pop(T &item, uint32_t timeout = 0, uint8_t *prio = nullptr) const {
            return osMessageQueueGet(id, &item, prio, timeout);
        }

        /// pop first item from the queue
        /// @param[in] timeout wait in tick, default 0
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @retval first item from queue, assuming no error
        T pop(uint32_t timeout = 0, uint8_t *prio = nullptr) const {
            T item = {};
            pop(item, timeout, prio);
            return item;
        }
    };

}

#endif //ETL_QUEUE_H
