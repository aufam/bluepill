#ifndef ETL_TIME_H
#define ETL_TIME_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

namespace Project::etl {

    struct Time {
        static constexpr auto tickFreq = configTICK_RATE_HZ;
        uint32_t tick;
        constexpr explicit Time(uint32_t tick) : tick(tick) {}
        Time() : tick(osKernelGetTickCount()) {}

        static constexpr Time ms2time(uint32_t val)   { return Time { (val * tickFreq + 999ul) / 1000ul }; }
        static constexpr Time s2time(uint32_t val)    { return Time { (val * tickFreq) }; }
        static constexpr Time min2time(uint32_t val)  { return Time { (val * tickFreq * 60) }; }

        static Time now() { return {}; }
        static Time elapsedSince(Time time) { return Time() - time; }

        [[nodiscard]] constexpr float ms()  const { return (float) tick * 1e3f / tickFreq; }
        [[nodiscard]] constexpr float s()   const { return (float) tick / tickFreq; }
        [[nodiscard]] constexpr float min() const { return (float) tick / tickFreq / 60; }

        constexpr explicit operator float() const { return (float) tick; }
        
        constexpr Time operator-(Time other) const { return Time { tick - other.tick }; }
        constexpr Time operator+(Time other) const { return Time { tick + other.tick }; }

        constexpr float operator/(Time other)  const { return (float) tick / (float) other.tick; }
        constexpr Time operator /(int other)   const { return Time { tick / other }; }
        constexpr Time operator *(float other) const { return Time { uint32_t ((float) tick * other) }; }

        constexpr Time& operator-=(Time other) { tick -= other.tick; return *this; }
        constexpr Time& operator+=(Time other) { tick += other.tick; return *this; }

        constexpr bool operator==(Time other) const { return tick == other.tick; }
        constexpr bool operator!=(Time other) const { return tick != other.tick; }
        constexpr bool operator>=(Time other) const { return tick >= other.tick; }
        constexpr bool operator<=(Time other) const { return tick <= other.tick; }
        constexpr bool operator >(Time other) const { return tick > other.tick; }
        constexpr bool operator <(Time other) const { return tick < other.tick; }
    };

    inline static constexpr Time timeInfinite(osWaitForever);
    inline static constexpr Time timeImmediate(0);

    inline void sleep(Time time) { osDelay(time.tick); }
}

namespace Project::etl::literals {
    constexpr auto operator ""ms   (unsigned long long val) { return Time::ms2time(val); }
    constexpr auto operator ""s    (unsigned long long val) { return Time::s2time(val); }
    constexpr auto operator ""min  (unsigned long long val) { return Time::min2time(val); }
}

#endif //ETL_TIME_H
