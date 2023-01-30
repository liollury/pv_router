#include "tank.h"
#include "measure.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#ifndef Network_h
#define Network_h
class Network {
  private:
    StaticJsonDocument<250> jsonDocument;
    char buffer[250];
    int WIFIbug = 0;
    unsigned long previousWifiMillis;
    Measure* measure;
    Tank* tank;
    ESP8266WebServer server = ESP8266WebServer(8080);
    void setupWifi();
    void setupWebServer();
    void generateRestData();
    void handleOnConnect();
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