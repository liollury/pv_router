#include "temperature.h"
#include "logger.h"
#include "const.h"
#include <Arduino.h>

OneWire oneWire(dallasOneWire);
DallasTemperature sensor(&oneWire);

Temperature::Temperature() {
  sensor.begin();
  sensor.setWaitForConversion(false);
  sensor.requestTemperatures();
}

void Temperature::setup() {
#ifdef READ_ONE_WIRE_SENSORS_ADDRESS
  char cMsg[40];
  sprintf(cMsg, "[Temperature] Device connected: %d", sensor.getDeviceCount());
  log(cMsg);
  this->readSensorsAddress();
#endif
}

void Temperature::update() {
  if (millis() - this->previousComputeMillis > 2000) {
    this->previousComputeMillis = millis();
    sensor.requestTemperatures();
  }
}

float Temperature::getTemperature(DeviceAddress addr) {
  return sensor.getTempC(addr);
}

#ifdef READ_ONE_WIRE_SENSORS_ADDRESS
void Temperature::readSensorsAddress() {
  DeviceAddress temp;
  for(int i = 0; i < sensor.getDeviceCount(); i++) {
    sensor.getAddress(temp, i);
    Serial.printf("Sensor %d : ", i + 1);
    for (uint8_t e = 0; e < 8; e++) {
      Serial.print("0x");
      if (temp[e] < 0x10) Serial.print("0");
      Serial.print(temp[e], HEX);
      Serial.print(", ");
    }
  Serial.println();
  }
}
#endif