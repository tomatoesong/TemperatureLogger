#include <Arduino.h>
#include "TMP100.h"

TMP100 tmp1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tmp1.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  float tempC;
  tmp1.readTempCelsius(tempC);
  Serial.println(tempC, 4);
  delay(3000);
}