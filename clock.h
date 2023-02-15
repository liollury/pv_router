#include "network.h"
#include "tank.h"
#include "measure.h"

#ifndef Clock_h
#define Clock_h
class Clock {
  private:
    int lastHour = -1;
    Measure* measure;
    Tank* tank;
    unsigned long previousClockMillis;
    struct tm timeinfo;
  public:
    Clock(Measure *measure, Tank* tank);
    void update();
};
#endif