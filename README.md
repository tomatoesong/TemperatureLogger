**TemperatureLogger** is a simple embedded project for the Arduino UNO that reads temperature data from a **TMP100** sensor and logs it to an **24FC256 EEPROM**. The repository includes the main firmware and supporting libraries.

---
## Features

- Reads temperature data using the TMP100 sensor
- Logs timestamped temperature readings to external EEPROM
- Includes custom libraries for TMP100 and EEPROM

---

## General
### Assumptions:
* For the I2C communication to the peripherals I am assuming the platform has an implementation of an I2C library similar to the Wire library Arduino has. 
* The step() function in DataLogger class needs to be called as often as possible to keep the period between recording as close as possible to 10 minutes. This could be also implemented using a timer and an ISR however, it would not be as portable.
* It is assumed that the user calls begin on the i2c object and set the correct clock speed (the lowest of all devices on the bus) before the TMP100 and 24FC256 calls their respective begin() function.
* User calls the begin() function on I2C, TMP100, and 24F256 before DataLogger begin is called
### Limitations
* I utilized the millis() function implemented in the Arduino library to keep track of time. This is not a greatly accurate way to keep track of time as it is not based off of a RTC in most embedded platforms. This may not be a huge issue if the only thing that matters is the time delta of 10 minutes. This code base can easily adapt to an RTC timer by using it to implement the same function as millis().

---

## DataLogger
### Description
This is the core of the data logging system that implements a circular memory addressing for continuous recording. Data containing time stamp, raw temperature, and check sum can be stored into this circular memory. Meta data is added to the end of the EEPROM address that keeps track of beginning and end of the valid data.

### Assumptions
* The user will use valid memory addresses in EEPROM for reading and writing data and meta data.
* To retrieve data the user understands that they need to create memory space in bytes equal or larger than the size returned by the getDataLength() in DataLogger.
* The returned status must be at the users discretion.
### Limitations
* The check sum performed to verify data integrity is not as robust as implementing a CRC

---

## EEPROM
### Description
Set of drivers that allows reading and writing to and from a buffer. All functions return a status to verify if the operation was successful.

### Assumptions
* I am assuming that there is only one 24FC256 attached to the device. Even though it is possible to implement up to 8 devices on the same line using the address pins, due to the limited time I chose to assume one device.
* The data will be retrieved more often than every 28.4 days as the data will overwrite the oldest data when it reaches the max capacity of the EEPROM.
* All accesses are happening exactly aligned with the pages. For the data logging usage all data written are 8 bytes which perfectly fits into the EEPROM without spanning across multiple pages. A write across multiple pages could be implemented by calculating the page address of the write location and verify if the complete write would span across to another page and split the data into multiple sets to be written in pages.
* We assume the user is aware of their i2c buffer limitation for their platform and writes and reads only within that limit.
* The user always provides enough memory space for readBytes and the correct size matching or less than the buffer size for writeBytes.
### Limitations
* The writeBytes function writes in pages which are put in a buffer in the 24FC256 and written to memory later. This may take up to 5 ms which limits how often you can write to the EEPROM.
* The data will be retrieved more often than every 28.4 days as the data will overwrite the oldest data when it reaches the max capacity of the EEPROM

---
## TMP100
### Description
Simple drivers that can return temperature data from TMP100 in one of three data formats: raw signed 16 bit, celsius and fahrenheit as floats. You can also convert the raw temperature data by using rawToCelsius or rawToFahrenheit.

### Assumption
* It is assumed we want maximum accuracy from the sensor with 12 bit precision and do not poll the sensor more often then every 320ms

## TESTING
Due to the lack of physical devices to perform testing only rudimentary value injection to some functions were tested. Without having written an I2C simulation that works like a TwoWire Object with injected data
it is hard to do any integration testing. With the time contraint and type of exercise this project is it was impractical to create a test suite. If I had access to more resources and time I would prefer to write an automated test setup with GoogleTests. This would require a fake a fake EEPROM that responds with specified values.

#### TMP100
* Tested with provided values from data sheet of MSB: 0xE7 and LSB: 0x00 which corresponds to -25.0000 C.
* All functions returned the correct temperature and raw values 

#### 24FC256
* Check sum calculation verified using injected values and a simple test. Found in checksumTest()




