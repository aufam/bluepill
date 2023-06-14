#ifndef ETL_TIMER_H
#define ETL_TIMER_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/utility.h"

namespace Project::etl {

    /// FreeRTOS timer interface
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    class TimerInterface {
    protected:
        osTimerId_t id; ///< timer pointer

    public:
        /// default constructor
        explicit constexpr TimerInterface(osTimerId_t id) : id(id) {}

        /// move constructor
        TimerInterface(TimerInterface&& t) : id(etl::exchange(t.id, nullptr)) {}

        /// move assignment
        TimerInterface& operator=(TimerInterface&& t) { 
            if (this == &t) return *this;
            detach(); 
            id = etl::exchange(t.id, nullptr); 
            return *this; 
        }

        /// default destructor
        ~TimerInterface() { detach(); }

        TimerInterface(const TimerInterface&) = delete;               ///< disable copy constructor
        TimerInterface& operator=(const TimerInterface&) = delete;    ///< disable copy assignment
        
        /// return true if id is not null
        explicit operator bool() { return (bool) id; }

        osTimerId_t get() { return id; }

        /// name as null terminated string
        /// @note cannot be called from ISR
        const char* getName() { return osTimerGetName(id); }

        /// start or restart the timer
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t start(uint32_t interval) { return osTimerStart(id, interval); }

        /// start or restart the timer
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t stop() { return osTimerStop(id); }

        /// check if the timer is running
        /// @note cannot be called from ISR
        bool isRunning() { return osTimerIsRunning(id); }

        /// stop this timer, detach the callback function and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { stop(); return osTimerDelete(etl::exchange(id, nullptr)); }
    };

    /// FreeRTOS timer
    /// @note requires cmsis os v2
    class Timer : public TimerInterface {
        StaticTimer_t controlBlock = {};

    public:
        /// empty constructor
        constexpr Timer() : TimerInterface(nullptr) {}

        Timer(const Timer&) = delete; ///< disable copy constructor
        Timer(Timer&& t) = delete;    ///< disable move constructor

        Timer& operator=(const Timer&) = delete; ///< disable copy assignment
        Timer& operator=(Timer&&) = delete;      ///< disable move assignment

        /// initiate timer
        /// @param interval function call interval in ticks
        /// @param fn function pointer
        /// @param arg function argument
        /// @param type timer type, osTimerPeriodic (default) or osTimerOnce
        /// @param name as null terminated string, default = null
        /// @param startNow true (default): start now, false: start later
        /// @return osStatus
        /// @note cannot be called from ISR
        template <typename Fn, typename Arg>
        osStatus_t init(uint32_t interval, Fn&& fn, Arg *arg, osTimerType_t type = osTimerPeriodic, const char *name = nullptr, bool startNow = true) {
            if (this->id) osError;
            osTimerAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            auto fp = static_cast<void (*)(Arg*)>(etl::forward<Fn>(fn));
            this->id = osTimerNew(reinterpret_cast<void (*)(void*)>(fp), type, reinterpret_cast<void*>(arg), &attr);
            if (startNow) this->start(interval);
            return osOK;
        }

        /// initiate timer
        /// @param interval function call interval in ticks
        /// @param fn function pointer
        /// @param type timer type, osTimerPeriodic (default) or osTimerOnce
        /// @param name as null terminated string, default = null
        /// @param startNow true (default): start now, false: start later
        /// @return osStatus
        /// @note cannot be called from ISR
        template <typename Fn>
        osStatus_t init(uint32_t interval, Fn&& fn, osTimerType_t type = osTimerPeriodic, const char *name = nullptr, bool startNow = true) {
            if (this->id) return osError;
            osTimerAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            auto fp = static_cast<void (*)()>(etl::forward<Fn>(fn));
            this->id = osTimerNew(reinterpret_cast<void (*)(void*)>(fp), type, nullptr, &attr);
            if (startNow) this->start(interval);
            return osOK;
        }

        /// detach timer
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    /// create dynamic timer
    /// @param interval function call interval in ticks
    /// @param fn timer function
    /// @param arg timer function argument
    /// @param type timer type, osTimerPeriodic (default) or osTimerOnce
    /// @param name as null terminated string, default = null
    /// @param startNow true (default): start now, false: start later
    /// @return timer object
    /// @note cannot be called from ISR
    template <typename Fn, typename Arg>
    auto make_timer(uint32_t interval, Fn&& fn, Arg *arg, osTimerType_t type = osTimerPeriodic, const char *name = nullptr, bool startNow = true) {
        osTimerAttr_t attr = {};
        attr.name = name;
        auto fp = static_cast<void (*)(Arg*)>(etl::forward<Fn>(fn));
        auto res = TimerInterface(osTimerNew(reinterpret_cast<void (*)(void*)>(fp), type, reinterpret_cast<void*>(arg), &attr));
        if (startNow) res.start(interval);
        return etl::move(res); 
    }
    
    /// create dynamic timer
    /// @param interval function call interval in ticks
    /// @param fn timer function
    /// @param type timer type, osTimerPeriodic (default) or osTimerOnce
    /// @param name as null terminated string, default = null
    /// @param startNow true (default): start now, false: start later
    /// @return timer object
    /// @note cannot be called from ISR
    template <typename Fn>
    auto make_timer(uint32_t interval, Fn&& fn, osTimerType_t type = osTimerPeriodic, const char *name = nullptr, bool startNow = true) {
        osTimerAttr_t attr = {};
        attr.name = name;
        auto fp = static_cast<void (*)()>(etl::forward<Fn>(fn));
        auto res = TimerInterface(osTimerNew(reinterpret_cast<void (*)(void*)>(fp), type, nullptr, &attr));
        if (startNow) res.start(interval);
        return etl::move(res); 
    }

}

#endif //ETL_TIMER_H
