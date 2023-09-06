#include "periph/i2c.h"
#include "main.h"
#define addr    0x68

namespace Project {

    struct DS3231 {
        inline static constexpr uint32_t timeout = 100; //?

        periph::I2C &i2c; //reference to i2c class
        uint8_t buff;
		bool h12 = true; //12H mode for true, and 24H mode for false
		bool PM_time = true; //PM for true, AM for false
		bool century = false; //true if today is 2100 atleast

        constexpr explicit DS3231(periph::I2C &i2c) : i2c(i2c), buff(0){} //constructor

		void init();

        uint8_t bcdtodec(uint8_t num);
        uint8_t dectobcd(uint8_t num);
        uint8_t getSecond();
        uint8_t getMinute();
        uint8_t getHour(); 
        uint8_t getDow();
        uint8_t getDate();
        uint8_t getMonth();
        uint8_t getYear(); //last digits only

		void setClockMode();
		void setSecond(uint8_t Second);
			// In addition to setting the seconds, this clears the
			// "Oscillator Stop Flag".
		void setMinute(uint8_t Minute);
			// Sets the minute
		void setHour(uint8_t Hour);
			// Sets the hour
		void setDoW(uint8_t DoW);
			// Sets the Day of the Week (1-7);
		void setDate(uint8_t Date);
			// Sets the Date of the Month
		void setMonth(uint8_t Month);
			// Sets the Month of the year
		void setYear(uint8_t Year);
			// Last two digits of the year
		void setClockMode(bool h12);
			// Set 12/24h mode. True is 12-h, false is 24-hour.        
    };

};


