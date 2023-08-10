#ifndef PERIPH_FLASH_H
#define PERIPH_FLASH_H

#include "stm32f1xx_hal.h"

namespace Project::periph {

    /// simple EEPROM emulation using flash memory
    /// @tparam T data type
    template <class T>
    struct Flash {
        static constexpr uint32_t pageNumber = 1; /// number of page that will be allocated as EEPROM
        static constexpr uint32_t pageOffset = 127; /// select page offset, default last page = 127 (for 128K flash)
        static constexpr uint32_t pageAddress = 0x08000000 + pageOffset * FLASH_PAGE_SIZE;
        inline static FLASH_EraseInitTypeDef EraseInitStruct = {
                .TypeErase = FLASH_TYPEERASE_PAGES,
                .Banks = 0,
                .PageAddress = pageAddress,
                .NbPages = pageNumber };

        union { T data; uint32_t buffer[(sizeof (T) + 3) / 4]; };
        uint32_t crc;
        constexpr Flash() : data{}, crc{} {}
        constexpr explicit Flash(const T& data) : data(data), crc{} {}

        /// erase data from selected page address
        /// @retval see @p HAL_StatusTypeDef
        HAL_StatusTypeDef erase() {
            HAL_FLASH_Unlock();
            uint32_t dummy;
            auto res = HAL_FLASHEx_Erase(&EraseInitStruct, &dummy);
            HAL_FLASH_Lock();
            return res;
        }

        /// write data to selected page address
        /// @retval see @p HAL_StatusTypeDef
        HAL_StatusTypeDef write() {
            HAL_FLASH_Unlock();
            uint32_t dummy, address = pageAddress;
            auto res = HAL_FLASHEx_Erase(&EraseInitStruct, &dummy);
            if (res != HAL_OK) goto lock;

            crc = calculateCRC();
            for (auto ptr = (uint32_t *) this; ptr <= &crc; ptr++, address += 4) {
                res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *ptr);
                if (res != HAL_OK) goto lock;
            }

            lock:
            HAL_FLASH_Lock();
            return res;
        }

        /// read data from selected page address
        /// @retval HAL_OK: success, HAL_ERROR: failed
        HAL_StatusTypeDef read() {
            auto eeprom = Flash<T> {};
            auto address = pageAddress;
            for (auto ptr = (uint32_t *) &eeprom; ptr <= &eeprom.crc; ptr++, address += 4)
                *ptr = *(__IO uint32_t *) address;

            if (eeprom.crc != eeprom.calculateCRC()) return HAL_ERROR;
            *this = eeprom;
            return HAL_OK;
        }

        /// calculate crc with xor
        [[nodiscard]] uint32_t calculateCRC() const {
            uint32_t res = 0;
            for (auto ptr = (uint32_t *) this; ptr < &crc; ptr++) res ^= *ptr;
            return res;
        }

        Flash& operator = (const T& other) {
            data = other;
            write();
            return *this;
        }
    };

}

#endif //PERIPH_FLASH_H
