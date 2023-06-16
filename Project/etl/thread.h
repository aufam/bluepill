#ifndef ETL_THREAD_H
#define ETL_THREAD_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/event.h"
#include "etl/array.h"

namespace Project::etl {

    /// FreeRTOS thread interface
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    class ThreadInterface {
    protected:
        osThreadId_t id; ///< thread pointer

    public:
        /// default constructor
        explicit constexpr ThreadInterface(osThreadId_t id) : id(id) {}

        /// move constructor
        ThreadInterface(ThreadInterface&& t) : id(etl::exchange(t.id, nullptr)) {}

        /// move assignment
        ThreadInterface& operator=(ThreadInterface&& t) { 
            if (this == &t) return *this;
            detach(); 
            id = etl::exchange(t.id, nullptr); 
            return *this; 
        }

        /// default destructor
        ~ThreadInterface() { detach(); }

        ThreadInterface(const ThreadInterface&) = delete;               ///< disable copy constructor
        ThreadInterface& operator=(const ThreadInterface&) = delete;    ///< disable copy assignment
        
        /// return true if id is not null
        explicit operator bool() { return (bool) id; }

        /// get thread pointer
        osThreadId_t get() { return id; }

        /// name as null terminated string
        /// @note cannot be called from ISR
        const char* getName() { return osThreadGetName(id); }

        /// return current thread state osThreadXxx
        /// @note cannot be called from ISR
        osThreadState_t getState() { return osThreadGetState(id); }

        /// remaining stack size in bytes
        /// @note cannot be called from ISR
        uint32_t getSpace() { return osThreadGetStackSpace(id); }

        /// change the priority
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t setPriority(osPriority_t prio) { return osThreadSetPriority(id, prio); }

        /// return the priority value
        /// @note cannot be called from ISR
        osPriority_t getPriority() { return osThreadGetPriority(id); }

        /// suspend execution of a thread
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t suspend() { return osThreadSuspend(id); }  

        /// resume execution of a thread
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t resume() { return osThreadResume(id); }

        /// terminate execution, delete resource, and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { return osThreadTerminate(etl::exchange(id, nullptr)); }

        /// send flags to this thread
        /// @param flags specifies the flags of the thread that shall be set
        /// @return this thread's flags after setting or error code if highest bit set
        /// @note can be called from ISR
        uint32_t setFlags(uint32_t flags) { return osThreadFlagsSet(id, flags); }

        /// set flags operator
        ThreadInterface& operator|(uint32_t flags) { setFlags(flags); return *this; }
    };
    
    /// FreeRTOS static thread
    /// @tparam N buffer length (in words)
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    /// @note invoke init method before using
    template <size_t N = configMINIMAL_STACK_SIZE>
    class Thread : public ThreadInterface {
        StaticTask_t controlBlock = {};
        Array<uint32_t, N> buffer = {};

    public:
        /// default constructor
        constexpr Thread() : ThreadInterface(nullptr) {}

        Thread(const Thread&) = delete; ///< disable copy constructor
        Thread(Thread&& t) = delete;    ///< disable move constructor

        Thread& operator=(const Thread&) = delete;  ///< disable copy assignment
        Thread& operator=(Thread&&) = delete;       ///< disable move assignment

        /// initiate thread
        /// @param fn thread function
        /// @param arg thread function argument
        /// @param prio osPriorityXxx, default = osPriorityNormal
        /// @param name as null terminated string, default = null
        /// @return osStatus
        /// @note cannot be called from ISR
        template <typename Fn, typename Arg>
        osStatus_t init(Fn&& fn, Arg* arg, osPriority_t prio = osPriorityNormal, const char* name = nullptr) { 
            if (this->id) return osError;
            osThreadAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.stack_mem = &buffer;
            attr.stack_size = sizeof(buffer); // in byte
            attr.priority = prio;
            auto fp = static_cast<void (*)(Arg*)>(etl::forward<Fn>(fn));
            this->id = osThreadNew(reinterpret_cast<void (*)(void*)>(fp), reinterpret_cast<void*>(arg), &attr);
            return osOK;
        }

        /// initiate thread
        /// @param fn function pointer
        /// @param prio osPriorityXxx, default = osPriorityNormal
        /// @param name as null terminated string, default = null
        /// @return osStatus
        /// @note cannot be called from ISR
        template <typename Fn>
        osStatus_t init(Fn&& fn, osPriority_t prio = osPriorityNormal, const char* name = nullptr) { 
            if (this->id) return osError;
            osThreadAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.stack_mem = &buffer;
            attr.stack_size = sizeof(buffer); // in byte
            attr.priority = prio;
            auto fp = static_cast<void (*)()>(etl::forward<Fn>(fn));
            this->id = osThreadNew(reinterpret_cast<void (*)(void*)>(fp), nullptr, &attr);
            return osOK;
        }

        /// detach thread
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    /// create dynamic thread
    /// @param fn thread function
    /// @param arg thread function argument
    /// @param stackSize in words (1 word = 4 bytes), default = configMINIMAL_STACK_SIZE
    /// @param prio osPriorityXxx
    /// @param name as null terminated string, default = null
    /// @return thread object
    /// @note cannot be called from ISR
    template <typename Fn, typename Arg>
    auto make_thread(Fn&& fn, Arg* arg, uint32_t stackSize = configMINIMAL_STACK_SIZE, osPriority_t prio = osPriorityNormal, const char* name = nullptr) {
        osThreadAttr_t attr = {};
        attr.name = name;
        attr.priority = prio;
        attr.stack_size = stackSize * 4;
        auto fp = static_cast<void (*)(Arg*)>(etl::forward<Fn>(fn));
        return ThreadInterface(osThreadNew(reinterpret_cast<void (*)(void*)>(fp), reinterpret_cast<void*>(arg), &attr)); 
    }
    
    /// create dynamic thread
    /// @param fn thread function
    /// @param stackSize in words (1 word = 4 bytes), default = configMINIMAL_STACK_SIZE
    /// @param prio osPriorityXxx
    /// @param name as null terminated string, default = null
    /// @return thread objecct
    /// @note cannot be called from ISR
    template <typename Fn>
    auto make_thread(Fn&& fn, uint32_t stackSize = configMINIMAL_STACK_SIZE, osPriority_t prio = osPriorityNormal, const char* name = nullptr) {
        osThreadAttr_t attr = {};
        attr.name = name;
        attr.priority = prio;
        attr.stack_size = stackSize * 4;
        auto fp = static_cast<void (*)()>(etl::forward<Fn>(fn));
        return ThreadInterface(osThreadNew(reinterpret_cast<void (*)(void*)>(fp), nullptr, &attr));
    }

    /// return the current running thread object
    /// @note should be called in thread function
    inline auto threadGetCurrent() { return ThreadInterface(osThreadGetId()); }

    /// pass control from current thread to next thread
    /// @return osStatus
    /// @note should be called in thread function
    inline osStatus_t threadYield() { return osThreadYield(); }

    /// terminate execution of current running thread
    /// @note should be called in thread function
    inline void threadExit() { osThreadExit(); }

    /// return number of active threads
    /// @note cannot be called from ISR
    inline uint32_t threadCount() { return osThreadGetCount(); }

    /// return vector of all threads
    /// @note cannot be called from ISR
    inline auto threadGetAll() {     
        class ThreadArray {
            osThreadId_t* ptr;
            uint32_t n;

        public:
            ThreadArray(const ThreadArray&) = delete;
            ThreadArray(ThreadArray&&) = delete;

            ThreadArray& operator=(const ThreadArray&) = delete;
            ThreadArray& operator=(ThreadArray&&) = delete;

            ThreadArray() : ptr(nullptr), n(threadCount()) {
                ptr = new osThreadId_t[n];
                n = osThreadEnumerate(ptr, n);
            }

            ~ThreadArray() { delete[] ptr; }

            uint32_t len() { return n; }

            ThreadInterface* begin() { return reinterpret_cast<ThreadInterface*>(ptr); }

            ThreadInterface* end() { return reinterpret_cast<ThreadInterface*>(ptr + n); }
            
            ThreadInterface operator[](int i) {
                auto l = len();
                if (l == 0) return ThreadInterface(nullptr);
                if (i < 0) i = l + i; // allowing negative index
                return ThreadInterface(ptr[i]);
            }
        };
        return ThreadArray();
    }

    /// reset the specified flags of current running thread
    /// @param flags specifies the flags of the thread that shall be reset
    /// @return current thread's flags before resetting or error code if highest bit set
    /// @note should be called in thread function
    inline uint32_t threadResetFlags(uint32_t flags) { return osThreadFlagsClear(flags); }

    /// get the current flags of the current running thread
    /// @return current thread's flags
    /// @note should be called in thread function
    inline uint32_t threadGetFlags() { return osThreadFlagsGet(); }

    /// wait for flags of the current running thread to become signaled
    /// @param flags specifies the flags to wait for
    /// @param option osFlagsWaitAny (default) or osFlagsWaitAny
    /// @param timeout default = osWaitForever
    /// @param doReset specifies wether reset the flags or not, default = true
    /// @return current thread's flags before resetting or error code if highest bit set
    /// @note should be called in thread function
    inline FlagManager threadWaitFlags(uint32_t flags, uint32_t option = osFlagsWaitAny, uint32_t timeout = osWaitForever, bool doReset = true) { 
        if (!doReset) option |= osFlagsNoClear;
        return osThreadFlagsWait(flags, option, timeout); 
    }

    /// wait for any flags of the current running thread to become signaled
    /// @param timeout default = osWaitForever
    /// @param doReset specifies wether reset the flags or not, default = true
    /// @return current thread's flags before resetting or error code if highest bit set
    /// @note should be called in thread function
    inline FlagManager threadWaitFlagsAny(uint32_t timeout = osWaitForever, bool doReset = true) { 
        uint32_t flags = (1u << 24) - 1; // all possible flags
        uint32_t option = osFlagsWaitAny;
        if (!doReset) option |= osFlagsNoClear;
        return osThreadFlagsWait(flags, option, timeout); 
    }

}

#endif //ETL_THREAD_H
