#ifndef ETL_EVENT_H
#define ETL_EVENT_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/utility.h"

namespace Project::etl {

    // provides a convenient way to manage and manipulate flags represented by a 32-bit unsigned integer
    class FlagManager {
        uint32_t flags;
    
    public:
        // construct a FlagManager object with the specified initial set of flags
        FlagManager(uint32_t flags) : flags(flags) {}

        // return true if osFlagsError flag is not set
        explicit operator bool() const { return !(flags & osFlagsError); }

        // return true if osFlagsError flag is not set and the specified flags are set
        bool operator&(uint32_t other) const { return operator bool() && (flags & other); }

        // return true if osFlagsError flag is not set and the specified flags are the same as this flags
        bool operator==(uint32_t other) const { return operator bool() && (flags == other); }

        bool operator!=(uint32_t other) const { return !operator ==(other); }

        // return raw flags
        uint32_t get() const { return flags; }
    };

    /// FreeRTOS event interface.
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    class EventInterface {
    protected:
        osEventFlagsId_t id; ///< event pointer
        
    public:
        /// default constructor
        explicit constexpr EventInterface(osEventFlagsId_t id) : id(id) {}

        /// move constructor
        EventInterface(EventInterface&& e) noexcept : id(etl::exchange(e.id, nullptr)) {}

        /// move assignment
        EventInterface& operator=(EventInterface&& e) noexcept {
            if (this == &e) return *this;
            detach(); 
            id = etl::exchange(e.id, nullptr); 
            return *this;
        }

        /// default destructor
        ~EventInterface() { detach(); }

        EventInterface(const EventInterface&) = delete;               ///< disable copy constructor
        EventInterface& operator=(const EventInterface&) = delete;    ///< disable copy assignment
        
        /// return true if id is not null
        explicit operator bool() { return (bool) id; }

        /// get event pointer
        osEventFlagsId_t get() { return id; }

        /// name as null terminated string
        const char* getName() { return osEventFlagsGetName(id); }  

        /// set flags of this event
        /// @param flags specifies the flags that shall be set
        /// @return flags after setting or error code if highest bit set
        /// @note can be called from ISR
        uint32_t setFlags(uint32_t flags) { return osEventFlagsSet(id, flags); }

        /// reset the specified flags of this event
        /// @param flags specifies the flags that shall be reset
        /// @return flags before resetting or error code if highest bit set
        /// @note can be called from ISR
        uint32_t resetFlags(uint32_t flags) { return osEventFlagsClear(id, flags); }

        /// get the flags of this event
        /// @return current thread's flags
        /// @note can be called from ISR
        uint32_t getFlags() { return osEventFlagsGet(id); }

        /// wait for flags of this event to become signaled
        /// @param flags specifies the flags to wait for
        /// @param option osFlagsWaitAny (default) or osFlagsWaitAny
        /// @param timeout default = osWaitForever
        /// @param doReset specifies wether reset the flags or not, default = true
        /// @return flags before resetting or error code if highest bit set
        /// @note can be called from ISR if timeout == 0
        FlagManager waitFlags(uint32_t flags, uint32_t option = osFlagsWaitAny, uint32_t timeout = osWaitForever, bool doReset = true) { 
            if (!doReset) option |= osFlagsNoClear;
            return osEventFlagsWait(id, flags, option, timeout); 
        }

        /// wait for any flags of this event to become signaled
        /// @param timeout default = osWaitForever
        /// @param doReset specifies wether reset the flags or not, default = true
        /// @return flags before resetting or error code if highest bit set
        /// @note can be called from ISR if timeout == 0
        FlagManager waitFlagsAny(uint32_t timeout = osWaitForever, bool doReset = true) { 
            uint32_t flags = (1u << 24) - 1; // all possible flags
            uint32_t option = osFlagsWaitAny;
            if (!doReset) option |= osFlagsNoClear;
            return osEventFlagsWait(id, flags, option, timeout); 
        }

        /// delete resource and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { return osEventFlagsDelete(etl::exchange(id, nullptr)); }

        /// set operator
        EventInterface& operator|(uint32_t flags) { setFlags(flags); return *this; }

        /// reset operator
        EventInterface& operator&(uint32_t flags) { resetFlags(flags); return *this; }
    };

    /// FreeRTOS static mutex.
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    /// @note invoke init method before using
    class Event : public EventInterface {
        StaticEventGroup_t controlBlock = {};

    public:
        /// default constructor
        constexpr Event() : EventInterface(nullptr) {}

        Event(const Event&) = delete; ///< disable copy constructor
        Event(Event&& t) = delete;    ///< disable move constructor

        Event& operator=(const Event&) = delete;  ///< disable copy assignment
        Event& operator=(Event&&) = delete;       ///< disable move assignment

        /// initiate event
        /// @param name string name, default null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(const char* name = nullptr) {
            if (this->id) return osError;
            osEventFlagsAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            this->id = osEventFlagsNew(&attr);
            return osOK;
        }

        /// deinit event
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    /// create dynamic event
    /// @param name as null terminated string, default = null
    /// @return event object
    /// @note cannot be called from ISR
    inline auto make_event(const char* name = nullptr) {
        osEventFlagsAttr_t attr = {};
        attr.name = name;
        return EventInterface(osEventFlagsNew(&attr));
    }
}

#endif