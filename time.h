#include "network.h"
#include "tank.h"
#include "measure.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

#ifndef Time_h
#define Time_h
class Time {
  private:
    int lastHour = -1;
    Measure* measure;
    Tank* tank;
    WiFiUDP ntpUDP = WiFiUDP();
    NTPClient ntp = NTPClient(this->ntpUDP, "fr.pool.ntp.org", 3600, 300000);
    unsigned long previousTimeMillis;
  public:
    Time(Measure *measure, Tank* tank);
    void update();
};
#endif