#include "clock.h"
#include "time.h"
#include "const.h"
#include "logger.h"


Clock::Clock(Measure *measure, Tank* tank) {
  this->measure = measure;
  this->tank = tank;
  this->previousClockMillis = millis();
  configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", "fr.pool.ntp.org");
}

void Clock::update() {
  if (millis() - this->previousClockMillis > 30000) {
    this->previousClockMillis = millis();
    if(!getLocalTime(&timeinfo)){
      log("Failed to obtain time");
      return;
    }
    char logChar[80];
    strftime(logChar, 80, "%A, %B %d %Y %H:%M:%S", &timeinfo);
    log(logChar);

    char timeHour[3];
    strftime(timeHour, 3, "%H", &timeinfo);
    int hour = atoi(timeHour);

    if (this->lastHour != 0 && hour == 0) { 
      this->measure->resetWh(); // Reset Watt Hour of the day. Once every day
    }
    // Turn off tank at night
    if ((this->tank->getMode() & TANK_MODE_AUTO_MASK) > 0) {
      if ((hour >= 19 || hour <= 8)) {
        this->tank->setMode(TANK_MODE_AUTO_OFF);
      } else { 
        this->tank->setMode(TANK_MODE_AUTO_ON);
      }
    }
    this->lastHour = hour;
  }
}