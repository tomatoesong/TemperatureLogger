#include "DataLogger.h"

/*
  @return
  @brief
  @param
  @attention
*/
DataLogger::DataLogger(TMP100 *tmp100, EEPROM24FC256 *eeprom24fc256) : _tmp100(tmp100), _eeprom24fc256(eeprom24fc256)
{
}

DataLogger::status DataLogger::begin()
{
    if (_tmp100 == nullptr || _eeprom24fc256 == nullptr)
        return status::NULL_POINTER;
    init();
    return DataLogger::status::OK;
}

DataLogger::status DataLogger::begin(TMP100 *tmp100, EEPROM24FC256 *eeprom24fc256)
{
    _tmp100 = tmp100;
    _eeprom24fc256 = eeprom24fc256;
    if (_tmp100 == nullptr || _eeprom24fc256 == nullptr)
        return DataLogger::status::NULL_POINTER;
    init();
    return DataLogger::status::OK;
}

/*
  @return   status of meta data wrtie to EEPROM
  @brief    Writes passed MetaData to EEPROM
  @param    metadata MetaData to be written to EEPROM
  @attention    User must ensure that checksum is already calculated before calling putMetaData()
*/
DataLogger::status DataLogger::putMetaData(MetaData metadata)
{
    const uint8_t *mdata = reinterpret_cast<const uint8_t *>(&metadata);
    EEPROM24FC256::status estatus = _eeprom24fc256->writeBytes(META_DATA_ADDRESS, mdata, META_DATA_SIZE);
    if (estatus != EEPROM24FC256::status::OK)
        return DataLogger::status::EEPROM_FAIL;
    return DataLogger::status::OK;
}

/*
  @return   status of meta data read from EEPROM
  @brief    Reads meta data from EEPROM
  @param    MetaData to hold meta data from EEPROM
*/
DataLogger::status DataLogger::getMetaData(MetaData &metadata)
{
    EEPROM24FC256::status estatus = _eeprom24fc256->readBytes(META_DATA_ADDRESS, reinterpret_cast<uint8_t *>(&metadata), META_DATA_SIZE);
    if (estatus != EEPROM24FC256::status::OK)
        return DataLogger::status::EEPROM_FAIL;
    if (metadata.checksum != CheckSumCalculation(metadata))
        return DataLogger::status::EEPROM_FAIL;
    return DataLogger::status::OK;
}

/*
  @return   status of data write to EEPROM
  @brief    Writes datapack to EEPROM in the next available data location. This is implmented with a circular buffer. When number of data exceeds EEPROM capacity
            it will wrap around and overwrite oldest data.
  @param    datapack DataPack struct to write into EEPROM
  @attention    User must ensure that checksum is already calculated before calling putDataPack()
*/
DataLogger::status DataLogger::putDataPack(DataPack datapack)
{
    MetaData metadata;
    const uint8_t *dpack = reinterpret_cast<const uint8_t *>(&datapack);
    getMetaData(metadata);

    uint16_t nextTail = (_tailAddress + DATA_PACK_SIZE) % MAX_DATA_LENGTH;
    EEPROM24FC256::status estatus = _eeprom24fc256->writeBytes(_tailAddress, dpack, DATA_PACK_SIZE);
    if (estatus != EEPROM24FC256::status::OK)
        return DataLogger::status::EEPROM_FAIL;
    metadata.tailAddress = _tailAddress;
    _tailAddress = nextTail;
    if (_tailAddress == _headAddress)
    {
        uint16_t nHead = (_headAddress + DATA_PACK_SIZE) % MAX_DATA_LENGTH;
        metadata.headAddress = _headAddress;
        _headAddress = nHead;
    }
    metadata.checksum = CheckSumCalculation(metadata);
    DataLogger::status dstatus = putMetaData(metadata);
    if (dstatus != DataLogger::status::OK)
        return dstatus;
    return DataLogger::status::OK;
}

/*
  @return   status of DataPack read from EEPROM
  @brief    retreives data pack from EEPROM
  @param    &datapack   reference to DataPack location to read from EEPROM to
  @param    address address of EEPROM to read from
  @attention    Ensure address are valid locations or else returns EEPROM_FAIL status
*/
DataLogger::status DataLogger::getDataPack(DataPack &datapack, uint16_t address)
{
    EEPROM24FC256::status status = _eeprom24fc256->readBytes(address, reinterpret_cast<uint8_t *>(&datapack), DATA_PACK_SIZE);
    if (status != EEPROM24FC256::status::OK)
        return DataLogger::status::EEPROM_FAIL;
    if (datapack.checksum != CheckSumCalculation(datapack))
        return DataLogger::status::EEPROM_FAIL;
    return DataLogger::status::OK;
}

/*
  @return   status of step function
  @brief    this function keeps tract of passed time and logs temperature data from TMP100 in raw int16_t in a struct to the EEPROM after every LOG_PERIOD_MS in the DataLogger.h.

  @attention    This needs to be called as often as possible for the most accurate result
*/
DataLogger::status DataLogger::step()
{
    uint32_t nowTime = millis();
    uint32_t deltaTime = nowTime - lastLogTime;
    if (deltaTime >= LOG_PERIOD_MS)
    {
        DataPack datapack;
        if (_tmp100->readRawTemp(datapack.temperature) != TMP100::status::OK)
            return DataLogger::status::TMP100_FAIL;
        datapack.timeStamp = nowTime;
        datapack.checksum = CheckSumCalculation(datapack);
        if (putDataPack(datapack) != DataLogger::status::OK)
            return DataLogger::status::EEPROM_FAIL;

        lastLogTime = nowTime;
    }
    return DataLogger::status::OK;
}

/*
  @return   number of bytes of data currently written in EEPROM
  @brief    it returns the number of bytes of DataPack in EEPROM
  @attention    this is meant to be used to find how many total data you have saved in EEPROM. To get number of DataPack, simply divide returned value by DATA_PACK_SIZE.
*/
size_t DataLogger::getDataLength()
{
    MetaData metadata;
    if (getMetaData(metadata) != DataLogger::status::OK)
        return 0;
    if (metadata.headAddress > metadata.tailAddress)
        return (MAX_DATA_LENGTH - metadata.headAddress) + (metadata.tailAddress + DATA_PACK_SIZE);
    return (size_t)(metadata.tailAddress - metadata.headAddress + DATA_PACK_SIZE);
}

/*
  @return   status of function
  @brief    Used in begin to initialize all necessary data to start logging and starts by noting one temperature data to EEPROM
*/
DataLogger::status DataLogger::init()
{
    // populate metadata and write to eeprom
    MetaData metadata;
    _headAddress = DATA_START_ADRESS;
    _tailAddress = DATA_START_ADRESS;
    metadata.headAddress = DATA_START_ADRESS;
    metadata.tailAddress = DATA_START_ADRESS;
    metadata.checksum = CheckSumCalculation(metadata);
    if (putMetaData(metadata) != DataLogger::status::OK)
        return DataLogger::status::EEPROM_FAIL;
    // save first temperature data to eeprom
    DataPack datapack;
    if (_tmp100->readRawTemp(datapack.temperature) != TMP100::status::OK)
        return DataLogger::status::TMP100_FAIL;
    lastLogTime = millis();
    datapack.timeStamp = lastLogTime;
    datapack.checksum = CheckSumCalculation(datapack);
    putDataPack(datapack);
    return DataLogger::status::OK;
}
