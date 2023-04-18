#include <HTTP_Method.h>
#include <Uri.h>
#include "tank.h"
#include "measure.h"
#include <ArduinoJson.h>
#include <WiFi.h>

#ifndef Network_h
#define Network_h
class Network {
  private:
    StaticJsonDocument<270> jsonDocument;
    char buffer[270];
    int WIFIbug = 0;
    unsigned long previousWifiMillis;
    bool connecting = false;
    Measure* measure;
    Tank* tank;
    void setupWifi();
    void connect();
    void disconnectEvent(WiFiEvent_t event, WiFiEventInfo_t info);
    void setupWebServer();
    void generateRestData();
    void handleOnConnect();
    void handleTankMode();
    void handleTargetTemperature();
    void handleErrorLog();
    void handleResetErrorLog();
    void handleRestart();
    void handleNotFound();
  public:
    Network(Measure* measure, Tank* tank);
    void setup();
    // void wifiWatchdog();
    void handleWebserverClient();
    void update();
};
#endif