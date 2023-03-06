#ifndef ETL_THREAD_H
#define ETL_THREAD_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/array.h"

namespace Project::etl {

    /// FreeRTOS thread
    /// @tparam N buffer length (in words)
    /// @note requires cmsis os v2
    template <size_t N = configMINIMAL_STACK_SIZE>
    class Thread {
        Array<uint32_t, N> buffer;
        StaticTask_t controlBlock;
    public:
        osThreadId_t id;

        typedef void (* Function) (void* );

        /// empty constructor
        constexpr Thread() : buffer{}, controlBlock{}, id(nullptr) {}

        /// disable copy constructor
        Thread(const Thread&) = delete;

        /// disable copy assignment
        Thread& operator=(const Thread&) = delete;

        /// move constructor
        constexpr Thread(Thread&& t) noexcept
        : buffer(move(t.buffer))
        , controlBlock(move(t.controlBlock))
        , id(t.id ? &controlBlock : nullptr) { t.id = nullptr; }

        /// move assignment
        constexpr Thread& operator=(Thread&& other) noexcept {
            buffer = move(other.buffer);
            controlBlock = move(other.controlBlock);
            id = other.id ? &controlBlock : nullptr;
            return *this;
        }

        /// initiate thread
        /// @retval @ref osOK: success, @ref osError: failed (already initiated)
        osStatus_t init(Function fn,                            ///< function pointer
                        void* arg = nullptr,                    ///< function argument, default = null
                        osPriority_t prio = osPriorityNormal,   ///< osPriorityXxx, default = @ref osPriorityNormal
                        const char* name = nullptr              ///< string name, default null
        ) {
            if (id) return osError;
            osThreadAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.stack_mem = &buffer;
            attr.stack_size = sizeof(buffer); // in byte
            attr.priority = prio;
            id = osThreadNew(fn, arg, &attr);
            return osOK;
        }

        /// terminate thread
        /// @retval @ref osOK: success, @ref osError: failed (already deinitiated)
        osStatus_t deinit() {
            if (id == nullptr) return osError;
            if (osThreadTerminate(id) == osOK) id = nullptr;
            return osOK;
        }
    };

}

#endif //ETL_THREAD_H
