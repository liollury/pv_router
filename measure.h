#include "tank.h"

#ifndef Measure_h
#define Measure_h
class Measure {
  private:
    int volt[100];
    int amp[100];
    float voltM[100];
    float ampM[100];
    float voltage = 0;
    float current = 0;
    int refVolt;
    Tank* tank;
    Temperature* temperatureSensor;
    unsigned long previousComputeMillis;
    unsigned long previousTemperatureMillis;
    volatile unsigned long lastZeroCrossInterruption = 0;
    volatile bool phasePositive = true;
    int currentTriacPosition = 0;
    volatile int zeroCrossingMs = 0;
    hw_timer_t* timer = NULL;
    bool syncLost = false;
  public:
    float pW;
    float Wh = 0;
    bool isPowerConnected = false; 
    int triacDelay = 100;
    float triacTemperature = 999;
    bool overProduction = false;
    Measure(Tank* tank, Temperature* temperatureSensor);
    void setup();
    void measurePower();
    void computePower();
    void computeTriacDelay();
    void update();
    void resetWh();
    void zeroCrossInterrupt();
    void onTimerInterrupt();
    void stopTriac();
    float getCurrentVoltage();
    float getCurrentAmp();
};
#endif