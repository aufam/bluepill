#ifndef ETL_TIMER_H
#define ETL_TIMER_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/utility.h"

namespace Project::etl {

    /// FreeRTOS timer
    /// @note requires cmsis os v2
    class Timer {
        StaticTimer_t controlBlock;
    public:
        osTimerId_t id;

        typedef void (*Function) (void *arg);

        /// empty constructor
        constexpr Timer() : controlBlock{}, id(nullptr) {}

        /// disable copy constructor
        Timer(const Timer&) = delete;

        /// disable copy assignment
        Timer& operator=(const Timer&) = delete;

        /// move constructor
        constexpr Timer(Timer&& t) noexcept
        : controlBlock(move(t.controlBlock))
        , id(t.id ? &controlBlock : nullptr) { t.id = nullptr; }

        constexpr Timer& operator=(Timer&& other) noexcept {
            controlBlock = move(other.controlBlock);
            id = other.id ? &controlBlock : nullptr;
            return *this;
        }

        /// initiate timer
        /// @retval @ref osOK: success, @ref osError: failed (already initiated)
        osStatus_t init(uint32_t interval,                      ///< in ms
                        Function fn,                            ///< function pointer
                        void *arg = nullptr,                    ///< function argument, default null
                        osTimerType_t type = osTimerPeriodic,   ///< @ref osTimerPeriodic (default) or @ref osTimerOnce
                        const char *name = nullptr,             ///< string name, default null
                        bool startNow = true                    ///< true: start now (default), false: start later
        ) {
            if (id) return osError;
            osTimerAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            id = osTimerNew(fn, type, arg, &attr);
            if (startNow) osTimerStart(id, interval);
            return osOK;
        }

        /// deinit timer
        /// @retval @ref osOK: success, @ref osError: failed (already initiated)
        osStatus_t deinit() {
            if (id == nullptr) return osError;
            if (osTimerDelete(id) == osOK) id = nullptr;
            return osOK;
        }

        /// start timer
        /// @param interval interval in ms
        /// @retval osStatusXxx
        osStatus_t start(uint32_t interval) const { return osTimerStart(id, interval); }

        /// stop timer
        /// @retval osStatusXxx
        osStatus_t stop() const { return osTimerStop(id); }

        /// check if this timer is running
        bool isRunning() { return osTimerIsRunning(id); }
    };

}

#endif //ETL_TIMER_H
