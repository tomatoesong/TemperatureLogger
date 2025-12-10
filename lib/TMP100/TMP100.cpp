#include "TMP100.h"

#include <Arduino.h>

// Constructor
TMP100::TMP100(uint8_t address, TwoWire* i2c) : _address(address), _i2c(i2c){
}

TMP100::status TMP100::begin(){
  _i2c->begin();
  _i2c->setClock(I2C_FAST_MODE_CLOCK);  //This should be outside of this class and the i2c should be already gone through begin()

  // Configure TMP100 to the default values with 12bit resolution
  _i2c->beginTransmission(_address);
  _i2c->write(TMP100_CONFIG_REGISTER);
  _i2c->write(TMP100_DEFAULT_CONFIG);
  _i2c->endTransmission();

  return status::OK;
}

/*
  @RETURN status of the sensor reading
  @PARAM: tempCelsius = reference to write temperature data in Celsius to
  @NOTE: TMP100 is 12bit Q4 encoding with 0.0625C resolution
*/
TMP100::status TMP100::readTempCelsius(float& tempCelsius){
  uint16_t rawTemp = 0;
  status rstatus = readRawTemp(rawTemp);
  if (status::OK == rstatus){
    tempCelsius = ((int16_t)rawTemp) * 0.0625f; 
  }
  return rstatus;
}

/*
  @RETURN status of the sensor reading 
  @PARAM: tempFahrenHeit = reference to write temperature data in Fahrenheit to
  @NOTE: TMP100 is 12bit Q4 encoding with 0.0625 * 1.8 resolution
*/
TMP100::status TMP100::readTempFahrenheit(float& tempFahrenHeit){
  uint16_t rawTemp = 0;
  status rstatus = readRawTemp(rawTemp);
  if (status::OK == rstatus){
    tempFahrenHeit = ((int16_t)rawTemp) * 0.1125f + 32.0f;
  }
  return rstatus;
}

/*
  @RETURN status of the sensor reading 
  @PARAM: rawTemp = reference to write raw temperature data to
*/
TMP100::status TMP100::readRawTemp(uint16_t& rawTemp){
  uint8_t HighByte;
  uint8_t LowByte;

  _i2c->beginTransmission(_address);
  _i2c->write(TMP100_TEMP_REGISTER);
  _i2c->endTransmission();

  // //Test values
  // // -25.0C
  // int HB = 0xE7;
  // int LB = 0x00;
  // HighByte = HB;
  // LowByte = LB;
  // //End Test Vlalues

  if (_i2c->requestFrom(_address, (uint8_t)2) != 2) return status::I2C_ERROR;
  HighByte = _i2c->read();
  LowByte = _i2c->read();
  rawTemp = ((HighByte << 8) | LowByte) >> 4;
  return status::OK;
}

