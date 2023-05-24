#ifndef PROJECT_LCD_H
#define PROJECT_LCD_H

#include "periph/i2c.h"

namespace Project {

    struct LCD {
        inline static constexpr uint32_t timeout = 100;

        enum {
            CLEAR_DISPLAY = 0x01,
            RETURN_HOME = 0x02,
            CURSOR_DECREMENT = 0x04,
            CURSOR_INCREMENT = 0x06,
            SHIFT_DISPLAY_RIGHT = 0x05,
            SHIFT_DISPLAY_LEFT = 0x07,
            DISPLAY_OFF_CURSOR_OFF = 0x08, 
            DISPLAY_OFF_CURSOR_ON = 0x0A, 
            DISPLAY_ON_CURSOR_OFF = 0x0C, 
            DISPLAY_ON_CURSOR_ON = 0x0E, 
            DISPLAY_ON_CURSOR_BLINKING = 0x0F,
            SHIFT_CURSOR_LEFT = 0x10,
            SHIFT_CURSOR_RIGHT = 0x14,
            SHIFT_ENTIRE_DISPLAY_LEFT = 0x18,
            SHIFT_ENTIRE_DISPLAY_RIGHT = 0x1C,
            FORCE_CURSOR_1ST_LINE = 0x80,
            FORCE_CURSOR_2ND_LINE = 0x38,
        };

        periph::I2C& i2c;
        uint8_t slaveAddr;
        uint8_t column = 0, row = 0;

        constexpr explicit LCD(periph::I2C& i2c, uint8_t slaveAddr = 0x4E) : i2c(i2c), slaveAddr(slaveAddr) {}

        void init(); ///< write initial commands and clear the screen

        int writeCmd(uint8_t cmd); ///< write command
        int writeChar(char ch); ///< write data to current column and row

        void writeString(const char* str) {
            writeCmd(0x80);
            for (; *str; str++) writeChar(*str);
        }

        LCD& operator<<(const char* str) { writeString(str); return *this; }

    };
}

#endif