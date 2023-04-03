#include "tank.h"
#include "logger.h"
#include "const.h"
#include <Arduino.h>

#define TANK_SEMAPHORE_BLOCK_DELAY 100 / portTICK_PERIOD_MS

static SemaphoreHandle_t tankMutex = xSemaphoreCreateMutex();

Tank::Tank(Temperature* temperatureSensor) {
  this->previousComputeMillis = millis();
  this->temperatureSensor = temperatureSensor;
}

void Tank::setMode(int mode) {
  xSemaphoreTake(tankMutex, TANK_SEMAPHORE_BLOCK_DELAY);
  if (this->mode != mode) {
    log("[Tank] New mode: " + String(mode));
  }
  this->mode = mode;
  xSemaphoreGive(tankMutex);
}

int Tank::getMode() {
  int mode = TANK_MODE_AUTO_OFF;
  xSemaphoreTake(tankMutex, TANK_SEMAPHORE_BLOCK_DELAY);
  mode = this->mode;
  xSemaphoreGive(tankMutex);
  return mode;
}

void Tank::setTemperature(float temperature) {
  xSemaphoreTake(tankMutex, TANK_SEMAPHORE_BLOCK_DELAY);
  if (this->temperature != temperature) {
    char cMsg[40];
    sprintf(cMsg, "[Tank] temperature: %fC", temperature);
    log(cMsg);
  }
  this->temperature = temperature;
  xSemaphoreGive(tankMutex);
}

float Tank::getTemperature() {
  float temperature = 999;
  xSemaphoreTake(tankMutex, TANK_SEMAPHORE_BLOCK_DELAY);
  temperature = this->temperature;
  xSemaphoreGive(tankMutex);
  return temperature;
}

void Tank::setTargetTemperature(float temperature) {
  xSemaphoreTake(tankMutex, TANK_SEMAPHORE_BLOCK_DELAY);
  if (this->targetTemperature != temperature) {
    char cMsg[40];
    sprintf(cMsg, "[Tank] target temperature: %fC", temperature);
    log(cMsg);
  }
  this->targetTemperature = temperature;
  xSemaphoreGive(tankMutex);
}

float Tank::getTargetTemperature() {
  float targetTemperature = 0;
  xSemaphoreTake(tankMutex, TANK_SEMAPHORE_BLOCK_DELAY);
  temperature = this->targetTemperature;
  xSemaphoreGive(tankMutex);
  return targetTemperature;
}

bool Tank::reachedTargetTemperature() {
  return this->getTargetTemperature() <= this->getTemperature();
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