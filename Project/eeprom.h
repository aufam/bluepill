#ifndef PROJECT_EEPROM_H
#define PROJECT_EEPROM_H

#include "stm32f1xx_hal.h"

namespace Project {

    struct EEPROM {
        uint32_t address;
        static const uint32_t startAddress = 0x0801F840;
        constexpr explicit EEPROM(uint32_t address) : address(address) {}

        uint32_t write(uint32_t *data, uint16_t nWords) const {
            return write(address, data, nWords);
        }
        void read(uint32_t *data, uint16_t nWords) const {
            read(address, data, nWords);
        }

        static uint32_t write(uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);
        static void read(uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);
    };

}

#endif //PROJECT_EEPROM_H
