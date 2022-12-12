#include "oled/oled.h"
#include <cstring> // memset

namespace Project {

    void Oled::init() {
        i2c.init();
        for (uint8_t i = 0; i < device.initSize; i++) writeCmd(device.initcmds[i]);
        clear();
    }

    void Oled::deinit() {
        i2c.deinit();
    }

    int Oled::writeCmd(uint8_t cmd) const {
        return i2c.writeBlocking(slaveAddr, ID_CMD, &cmd, 1, 100);
    }

    int Oled::writeData(uint8_t *data, uint16_t len) {
        return i2c.writeBlocking(slaveAddr, ID_DATA, data, len, 100);
    }

    int Oled::print(char ch, bool invertColor) {
        if (!font) return -1;
        const uint8_t nChar = fontCharCount();
        const char firstChar = fontFirstChar();
        const char lastChar = firstChar + nChar - 1;
        const uint8_t fontR = fontRows();
        const uint8_t fontH = fontHeight();
        uint8_t fontW = fontWidth();

        switch (ch) {
            case '\r': // carriage return
                clear(column, row,
                      screenWidth() - 1, row + fontR - 1,
                      invertColor); // clear remaining space in this row
                setColumn(0);
                return 0;
            case '\n': // new line
                if (row == screenRows() - fontR) return 0; // last row
                clear(column, row,
                      screenWidth() - 1, row + fontR - 1,
                      invertColor); // clear remaining space in this row
                setCursor(0, row + fontR);
                return 0;
            case '\t': // horizontal tab
                for (int _ = 0; _ < 4; _++) print(' ', invertColor);
                return 0;
            default:
                break;
        }

        if (ch < firstChar || ch > lastChar) return 1;

        const uint8_t c = ch - firstChar; // character offset
        const uint8_t *fontWidthTable = font + FONT_WIDTH_TABLE;
        const uint8_t *fontData = fontWidthTable;
        uint8_t shift = 0;
        if (fontSize() < 2) { // fixed font width
            fontData += c * fontR * fontW;
        }
        else {
            if (fontH % 8) shift = 8 - (fontH % 8);
            int index = 0;
            for (uint8_t i = 0; i < c; i++) {
                index += fontWidthTable[i];
            }
            fontW = fontWidthTable[c];
            fontData += index * fontR + nChar;
        }

        if (fontW + column >= screenWidth()) return -2;

        const uint8_t _col = column;
        const uint8_t _row = row;
        uint8_t bytes[fontW + 1];
        bytes[fontW] = invertColor ? 0xFF : 0; // letter spacing
        for (uint8_t y = 0; y < fontR; y++) {
            if (y > 0) setCursor(_col, row + 1);
            for (uint8_t x = 0; x < fontW; x++) {
                bytes[x] = fontData[(y * fontW) + x];
                if ((y + 1) == fontR) bytes[x] >>= shift; // shift the last row
                if (invertColor) bytes[x] ^= 0xFF;
            }
            writeData(bytes, fontW + 1);
        }

        setCursor(_col + fontW + 1, _row);
        return 0;
    }

    int Oled::print(const char *str, bool invertColor, uint8_t columnStart, uint8_t rowStart) {
        if (columnStart >= screenWidth()) columnStart = column;
        else setColumn(columnStart);
        if (rowStart >= screenRows()) rowStart = row;
        else setRow(rowStart);

        for (auto *ch = str; *ch != '\0'; ch++) {
            auto res = print(*ch, invertColor);
            if (res == -1) return -1; // error
            if (res != -2) continue;
            // column exceeds screen width
            rowStart += fontRows();
            if (rowStart >= screenRows()) break; // rowStart exceeds screen rows
            setCursor(columnStart, rowStart);
            res = print(*ch, invertColor);
            if (res != -2) continue;
            // column still exceeds screen width
            setColumn(0);
            print(*ch, invertColor);
        }
        return 0;
    }

    void Oled::clear(uint8_t columnStart, uint8_t rowStart, uint8_t columnEnd, uint8_t rowEnd, bool invertColor) {
        if (rowEnd >= screenRows()) rowEnd = screenRows() - 1;
        if (columnEnd >= screenWidth()) columnEnd = screenWidth() - 1;
        if (columnStart > columnEnd || rowStart > rowEnd) return; // invalid value

        uint16_t len = columnEnd - columnStart + 1;
        uint8_t data[len];
        int color = invertColor ? 0xFF : 0;
        memset(data, color, len);
        for (auto r = rowStart; r <= rowEnd; r++) {
            setCursor(columnStart, r);
            writeData(data, len);
        }
        setCursor(columnStart, rowStart);
    }

    void Oled::setColumn(uint8_t newColumn) {
        if (newColumn >= screenWidth()) return;
        column = newColumn;
        newColumn += columnOffset();
        writeCmd(SSD1306_SETLOWCOLUMN | (newColumn & 0xF));
        writeCmd(SSD1306_SETHIGHCOLUMN | (newColumn >> 4));
    }

    void Oled::setRow(uint8_t newRow) {
        if (newRow >= screenRows()) return;
        row = newRow;
        writeCmd(SSD1306_SETSTARTPAGE | newRow);
    }

} // namespace Project

