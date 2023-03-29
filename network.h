#include <HTTP_Method.h>
#include <Uri.h>
#include "tank.h"
#include "measure.h"
#include <ArduinoJson.h>

#ifndef Network_h
#define Network_h
class Network {
  private:
    StaticJsonDocument<270> jsonDocument;
    char buffer[270];
    int WIFIbug = 0;
    unsigned long previousWifiMillis;
    Measure* measure;
    Tank* tank;
    void setupWifi();
    void setupWebServer();
    void generateRestData();
    void handleOnConnect();
    void handleTankMode();
    void handleTargetTemperature();
    void handleRestart();
    void handleNotFound();
  public:
    Network(Measure* measure, Tank* tank);
    void setup();
    void wifiWatchdog();
    void handleWebserverClient();
    void update();
};
#endif