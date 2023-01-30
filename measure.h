#include "tank.h"

#ifndef Measure_h
#define Measure_h
class Measure {
  private:
    int volt[100];
    int amp[100];
    float voltM[100];
    float ampM[100];
    int refVolt;
    Tank* tank;
    unsigned long previousComputeMillis;
    volatile unsigned long lastZeroCrossInterruption = 0;
    volatile bool phasePositive = true;
    int currentTriacPosition = 0;
    volatile int zeroCrossingMs = 0;
    // hw_timer_t* timer = NULL;
  public:
    float pW;
    float pVA;
    float Wh = 0; 
    float powerFactor;
    int triacDelay = 100;
    bool overProduction = false;
    Measure(Tank* tank);
    void measurePower();
    void computePower();
    void computeTriacDelay();
    void update();
    void resetWh();
    void zeroCrossInterrupt();
    void onTimerInterrupt();
    float getCurrentVoltage();
    float getCurrentAmp();
};
#endif