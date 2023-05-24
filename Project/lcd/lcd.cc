#include "lcd/lcd.h"
#include "cmsis_os2.h"

using namespace Project;

void LCD::init() {
	// 4 bit initialisation
	osDelay(50);  // wait for >40ms
	writeCmd(0x30);
	osDelay(5);  // wait for >4.1ms
	writeCmd(0x30);
	osDelay(1);  // wait for >100us
	writeCmd(0x30);
	osDelay(10);
	writeCmd(0x20);  // 4bit mode
	osDelay(10);

    // dislay initialisation
	writeCmd(0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	osDelay(1);
	writeCmd(0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	osDelay(1);
	writeCmd(0x01);  // clear display
	osDelay(1);
	writeCmd(0x02);  // clear display
	osDelay(1);
	writeCmd(0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	osDelay(1);
	writeCmd(0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

int LCD::writeCmd(uint8_t cmd) {
	uint8_t upper = cmd & 0xf0;
	uint8_t lower = (cmd << 4) & 0xf0;
	uint8_t data[4];
	data[0] = upper | 0x0C;  //en=1, rs=0
	data[1] = upper | 0x08;  //en=0, rs=0
	data[2] = lower | 0x0C;  //en=1, rs=0
	data[3] = lower | 0x08;  //en=0, rs=0
	return HAL_I2C_Master_Transmit(&i2c.hi2c, slaveAddr, data, 4, timeout);
}

int LCD::writeChar(char ch) {
	uint8_t upper = ch & 0xf0;
	uint8_t lower = (ch << 4) & 0xf0;
	uint8_t data[4];
	data[0] = upper | 0x0C;  //en=1, rs=0
	data[1] = upper | 0x09;  //en=0, rs=0
	data[2] = lower | 0x0D;  //en=1, rs=0
	data[3] = lower | 0x09;  //en=0, rs=0
	return HAL_I2C_Master_Transmit(&i2c.hi2c, slaveAddr, data, 4, timeout);
}