#include "const.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef Temperature_h
#define Temperature_h
class Temperature {
  private:
    unsigned long previousComputeMillis = 0;
  public:
    Temperature();
    void update();
    float getTemperature(DeviceAddress addr);
    #ifdef READ_ONE_WIRE_SENSORS_ADDRESS
    void readSensorsAddress();
    #endif
};
#endif