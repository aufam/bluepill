#ifndef ETL_THREAD_H
#define ETL_THREAD_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

namespace Project::etl {

    /// FreeRTOS thread
    /// @tparam N buffer length (in words)
    /// @note requires cmsis os v2
    template <size_t N = configMINIMAL_STACK_SIZE>
    class Thread {
        uint32_t buffer[N];
        StaticTask_t controlBlock;
    public:
        osThreadId_t id;
        constexpr Thread() : id(nullptr), buffer{}, controlBlock{} {}

        typedef void (* Function) (void* );

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
            attr.stack_mem = buffer;
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
