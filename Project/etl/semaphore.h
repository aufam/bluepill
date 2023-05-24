#ifndef ETL_SEMAPHORE_H
#define ETL_SEMAPHORE_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/utility.h"

namespace Project::etl {

    /// FreeRTOS semaphore interface
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    struct SemaphoreInterface {
        osSemaphoreId_t id; ///< semaphore pointer
        
        /// default constructor
        explicit constexpr SemaphoreInterface(osSemaphoreId_t id) : id(id) {}

        /// move constructor
        SemaphoreInterface(SemaphoreInterface&& s) : id(etl::move(s.id)) { s.id = nullptr; }

        /// move assignment
        SemaphoreInterface& operator=(SemaphoreInterface&& s) { 
            if (this == &s) return *this;
            detach(); 
            id = etl::exchange(s.id, nullptr); 
            return *this; 
        }

        /// default destructor
        ~SemaphoreInterface() { detach(); }

        SemaphoreInterface(const SemaphoreInterface&) = delete;               ///< disable copy constructor
        SemaphoreInterface& operator=(const SemaphoreInterface&) = delete;    ///< disable copy assignment
        
        /// return true if id is not null
        explicit operator bool() { return (bool) id; }

        /// acquire semaphore token, token counter will be decreased by one
        /// @param timeout default = osWaitForever
        /// @return osStatus
        /// @note can be called from ISR if timeout == 0
        osStatus_t acquire(uint32_t timeout = osWaitForever) { return osSemaphoreAcquire(id, timeout); }

        /// release semaphore token, token counter will be increased by one
        /// @return osStatus
        /// @note can be called from ISR
        osStatus_t release() { return osSemaphoreRelease(id); }

        /// get token counter
        /// @note can be called from ISR
        uint32_t count() { return osSemaphoreGetCount(id); }

        /// delete resource and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { return osSemaphoreDelete(etl::exchange(id, nullptr)); }

        /// release operator
        SemaphoreInterface& operator++(int) { release(); return *this; }

        /// release operator
        void operator++() { release(); }

        /// acquire operator, wait forever
        SemaphoreInterface& operator--(int) { acquire(); return *this; }

        /// acquire operator, wait forever
        void operator--() { acquire(); }
    };

    /// create dynamic semaphore
    /// @param maxCount maximum token counter, default = 1
    /// @param initialCount initial token counter, default = 0
    /// @param name as null terminated string, default = null
    /// @return semaphore object
    /// @note cannot be called from ISR
    inline auto semaphore(uint32_t maxCount = 1, uint32_t initialCount = 0, const char* name = nullptr) {
        osSemaphoreAttr_t attr = {};
        attr.name = name;
        return SemaphoreInterface(osSemaphoreNew(maxCount, initialCount, &attr));
    }

    /// FreeRTOS static semaphore
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    class Semaphore : public SemaphoreInterface {
        StaticSemaphore_t controlBlock = {};

    public:
        /// default constructor
        constexpr Semaphore() : SemaphoreInterface(nullptr) {}

        Semaphore(const Semaphore&) = delete; ///< disable copy constructor
        Semaphore(Semaphore&& t) = delete;    ///< disable move constructor

        Semaphore& operator=(const Semaphore&) = delete;  ///< disable copy assignment
        Semaphore& operator=(Semaphore&&) = delete;       ///< disable move assignment

        /// initiate semaphore
        /// @param maxCount maximum token counter, default = 1
        /// @param initialCount initial token counter, default = 0
        /// @param name as null terminated string, default = null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(uint32_t maxCount = 1, uint32_t initialCount = 0, const char* name = nullptr) {
            if (id) return osError;
            osSemaphoreAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            id = osSemaphoreNew(maxCount, initialCount, &attr);
            return osOK;
        }

        /// deinit semaphore
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };
}

#endif