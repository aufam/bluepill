#ifndef PERIPH_RTC_H
#define PERIPH_RTC_H

#include "../../Core/Inc/rtc.h"
#include "etl/time.h"

namespace Project::Periph {

    /// RTC peripheral class
    /// @note requirements: no RTC output
    struct RealTimeClock {
        inline static const char *days[7] = {
                "Sun",
                "Mon",
                "Tue",
                "Wed",
                "Thu",
                "Fri",
                "Sat"
        };

        RTC_TimeTypeDef sTime = {};
        RTC_DateTypeDef sDate = {};
        etl::Time lastUpdate = {};
        constexpr RealTimeClock() = default;

        /// get time and date and store to sTime and sDate
        void update() {
            auto now = etl::Time::now();
            if (now - lastUpdate < 1s) return;

            HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
            lastUpdate = now;
        }

        int setDate(uint8_t week_day, uint8_t date, uint8_t month, uint8_t year) {
            sDate.WeekDay = week_day;
            sDate.Date = date;
            sDate.Month = month;
            sDate.Year = year;
            return HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        }

        int setTime(uint8_t hrs, uint8_t mins, uint8_t secs) {
            sTime.Hours = hrs;
            sTime.Minutes = mins;
            sTime.Seconds = secs;
            return HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        }

        auto getSeconds()  { update(); return sTime.Seconds; }
        auto getMinutes()  { update(); return sTime.Minutes; }
        auto getHours()    { update(); return sTime.Hours; }
        auto getDay()  { update(); return days[sDate.WeekDay]; }
        auto getWeekDay()  { update(); return sDate.WeekDay; }
        auto getDate()     { update(); return sDate.Date; }
        auto getMonth()    { update(); return sDate.Month; }
        auto getYear()     { update(); return sDate.Year; }


    };

    inline RealTimeClock rtc;

} // namespace Project


#endif // PERIPH_RTC_H