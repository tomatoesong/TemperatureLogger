#pragma once

#include <Wire.h>

#define EEPROM_24FC256_ADRESS 0x00

//24FC256 Driver
class EEPROM24FC256
{
    public:
    enum class status
    {
        OK,
        I2C_ERROR
    };
    explicit EEPROM24FC256(uint8_t address = 0x48, TwoWire *i2c = &Wire);
    status begin();
    status readBytes(uint16_t maddress, uint8_t* outbuff, size_t length);
    status writeBytes(uint16_t maddress, uint8_t* inbuff, size_t length);

    private:
    uint8_t _address;
    TwoWire *_i2c;
};
