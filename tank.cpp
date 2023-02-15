#include "tank.h"
#include "logger.h"
#include "const.h"
#include <Arduino.h>

OneWire oneWire(dallasOneWire);
DallasTemperature sensor(&oneWire);

Tank::Tank() {
  this->previousComputeMillis = millis();
  sensor.begin();
}

void Tank::setMode(int mode) {
  if (this->mode != mode) {
    log("[Tank] New mode: " + mode);
  }
  this->mode = mode;
}

int Tank::getMode() {
  return this->mode;
}

void Tank::setTemperature(float temperature) {
  if (this->temperature != temperature) {
    char cMsg[40];
    sprintf(cMsg, "[Tank] temperature: %fC", temperature);
    log(cMsg);
  }
  this->temperature = temperature;
}

float Tank::getTemperature() {
  return this->temperature;
}

void Tank::setTargetTemperature(float temperature) {
  if (this->targetTemperature != temperature) {
    char cMsg[40];
    sprintf(cMsg, "[Tank] target temperature: %fC", temperature);
    log(cMsg);
  }
  this->targetTemperature = temperature;
}

float Tank::getTargetTemperature() {
  return this->targetTemperature;
}

bool Tank::reachedTargetTemperature() {
  return this->targetTemperature <= this->temperature;
}

void Tank::update() {
  if (millis() - this->previousComputeMillis > 1000) {
    sensor.requestTemperatures();
    float temperatureC = sensor.getTempCByIndex(0);
    this->setTemperature(temperatureC);
    this->previousComputeMillis = millis();
  }
}