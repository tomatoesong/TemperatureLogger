/*
    Author: Thomas Song
    December 20205

    ====ASSUMPTIONS====
    ===================
    - We are assumming this i2c library is portable across embedded platforms to give the 
      drivers for TMP100 and 24FC256 portability
*/
#include <Arduino.h>
#include "TMP100.h"
#include "EEPROM24FC256.h"
#include "DataLogger.h"

TwoWire i2c1;
TMP100 tmp1;
EEPROM24FC256 ee24fc256;
DataPack dataPack;
MetaData metaData;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  i2c1.begin();
  i2c1.setClock(400000);
  tmp1.begin(&i2c1);
  ee24fc256.begin(&i2c1);

  dataPack.temperature = 0xFF;
  dataPack.checksum = 0xFF;
  dataPack.timeStamp = 0xFFFF;

  metaData.headAddress = 0xFF;
  metaData.tailAddress = 0;
  metaData.padding = 0xFFFFFFFF;
}

void loop() {
  // put your main code here, to run repeatedly:
  // float tempC;
  // tmp1.readTempCelsius(tempC);
  // Serial.println(tempC, 4);
  size_t dataSize = sizeof(dataPack);
  size_t metaDataSize = sizeof(metaData);
  Serial.print("dataSize: ");
  Serial.print(dataSize);
  Serial.print(", metaDataSize: ");
  Serial.println(metaDataSize);
  delay(3000);
}
// static_assert(sizeof(Log) == 6, "Log struct must be 6 bytes!");