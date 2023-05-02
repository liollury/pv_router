#include "const.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef Temperature_h
#define Temperature_h
class Temperature {
  private:
    unsigned long previousComputeMillis = 0;
    int failCount = 0;

  public:
    Temperature();
    void setup();
    void update();
    float getTemperature(DeviceAddress addr, int bus, float fallbackTemperature, float failTemperature);
    #ifdef READ_ONE_WIRE_SENSORS_ADDRESS
    void readSensorsAddress();
    #endif
  private:
    void updateSensors();
};
#endif