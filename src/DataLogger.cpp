
#include "DataLogger.h"

DataLogger::DataLogger(TMP100 *tmp100, EEPROM24FC256 *eeprom24fc256) : _tmp100(tmp100), _eeprom24fc256(eeprom24fc256)
{
}

DataLogger::status DataLogger::begin()
{
    if(_tmp100 == nullptr || _eeprom24fc256 == nullptr) return status::NULL_POINTER;
    deltaTime = millis();
    return status::OK;
}

DataLogger::status DataLogger::begin(TMP100 *tmp100, EEPROM24FC256 *eeprom24fc256)
{
    _tmp100 = tmp100;
    _eeprom24fc256 = eeprom24fc256;
    if(_tmp100 == nullptr || _eeprom24fc256 == nullptr) return status::NULL_POINTER;
    deltaTime = millis();
    return status::OK;
}

uint16_t DataLogger::CheckSumCalculation()
{
    return 0;
}
