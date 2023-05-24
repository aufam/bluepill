#ifndef ETL_MUTEX_H
#define ETL_MUTEX_H

#include "etl/thread.h"

namespace Project::etl {

    /// FreeRTOS mutex interface.
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    struct MutexInterface {
        osMutexId_t id; ///< mutex pointer

        /// default constructor
        explicit constexpr MutexInterface(osMutexId_t id) : id(id) {}

        /// move constructor
        MutexInterface(MutexInterface&& m) : id(etl::move(m.id)) { m.id = nullptr; }

        /// move assignment
        MutexInterface& operator=(MutexInterface&& m) { 
            if (this == &m) return *this;
            detach(); 
            id = etl::exchange(m.id, nullptr); 
            return *this;
        }

        /// default destructor
        ~MutexInterface() { detach(); }

        MutexInterface(const MutexInterface&) = delete;               ///< disable copy constructor
        MutexInterface& operator=(const MutexInterface&) = delete;    ///< disable copy assignment
        
        /// return true if id is not null
        explicit operator bool() { return (bool) id; }

        /// lock mutex
        /// @param timeout default = osWaitForever
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t lock(uint32_t timeout = osWaitForever) { return osMutexAcquire(id, timeout); }

        /// unlock mutex
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t unlock() { return osMutexRelease(id); }

        /// get thread that owns this mutex
        /// @return thread object
        /// @note cannot be called from ISR
        auto getOwner() { return etl::ThreadInterface(osMutexGetOwner(id)); }

        /// unlock mutex, delete resource, and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { unlock(); return osMutexDelete(etl::exchange(id, nullptr)); }
    };

    /// create dynamic mutex
    /// @param name string name, default null
    /// @return mutex object
    /// @note cannot be called from ISR
    auto mutex(const char* name = nullptr) {
        osMutexAttr_t attr = {};
        attr.name = name;
        return MutexInterface(osMutexNew(&attr));
    }

    /// FreeRTOS static mutex.
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    class Mutex : public MutexInterface {
        StaticSemaphore_t controlBlock = {};

    public:
        /// default constructor
        constexpr Mutex() : MutexInterface(nullptr) {}

        Mutex(const Mutex&) = delete; ///< disable copy constructor
        Mutex(Mutex&& t) = delete;    ///< disable move constructor

        Mutex& operator=(const Mutex&) = delete;  ///< disable copy assignment
        Mutex& operator=(Mutex&&) = delete;       ///< disable move assignment

        /// initiate mutex
        /// @param name string name, default null
        /// @return osStatus
        /// @note cannot be called from ISR
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
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    /// lock mutex when entering a scope and unlock when exiting
    struct MutexScope {
        osMutexId_t id;

        explicit MutexScope(osMutexId_t mutex, uint32_t timeout = osWaitForever) : id(mutex) {
            osMutexAcquire(id, timeout);
        }

        ~MutexScope() { osMutexRelease(id); }
    };

    /// lock scope
    /// @return mutex scope object
    /// @note cannot be called from ISR
    [[nodiscard("has to be assigned to a local scope variable")]] 
    auto lockScope(MutexInterface& mutex) { return MutexScope(mutex.id); }

    /// lock scope
    /// @return mutex scope object
    /// @note cannot be called from ISR
    [[nodiscard("has to be assigned to a local scope variable")]] 
    auto lockScope(Mutex& mutex) { return MutexScope(mutex.id); }

    /// lock scope
    /// @return mutex scope object
    /// @note cannot be called from ISR
    [[nodiscard("has to be assigned to a local scope variable")]] 
    auto lockScope(osMutexId_t mutex) { return MutexScope(mutex); }
}

#endif //ETL_MUTEX_H
