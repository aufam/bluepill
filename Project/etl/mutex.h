#ifndef ETL_MUTEX_H
#define ETL_MUTEX_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/utility.h"

namespace Project::etl {

    /// FreeRTOS mutex
    /// @note requires cmsis os v2
    class Mutex {
        StaticSemaphore_t controlBlock;
    public:
        osMutexId_t id;

        /// empty constructor
        constexpr Mutex() : controlBlock{}, id(nullptr) {}

        /// disable copy constructor
        Mutex(const Mutex&) = delete;

        /// disable copy assignment
        Mutex& operator=(const Mutex&) = delete;

        /// move constructor
        constexpr Mutex(Mutex&& m)
        : controlBlock(move(m.controlBlock))
        , id(m.id ? &controlBlock : nullptr) { m.id = nullptr; }

        /// move assignment
        constexpr Mutex& operator=(Mutex&& other) noexcept {
            controlBlock = move(other.controlBlock);
            id = other.id ? &controlBlock : nullptr;
            other.id = nullptr;
            return *this;
        }

        /// initiate mutex
        /// @param name string name, default null
        /// @retval @ref osOK: success, @ref osError: failed (already initiated)
        osStatus_t init(const char* name = nullptr) {
            if (id) return osError;
            osMutexAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            id = osMutexNew(&attr);
            return osOK;
        }

        /// deinit mutex
        /// @retval @ref osOK: success, @ref osError: failed (already deinitiated)
        osStatus_t deinit() {
            if (id == nullptr) return osError;
            if (osMutexDelete(id) == osOK) id = nullptr;
            return osOK;
        }

        /// lock mutex
        /// @param timeout in tick, default = @ref osWaitForever
        /// @retval osStatusXxx
        osStatus_t lock(uint32_t timeout = osWaitForever) const { return osMutexAcquire(id, timeout); }

        /// unlock mutex
        /// @retval osStatusXxx
        osStatus_t unlock() const { return osMutexRelease(id); }
    };

    /// lock mutex when entering a scope and unlock when exiting
    struct MutexScope {
        osMutexId_t mutex;

        explicit MutexScope(Mutex& mutex, uint32_t timeout = osWaitForever) : mutex(mutex.id) {
            mutex.lock(timeout);
        }
        explicit MutexScope(osMutexId_t mutex, uint32_t timeout = osWaitForever) : mutex(mutex) {
            osMutexAcquire(mutex, timeout);
        }
        ~MutexScope() { osMutexRelease(mutex); }
    };
}

#endif //ETL_MUTEX_H
