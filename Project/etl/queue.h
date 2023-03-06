#ifndef ETL_QUEUE_H
#define ETL_QUEUE_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

namespace Project::etl {

    /// FreeRTOS message queue
    /// @tparam T item type
    /// @tparam N maximum number of item
    /// @note requires cmsis os v2
    template <class T, size_t N>
    class Queue {
        T buffer[N] = {};
        StaticQueue_t controlBlock = {};
    public:
        osMessageQueueId_t id = nullptr;
        constexpr Queue() = default;

        typedef T Type;

        /// initiate queue
        /// @param name string name, default null
        /// @retval @ref osOK: success, @ref osError: failed (already initiated)
        osStatus_t init(const char *name = nullptr) {
            if (id) return osError;
            osMessageQueueAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.mq_mem = buffer;
            attr.mq_size = sizeof(buffer);
            id = osMessageQueueNew(N, sizeof(T), &attr);
            return osOK;
        }

        /// deinit queue
        /// @retval @ref osOK: success, @ref osError: failed (already initiated)
        osStatus_t deinit() {
            if (id == nullptr) return osError;
            if (osMessageQueueDelete(id) == osOK) id = nullptr;
            return osOK;
        }

        /// push an item to the queue
        /// @param[in] item the item
        /// @param[out] timeout in tick, default 0
        /// @param[in] prio priority level, default 0 (lowest)
        /// @retval osStatusXxx
        osStatus_t push(const T &item, uint32_t timeout = 0, uint8_t prio = 0) {
            return osMessageQueuePut(id, &item, prio, timeout);
        }

        /// pop first item from the queue
        /// @param[out] item first item from queue
        /// @param[out] timeout in tick, default 0
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @retval osStatusXxx
        osStatus_t pop(T &item, uint32_t timeout = 0, uint8_t *prio = nullptr) {
            return osMessageQueueGet(id, &item, prio, timeout);
        }

        /// pop first item from the queue
        /// @param[in] timeout wait in tick, default 0
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @retval first item from queue, assuming no error
        T pop(uint32_t timeout = 0, uint8_t *prio = nullptr) {
            T item = {};
            pop(item, timeout, prio);
            return item;
        }

        auto clear()    { return osMessageQueueReset(id); }        ///< clear all items
        auto rem()      { return osMessageQueueGetSpace(id); }     ///< remaining space
        auto len()      { return osMessageQueueGetCount(id); }     ///< number of items
        auto maxLen()   { return osMessageQueueGetCapacity(id); }  ///< maximum number of item
        auto itemSize() { return osMessageQueueGetMsgSize(id); }   ///< item size in bytes

        T* data()  { return buffer; }
        T* begin() { return buffer; }
        T* end()   { return buffer + len(); }
        T& front() { return buffer[0]; }            ///< get first item, no pop
        T& back()  { return buffer[len() - 1]; }    ///< get last item, no pop
        T& operator [](size_t i) { return buffer[i]; }

        Queue &operator << (const T &item) { push(item); return *this; } ///< push operator, no wait
        Queue &operator >> (T &item) { pop(item); return *this; }        ///< pop operator, no wait

        /// return true if len > 0
        explicit operator bool () { return len() > 0; }
    };

}

#endif //ETL_QUEUE_H
