#include "clock.h"
#include "time.h"
#include "const.h"
#include "logger.h"


Clock::Clock(Measure *measure, Tank* tank) {
  this->measure = measure;
  this->tank = tank;
  this->previousClockMillis = millis();
  configTime(3600, 3600, "fr.pool.ntp.org");
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
    if (hour >= 19 || hour <= 8) {
      this->tank->setMode(TANK_MODE_OFF);
    } else { 
      this->tank->setMode(TANK_MODE_AUTO);
    }
    this->lastHour = hour;
  }
}