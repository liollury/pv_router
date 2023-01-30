#include "time.h"
#include "const.h"
#include "logger.h"
#include <NTPClient.h>

Time::Time(Measure *measure, Tank* tank) {
  this->measure = measure;
  this->tank = tank;
  this->previousTimeMillis = millis();
  this->ntp.begin();
}

void Time::update() {
  if (millis() - this->previousTimeMillis > 30000) {
    this->previousTimeMillis = millis();
    this->ntp.update();
    log("[NTP] " + this->ntp.getFormattedTime());
    int hour = int(this->ntp.getHours());
    if (this->lastHour != 0 && hour == 0) { 
      this->measure->resetWh(); // Reset Watt Hour of the day. Once every day
    }
    // Turn off tank at night
    if (hour >= 19 || hour <= 8) {
      this->tank->setMode(TANK_MODE_OFF);
    } else { 
      this->tank->setMode(TANK_MODE_AUTO);
    }
    this->lastHour = hour;
  }
}