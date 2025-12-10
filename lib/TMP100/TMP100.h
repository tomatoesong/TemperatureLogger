#pragma once

#include <Wire.h>

#define TMP100_TEMP_REGISTER 0x00
#define TMP100_CONFIG_REGISTER 0x01
#define TMP100_DEFAULT_CONFIG 0x60
#define I2C_FAST_MODE_CLOCK 400000

//TMP100 Driver
class TMP100 {
public:
  enum class status { OK,
                      I2C_ERROR };
  explicit TMP100(uint8_t address = 0x48, TwoWire* i2c = &Wire);
  status begin();
  status readTempCelsius(float& tempCelsius);
  status readTempFahrenheit(float& tempFahrenHeit);

private:
  uint8_t _address;
  TwoWire* _i2c;

  status readRawTemp(uint16_t& rawTemp);
};