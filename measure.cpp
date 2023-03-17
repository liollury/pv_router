#include "const.h"
#include "measure.h"
#include "logger.h"
#include <Arduino.h>

Measure* instance;

void IRAM_ATTR zeroCrossInterruptHandler() {
  instance->zeroCrossInterrupt();
}

void IRAM_ATTR onTimerInterruptHandler() {
  instance->onTimerInterrupt();
}

Measure::Measure(Tank* tank) {
  instance = this;
  this->tank = tank;
  this->previousComputeMillis = millis();
  pinMode(ZeroCross, INPUT);
  pinMode(PulseTriac, OUTPUT);
  digitalWrite(PulseTriac, LOW);

  for (int i = 0; i < 100; i++) {  //Reset table measurements
    this->voltM[i] = 0;
    this->ampM[i] = 0;
  }

  attachInterrupt(ZeroCross, zeroCrossInterruptHandler, RISING);
  this->timer = timerBegin(0, 80, true);  //Clock Divider, 1 micro second Tick
  timerAttachInterrupt(this->timer, onTimerInterruptHandler, true);
  timerAlarmWrite(this->timer, 100, true);  //Interrupt every 100 Ticks or microsecond
  timerAlarmEnable(this->timer);
}

void IRAM_ATTR Measure::zeroCrossInterrupt() {
  if ((millis() - this->lastZeroCrossInterruption) > 2) {  // to avoid glitch detection during 2ms
    this->currentTriacPosition = 0; //Time synchro every 10ms
    this->lastZeroCrossInterruption = millis();
    digitalWrite(PulseTriac, LOW);  //Stop Triac
    this->phasePositive = !this->phasePositive;
    if (this->phasePositive) {
      this->zeroCrossingMs = (micros() % 20000) / 200;  // To synchronise the voltage and curent short integration and the curves to display
    }
  }
}

void IRAM_ATTR Measure::onTimerInterrupt() {  //Interruption every 100 micro second
  this->currentTriacPosition += 1;
  if (this->currentTriacPosition > this->triacDelay && this->triacDelay < 98) {  //100 steps in 10 ms
    digitalWrite(PulseTriac, HIGH); //Activate Triac
  }
}

void Measure::measurePower() {
  int index;
  this->refVolt = analogRead(AnalogRef);
  unsigned long MeasureMillis = millis();
  /*Serial.print(this->refVolt);
  Serial.print('-');
  Serial.println(analogRead(AnalogAmp));*/

  while (millis() - MeasureMillis < 21) {                             //Read values in continuous during 20ms. One loop is around 150 micro seconds
    index = ((micros() % 20000) / 200 + 100 - zeroCrossingMs) % 100;  //We have more results that we need during 20ms to fill the tables of 100 samples
    this->volt[index] = analogRead(AnalogVolt) - refVolt;
    this->amp[index] = analogRead(AnalogAmp) - refVolt;
  }
}


void Measure::computePower() {
  float voltage;
  float rmsVoltage = 0;
  float current;
  float rmsCurrent = 0;
  this->pW = 0;

  for (int i = 0; i < 100; i++) {
    voltM[i] = (19 * voltM[i] + float(volt[i])) / 20;  //Mean value. First Order Filter. Short Integration
    voltage = kV * voltM[i];
    rmsVoltage += sq(voltage);
    ampM[i] = (19 * ampM[i] + float(amp[i])) / 20;  //Mean value. First Order Filter
    current = kI * ampM[i];
    rmsCurrent += sq(current);
    this->pW += voltage * current;
  }
  rmsVoltage = sqrt(rmsVoltage / 100);  //RMS voltage
  this->voltage = rmsVoltage;
  // log(rmsVoltage);
  rmsCurrent = sqrt(rmsCurrent / 100);  //RMS current
  // log(rmsCurrent);
  this->current = rmsCurrent;
  this->pW = this->pW / 100;
  this->pVA = floor(rmsVoltage * rmsCurrent);
  this->powerFactor = floor(100 * pW / pVA) / 100;
  this->Wh += this->pW / 90000;  // Watt Hour, Every 40ms
  this->pW = floor(this->pW);
  // log(this->pW);
}

void Measure::computeTriacDelay() {
  if ((this->tank->getMode() & TANK_MODE_ON_MASK) > 0 && !this->tank->reachedTargetTemperature()) {    
    float delay = this->triacDelay + (this->pW + powerMargin) / 200;  //Decrease/Increase delay to increase/Decrease Triac conduction
    if (delay < minTriacDelay || this->tank->getMode() == TANK_MODE_ON) { 
      delay = minTriacDelay; 
    } else if (delay > 100) {
      delay = 100; 
    }
    this->overProduction = delay < 100;
    this->triacDelay = int(delay);
  } else {
    this->triacDelay = 100;
  }
}

void Measure::update() {
  if (millis() - this->previousComputeMillis > 40) {
    this->previousComputeMillis = millis();
    this->measurePower();
    this->computePower();
    this->computeTriacDelay();
  }
}

void Measure::resetWh() {
  this->Wh = 0;
}

float Measure::getCurrentVoltage() {
  return this->voltage;
}

float Measure::getCurrentAmp() {
  return this->current;
}