/*
    Author: Thomas Song
    December 20205

    Driver written for TMP100
*/
#pragma once

#include <Wire.h>
#include <stdint.h>

constexpr uint8_t TMP100_DEVICE_ADDRESS = 0x48;
constexpr uint8_t TMP100_TEMP_REGISTER   = 0x00;
constexpr uint8_t TMP100_CONFIG_REGISTER = 0x01;
constexpr uint8_t TMP100_DEFAULT_CONFIG  = 0x60;
constexpr uint32_t I2C_FAST_MODE_CLOCK = 400000;

// TMP100 Driver
class TMP100
{
public:
  enum class status
  {
    OK,
    I2C_ERROR,
    NULL_POINTER
  };
  explicit TMP100(uint8_t address = TMP100_DEVICE_ADDRESS, TwoWire *i2c = &Wire);
  status begin(TwoWire *i2c);
  status begin();
  status readTempCelsius(float &tempCelsius);
  status readTempFahrenheit(float &tempFahrenHeit);
  status readRawTemp(int16_t &rawTemp);
  float rawToCelsius(int16_t rawTemperature);
  float rawToFahrenheit(int16_t rawTemperature);

private:
  uint8_t _address;
  TwoWire *_i2c;
};