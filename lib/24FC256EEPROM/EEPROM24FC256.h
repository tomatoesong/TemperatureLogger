/*
    Author: Thomas Song
    December 20205

    Driver written for 24FC256 EEPROM 
    ====ASSUMPTIONS====
    ===================
    - This driver assumes that there is only one 24FC256 on the same I2C bus
*/
#pragma once

#include <Wire.h>
#include <stdint.h>

// constexpr uint8_t EEPROM_24FC256_DEVICE_ADDRESS = 0x00;
// constexpr uint8_t EEPROM_24FC256_CONTROL_BASE = 0xA0;    // 0b1010XXXX for Control Bytes Base
constexpr uint8_t EEPROM_24FC256_DEVICE_ADDRESS = 0x50;
constexpr uint32_t EEPROM_24FC256_MAX_BYTE = 32UL * 1024UL;  //32KBytes space per device
constexpr uint8_t EEPROM_24FC256_PAGE_SIZE = 64;

//24FC256 Driver
class EEPROM24FC256
{
    public:
    enum class status
    {
        OK,
        I2C_ERROR,
        OUT_OF_BOUNDS,
        NULL_POINTER
    };
    explicit EEPROM24FC256(uint8_t address = EEPROM_24FC256_DEVICE_ADDRESS, TwoWire *i2c = &Wire);
    status begin();
    status begin(TwoWire *i2c);
    status readBytes(uint16_t maddress, uint8_t* outbuff, size_t length);
    status writeBytes(uint16_t maddress, const uint8_t* inbuff, size_t length);

    private:
    uint8_t _address;
    TwoWire *_i2c;
};
