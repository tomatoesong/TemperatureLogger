/*
    Author: Thomas Song
    December 20205

    Data logger implmentation for the Aruino Uno. Logs data given a specific period in the DataLogger.h as LOG_PERIOD_MS
*/
#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "TMP100.h"
#include "EEPROM24FC256.h"

struct __attribute__((packed)) DataPack
{
    uint32_t timeStamp = 0;
    int16_t temperature = 0;
    uint16_t checksum = 0;
};

struct __attribute__((packed)) MetaData
{
    uint16_t headAddress = 0;
    uint16_t tailAddress = 0;
    uint16_t padding = 0;
    uint16_t checksum = 0;
};

constexpr uint32_t LOG_PERIOD_MS = 600000; //10 minutes in miliseconds
constexpr size_t DATA_PACK_SIZE = sizeof(DataPack);
constexpr size_t META_DATA_SIZE = sizeof(MetaData);
constexpr uint16_t META_DATA_ADDRESS = EEPROM_24FC256_MAX_BYTE - META_DATA_SIZE;    //  Meta data location at the end of the EEPROM
constexpr uint16_t DATA_START_ADRESS = 0x00;
constexpr size_t MAX_DATA_LENGTH = EEPROM_24FC256_MAX_BYTE - META_DATA_SIZE;    //  Max number of bytes available after taking meta data into account

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
    status putMetaData(MetaData metadata);
    status getMetaData(MetaData &metadata);
    status putDataPack(DataPack datapack);
    status getDataPack(DataPack &datapack, uint16_t address);
    size_t getDataLength();

    status step();

    // Check sum calculator that works for both data structs in this class
    // Sums all bytes excluding the checksum value 
    template <typename T>
    uint16_t CheckSumCalculation(T &data)
    {
        uint16_t tempCheckSum = data.checksum;
        uint16_t checkSum = 0;
        data.checksum = 0;
        const uint8_t *dataPtr = reinterpret_cast<const uint8_t *>(&data);
        for (size_t i = 0; i < sizeof(T); i++)
        {
            checkSum += dataPtr[i];
        }
        data.checksum = tempCheckSum;
        return checkSum;
    }

private:
    uint32_t lastLogTime; // For platfrom agnostic behavior
    TMP100 *_tmp100;
    EEPROM24FC256 *_eeprom24fc256;

    uint16_t _headAddress;
    uint16_t _tailAddress;

    status init();
};