#ifndef WTP_BLUEPILL_EEPROM_H
#define WTP_BLUEPILL_EEPROM_H

#include "periph/i2c.h"
#include <cmath>

namespace Project {

    /// EEPROM external using I2C bus
    /// @tparam T data type
    template <class T>
    struct EEPROM {
        using I2C = Periph::I2C;

        static constexpr uint16_t pageSize = 64;
        static constexpr uint16_t defaultEEPROMAddress = 0xA0;

        I2C& i2c;
        uint16_t eepromAddress;
        uint16_t address;
        union { T data; uint32_t buffer[(sizeof (T) + 3) / 4]; };

        constexpr EEPROM(uint16_t addr, I2C& i2c, uint16_t eepromAddress = defaultEEPROMAddress)
        : i2c(i2c)
        , eepromAddress(eepromAddress)
        , address(addr)
        , data{} {
            assert_param(pageSize - (addr % pageSize) >= sizeof (T)); /// Data size is too big
        }

        /// write data to selected address
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int write() {
            return i2c.writeBlocking(eepromAddress, address, (uint8_t *) &data, sizeof (T));
        }

        int read() {
            return i2c.readBlocking(eepromAddress, address, (uint8_t *) &data, sizeof (T));
        }

        EEPROM& operator = (const T& other) {
            data = other;
            write();
            return *this;
        }

    };


}

#endif //WTP_BLUEPILL_EEPROM_H
