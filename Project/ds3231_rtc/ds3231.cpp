#include "ds3231_rtc/ds3231.h"

using namespace Project;

//Conversion Function
uint8_t DS3231::bcdtodec(uint8_t num){  
    return ((num/16*10) + (num%16));
}

uint8_t DS3231::dectobcd(uint8_t num){ 
    return ( (num/10*16) + (num%10) );
}
///////////////////////////////////////////////

//Get Time Function
uint8_t DS3231::getSecond(){
    i2c.readBlocking({addr<<1, 0x00, &buff, 1});
    return bcdtodec(buff);
}

uint8_t DS3231::getMinute(){
    i2c.readBlocking({addr<<1, 0x01, &buff, 1});
    return buff;
}

uint8_t DS3231::getHour(){
    i2c.readBlocking({addr<<1, 0x02, &buff, 1});

    h12 = buff & 0b01000000; //bit ke 6 adalah bit 12/24' mode,
    if(h12){
        PM_time = buff & 0b01000000;
        return bcdtodec(buff & 0b00011111); //bit 0 - bit 4 adalah BCD Value Hour
    } else {
         return bcdtodec(buff & 0b00111111); 
    }
}

uint8_t DS3231::getDow(){
    i2c.readBlocking({addr<<1, 0x03, &buff, 1});
    return bcdtodec(buff);
}

uint8_t DS3231::getDate(){
    i2c.readBlocking({addr<<1, 0x04, &buff, 1});
    return buff;
}

uint8_t DS3231::getMonth(){
    i2c.readBlocking({addr<<1, 0x05, &buff, 1});
    century = buff & 0b10000000;
    return bcdtodec(buff & 0b01111111);
}

uint8_t DS3231::getYear(){
    i2c.readBlocking({addr<<1, 0x06, &buff, 1});
    return bcdtodec(buff);
}
////////////////////////////////////////////////////////////////


