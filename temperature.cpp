#include "temperature.h"
#include "logger.h"
#include "const.h"
#include "memory.h"
#include <Arduino.h>

OneWire oneWire(dallasOneWire);
DallasTemperature sensors(&oneWire);

Temperature::Temperature() {
  sensors.begin();
  sensors.setResolution(11);
  #ifndef WAIT_FOR_REQUEST_TEMPERATURE
  sensors.setWaitForConversion(false);
  this->updateSensors();
  #endif
}

void Temperature::setup() {
#ifdef READ_ONE_WIRE_SENSORS_ADDRESS
  char cMsg[40];
  sprintf(cMsg, "[Temperature] Device connected: %d", sensors.getDeviceCount());
  log(cMsg);
  this->readSensorsAddress();
#endif
}

void Temperature::updateSensors() {
    this->previousComputeMillis = millis();
    sensors.requestTemperatures();
}

void Temperature::update() {
  // Update temperature if no "getTemperature" done it yet
  if (millis() - this->previousComputeMillis > 16000) {
    this->updateSensors();
  }
}

float Temperature::getTemperature(DeviceAddress addr) {
  float result = sensors.getTempC(addr);
  if (result < -100) {
    readLogData();
    errorLogData[14]++;
    errorLogData[15] = result;
    writeLogData(errorLogData);
  }
  // If last update is old, call new one
  if (millis() - this->previousComputeMillis > 8000) {
    this->updateSensors();
  }
  return result;
}

#ifdef READ_ONE_WIRE_SENSORS_ADDRESS
void Temperature::readSensorsAddress() {
  DeviceAddress temp;
  for(int i = 0; i < sensors.getDeviceCount(); i++) {
    sensors.getAddress(temp, i);
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