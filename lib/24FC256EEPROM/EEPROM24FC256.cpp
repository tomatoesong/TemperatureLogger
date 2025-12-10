#include "EEPROM24FC256.h"

EEPROM24FC256::EEPROM24FC256(uint8_t address = 0x48, TwoWire *i2c = &Wire) : _address(address), _i2c(i2c)
{
}

EEPROM24FC256::status EEPROM24FC256::begin()
{
}

EEPROM24FC256::status EEPROM24FC256::readBytes(uint16_t maddress, uint8_t* out, size_t length)
{
}

EEPROM24FC256::status EEPROM24FC256::writeBytes(uint16_t maddress, uint8_t* inbuff, size_t length)
{
}