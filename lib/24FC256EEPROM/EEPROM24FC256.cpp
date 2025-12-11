#include "EEPROM24FC256.h"

EEPROM24FC256::EEPROM24FC256(uint8_t address, TwoWire *i2c) : _address(address), _i2c(i2c)
{
}

/*
  @return status
  @brief device config can be set here to start
  @attention User needs to ensure that the I2C object has already called begin()
  and the correct clock speed is set
*/
EEPROM24FC256::status EEPROM24FC256::begin()
{
    //Could write custom config setting initializer here
    if(_i2c == nullptr) return status::NULL_POINTER;
    return status::OK;
}

/*
  @return status
  @brief device config can be set here to start
  @param *i2c for setting the TwoWire object
  @attention User needs to ensure that the I2C object has already called begin()
  and the correct clock speed is set
*/
EEPROM24FC256::status EEPROM24FC256::begin(TwoWire *i2c)
{
    _i2c = i2c;
    if(_i2c == nullptr) return status::NULL_POINTER;
    //Could write custom config setting initializer here
    return status::OK;
}

/*
  @return status of the read
  @param maddress 16bit memory address
  @param outbuff pointer to the read data destination
  @param numOfBytes number of bytes to read
  @attention numbOfBytes need to be within the limits of the platform (ex. 32 Bytes for Arduino Uno)
*/
EEPROM24FC256::status EEPROM24FC256::readBytes(uint16_t maddress, uint8_t *outbuff, size_t numOfBytes)
{
    if (((static_cast<uint32_t>(maddress) + static_cast<uint32_t>(numOfBytes)) >= EEPROM_24FC256_MAX_BYTE) || numOfBytes == 0)
        return status::OUT_OF_BOUNDS;
    uint8_t MS_Byte = maddress >> 8; 
    uint8_t LS_Byte = maddress & 0xFF;
    uint8_t AddressBytes[2] = {MS_Byte, LS_Byte};
    _i2c->beginTransmission(_address);
    _i2c->write(AddressBytes, 2);
    if (_i2c->endTransmission(false) != 0)
        return status::I2C_ERROR;

    if (_i2c->requestFrom(_address, numOfBytes) != numOfBytes)
        return status::I2C_ERROR;
    for (size_t i = 0; i < numOfBytes; i++)
    {
        outbuff[i] = _i2c->read();
    }
    return status::OK;
}

/*
  @return status of the write
  @param maddress 16bit memory address
  @param inbuff pointer to the write data source
  @param numOfBytes number of bytes to write
  @attention This is a page write therefore numOfBytes should be max 64 Bytes and the write must happen 
  within one page
*/
EEPROM24FC256::status EEPROM24FC256::writeBytes(uint16_t maddress, uint8_t *inbuff, size_t numOfBytes)
{
    if (((static_cast<uint32_t>(maddress) + static_cast<uint32_t>(numOfBytes)) >= EEPROM_24FC256_MAX_BYTE) || numOfBytes == 0)
        return status::OUT_OF_BOUNDS;
    uint16_t CurrentPage = maddress / EEPROM_24FC256_PAGE_SIZE;
    uint16_t EndOfDataPage = (maddress + numOfBytes) / EEPROM_24FC256_PAGE_SIZE;
    if (CurrentPage != EndOfDataPage)
        return status::OUT_OF_BOUNDS;
    uint8_t MS_Byte = maddress >> 8; 
    uint8_t LS_Byte = maddress & 0xFF;
    uint8_t Bytes[2 + EEPROM_24FC256_PAGE_SIZE];    //  Fixed buffer size since it is safer especially for different platforms
    Bytes[0] = MS_Byte;
    Bytes[1] = LS_Byte;
    memcpy(&Bytes[2], inbuff, numOfBytes);
    _i2c->beginTransmission(_address);
    _i2c->write(Bytes, numOfBytes + 2);
    if((_i2c->endTransmission()) != 0) return status::I2C_ERROR;
    return status::OK;
}