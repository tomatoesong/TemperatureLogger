
#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "TMP100.h"
#include "EEPROM24FC256.h"

struct __attribute__((packed)) DataPack
{
    uint32_t timeStamp = 0;
    int16_t temperature = 0;
    uint16_t checksum;
};

struct __attribute__((packed)) MetaData
{
    uint16_t headAddress = 0;
    uint16_t tailAddress = 0;
    uint16_t checksum;
    uint16_t padding;
};

class DataLogger
{
public:
    enum class status
    {
        OK,
        TMP100_FAIL,
        EEPROM_FAIL,
        NULL_POINTER
    };
    DataLogger(TMP100 *tmp100, EEPROM24FC256 *eeprom24fc256);
    status begin();
    status begin(TMP100 *tmp100, EEPROM24FC256 *eeprom24fc256);

private:
    uint32_t deltaTime;    //For platfrom agnostic behavior 
    TMP100 *_tmp100;
    EEPROM24FC256 *_eeprom24fc256;

    uint16_t CheckSumCalculation();
};