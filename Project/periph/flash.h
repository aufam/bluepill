#ifndef PERIPH_FLASH_H
#define PERIPH_FLASH_H

#include "stm32f1xx_hal.h"
#include "etl/algorithm.h"

namespace Project::periph {

    /// simple EEPROM emulation using flash memory
    /// @tparam T data type
    class Flash {
        template <typename T>
        struct Chunk {
            union { T data; uint32_t buffer[(sizeof (T) + 3) / 4]; };
            uint32_t crc;
        };

    public:
        static constexpr uint32_t pageNumber = 1;   ///< number of page that will be allocated as EEPROM
        static constexpr uint32_t pageOffset = 127; ///< select page offset, default is the last page = 127 (for 128K flash)
        static constexpr uint32_t pageAddress = 0x08000000 + pageOffset * FLASH_PAGE_SIZE;

        /// erase data from selected page address
        /// @retval see @p HAL_StatusTypeDef
        static HAL_StatusTypeDef erase() {
            HAL_FLASH_Unlock();
            uint32_t dummy;
            auto res = HAL_FLASHEx_Erase(&EraseInitStruct, &dummy);
            HAL_FLASH_Lock();
            return res;
        }

        /// write data to selected page address
        /// @retval see @p HAL_StatusTypeDef
        template <typename T>
        static HAL_StatusTypeDef write(const T& data) {
            Chunk<T> chunk = {};

            HAL_FLASH_Unlock();
            uint32_t dummy, address = pageAddress;
            auto res = HAL_FLASHEx_Erase(&EraseInitStruct, &dummy);
            if (res != HAL_OK) goto lock;

            chunk.data = data;
            chunk.crc = calculateCRC(chunk.buffer, &chunk.crc);
            for (auto ptr = chunk.buffer; ptr <= &chunk.crc; ptr++, address += 4) {
                res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *ptr);
                if (res != HAL_OK) goto lock;
            }

            lock:
            HAL_FLASH_Lock();
            return res;
        }

        /// read data from selected page address
        /// @retval HAL_OK: success, HAL_ERROR: failed
        template <typename T>
        static HAL_StatusTypeDef read(T& data) {
            auto eeprom = (Chunk<T>*) pageAddress;
            if (eeprom->crc != calculateCRC(eeprom->buffer, &eeprom->crc)) 
                return HAL_ERROR;
            
            
            memcpy(&data, &eeprom->data, sizeof(T));
            return HAL_OK;
        }
    
    private:
        inline static FLASH_EraseInitTypeDef EraseInitStruct = {
                .TypeErase = FLASH_TYPEERASE_PAGES,
                .Banks = 0,
                .PageAddress = pageAddress,
                .NbPages = pageNumber };
        
        /// calculate crc with xor
        static uint32_t calculateCRC(volatile uint32_t* start, volatile uint32_t* stop) {
            uint32_t res = 0;
            for (auto ptr = start; ptr < stop; ptr++) 
                res ^= *ptr;
            return res;
        }
    };

}

#endif //PERIPH_FLASH_H
