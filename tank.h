#include "const.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef Tank_h
#define Tank_h
class Tank {
  private:
    int mode = TANK_MODE_AUTO_OFF;
    float temperature = 45.5;
    float targetTemperature = 70;
    unsigned long previousComputeMillis;
  public:
    Tank();
    void setMode(int mode);
    int getMode();
    void setTemperature(float temperature);
    float getTemperature();
    void setTargetTemperature(float temperature);
    float getTargetTemperature();
    bool reachedTargetTemperature();
    void update();
};
#endif