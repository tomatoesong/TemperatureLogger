/*
    Author: Thomas Song
    December 20205

    Driver written for TMP100
*/
#include "TMP100.h"

#include <Arduino.h>

// Constructor
TMP100::TMP100(uint8_t address, TwoWire *i2c) : _address(address), _i2c(i2c)
{
}

/*
  @return i2c config result status
  @attention User needs to ensure that the I2C object has already called begin()
  and the correct clock speed is set
*/
TMP100::status TMP100::begin()
{
  // _i2c->begin();
  // _i2c->setClock(I2C_FAST_MODE_CLOCK);  //This should be outside of this class and the i2c should be already gone through begin()
  // Configure TMP100 to the default values with 12bit resolution
  if (_i2c == nullptr)
    return status::NULL_POINTER;
  _i2c->beginTransmission(_address);
  _i2c->write(TMP100_CONFIG_REGISTER);
  _i2c->write(TMP100_DEFAULT_CONFIG);
  if (_i2c->endTransmission() != 0)
    return status::I2C_ERROR;
  delay(320); // Wait the typical conversion time for 12bit resolution
  return status::OK;
}

/*
  @return i2c config result status
  @param *i2c for setting the TwoWire object
  @attention User needs to ensure that the I2C object has already called begin()
  and the correct clock speed is set
*/
TMP100::status TMP100::begin(TwoWire *i2c)
{
  _i2c = i2c;
  if (_i2c == nullptr)
    return status::NULL_POINTER;
  _i2c->beginTransmission(_address);
  _i2c->write(TMP100_CONFIG_REGISTER);
  _i2c->write(TMP100_DEFAULT_CONFIG);
  if (_i2c->endTransmission() != 0)
    return status::I2C_ERROR;
  delay(320); // Wait the typical conversion time for 12bit resolution
  return status::OK;
}

/*
  @return status of the sensor reading
  @param tempCelsius reference to write temperature data in Celsius to
  @attention TMP100 is 12bit Q4 encoding with 0.0625C resolution
*/
TMP100::status TMP100::readTempCelsius(float &tempCelsius)
{
  int16_t rawTemp = 0;
  status rstatus = readRawTemp(rawTemp);
  if (status::OK == rstatus)
  {
    tempCelsius = rawTemp * 0.0625f;
  }
  return rstatus;
}

/*
  @return status of the sensor reading
  @param tempFahrenHeit reference to write temperature data in Fahrenheit to
  @attention TMP100 is 12bit Q4 encoding with 0.0625 * 1.8 resolution
*/
TMP100::status TMP100::readTempFahrenheit(float &tempFahrenHeit)
{
  int16_t rawTemp = 0;
  status rstatus = readRawTemp(rawTemp);
  if (status::OK == rstatus)
  {
    tempFahrenHeit = rawTemp * 0.1125f + 32.0f;
  }
  return rstatus;
}

float TMP100::rawToCelsius(int16_t rawTemperature)
{
  return rawTemperature * 0.0625f;
}

float TMP100::rawToFahrenheit(int16_t rawTemperature)
{
  return rawTemperature * 0.1125f + 32.0f;
}

/*
  @return status of the sensor reading
  @param rawTemp reference to write raw temperature data to
*/
TMP100::status TMP100::readRawTemp(int16_t &rawTemp)
{
  uint8_t HighByte;
  uint8_t LowByte;

  _i2c->beginTransmission(_address);
  _i2c->write(TMP100_TEMP_REGISTER);
  if (_i2c->endTransmission(false) != 0)
    return status::I2C_ERROR;

  // //Test values
  // // -25.0C
  // int HB = 0xE7;
  // int LB = 0x00;
  // HighByte = HB;
  // LowByte = LB;
  // //End Test Vlalues

  if (_i2c->requestFrom(_address, (uint8_t)2) != 2)
    return status::I2C_ERROR;
  HighByte = _i2c->read();
  LowByte = _i2c->read();
  rawTemp = ((HighByte << 8) | LowByte) >> 4;
  return status::OK;
}
