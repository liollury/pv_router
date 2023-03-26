#include "tank.h"
#include "logger.h"
#include "const.h"
#include <Arduino.h>


Tank::Tank(Temperature* temperatureSensor) {
  this->previousComputeMillis = millis();
  this->temperatureSensor = temperatureSensor;
}

void Tank::setMode(int mode) {
  if (this->mode != mode) {
    log("[Tank] New mode: " + String(mode));
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
  if (millis() - this->previousComputeMillis > 2000) {
    float temperatureC = this->temperatureSensor->getTemperature(TankOneWireTempSensor);
    if (temperatureC > 0) {
      this->setTemperature(temperatureC);
    }
    this->previousComputeMillis = millis();
  }
}