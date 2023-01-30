#include "tank.h"
#include "logger.h"
#include <Arduino.h>

Tank::Tank() {
  
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
    sprintf(cMsg, "[Tank] temperature: %f°C", temperature);
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
    sprintf(cMsg, "[Tank] target temperature: %f°C", temperature);
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