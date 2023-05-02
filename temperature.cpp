#include "temperature.h"
#include "logger.h"
#include "const.h"
#include "memory.h"
#include <Arduino.h>

int busCount = 2;
OneWire oneWireB1(dallasOneWireBus1);
DallasTemperature sensorsB1(&oneWireB1);
OneWire oneWireB2(dallasOneWireBus2);
DallasTemperature sensorsB2(&oneWireB2);

DallasTemperature sensorsBus[] = {sensorsB1, sensorsB2};
static SemaphoreHandle_t temperatureUpdateMutex = xSemaphoreCreateMutex();

Temperature::Temperature() {
  for(int bus = 0; bus < busCount; bus++) { 
    sensorsBus[bus].begin();
    sensorsBus[bus].setResolution(ResolutionOnDallasBus[bus]);
    #ifndef WAIT_FOR_REQUEST_TEMPERATURE
    sensorsBus[bus].setWaitForConversion(false);
    #endif
  }
  #ifndef WAIT_FOR_REQUEST_TEMPERATURE
  this->updateSensors();
  #endif
}

void Temperature::setup() {
#ifdef READ_ONE_WIRE_SENSORS_ADDRESS
  char cMsg[50];
  for(int bus = 0; bus < busCount; bus++) { 
    sprintf(cMsg, "[Temperature] Device connected on bus %d: %d", bus + 1, sensorsBus[bus].getDeviceCount());
    log(cMsg);
  }
  this->readSensorsAddress();
#endif
}

void Temperature::updateSensors() {
  this->previousComputeMillis = millis();
  for(int bus = 0; bus < busCount; bus++) { 
    xSemaphoreTake(temperatureUpdateMutex, portMAX_DELAY);
    sensorsBus[bus].requestTemperatures();
    xSemaphoreGive(temperatureUpdateMutex);
  }
}

void Temperature::update() {
  // Update temperature if no "getTemperature" done it yet
  if (millis() - this->previousComputeMillis > 16000) {
    this->updateSensors();
  }
}

float Temperature::getTemperature(DeviceAddress addr, int bus, float fallbackTemperature, float failTemperature) {
  xSemaphoreTake(temperatureUpdateMutex, portMAX_DELAY);
  float result = sensorsBus[bus - 1].getTempC(addr);
  xSemaphoreGive(temperatureUpdateMutex);
  if (result < -100) {
    this->failCount++;
    if (this->failCount >= ACCEPTABLE_TEMPERATURE_READING_FAIL) {
      readLogData();
      errorLogData[14 + bus - 1]++;
      errorLogData[16] = result;
      writeLogData(errorLogData);
      result = failTemperature;
    } else {
      result = fallbackTemperature;
    }
  } else {
    this->failCount = 0;
  }
  // If last update is old, call new one
  if (millis() - this->previousComputeMillis > 9000 && xPortGetCoreID() == ACCESSORY_CORE) {
    this->updateSensors();
  }
  return result;
}

#ifdef READ_ONE_WIRE_SENSORS_ADDRESS
void Temperature::readSensorsAddress() {
  DeviceAddress temp;
  for(int bus = 0; bus < busCount; bus++) {
    for(int i = 0; i < sensorsBus[bus].getDeviceCount(); i++) {
      sensorsBus[bus].getAddress(temp, i);
      Serial.printf("Sensor %d on bus %d : ", i + 1, bus + 1);
      for (uint8_t e = 0; e < 8; e++) {
        Serial.print("0x");
        if (temp[e] < 0x10) Serial.print("0");
        Serial.print(temp[e], HEX);
        Serial.print(", ");
      }
    Serial.println();
    }
  }
}
#endif