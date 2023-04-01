#include "network.h"
#include "const.h"
#include "logger.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <WebServer.h>

WebServer server(8080);

Network::Network(Measure *measure, Tank* tank) {
  this->measure = measure;
  this->tank = tank;
  this->previousWifiMillis = millis();
}

void Network::generateRestData() {
  this->jsonDocument.clear();
  this->jsonDocument["tankTemperature"] = this->tank->getTemperature();
  this->jsonDocument["tankTargetTemperature"] = this->tank->getTargetTemperature();
  this->jsonDocument["triacTemperature"] = this->measure->triacTemperature;
  this->jsonDocument["triacPosition"] = 100 - this->measure->triacDelay; // (100 - delay)%
  this->jsonDocument["overProduction"] = this->measure->overProduction;
  this->jsonDocument["consumption"] = this->measure->Wh / 1000;
  this->jsonDocument["tankMode"] = this->tank->getMode();
  this->jsonDocument["powerConnected"] = this->measure->isPowerConnected;
  if (this->measure->isPowerConnected) {
    this->jsonDocument["instant"] = this->measure->pW;
    this->jsonDocument["voltage"] = this->measure->getCurrentVoltage();
    this->jsonDocument["amp"] = this->measure->getCurrentAmp();
  }
  serializeJson(this->jsonDocument, this->buffer);
  server.send(200, "application/json", this->buffer);
}

void Network::handleTargetTemperature() {
  float temperature = server.arg("temperature").toFloat();
  if (temperature != temperature || temperature < 40 || temperature > 80) { // NaN or out of bound
    server.send(400, "text/html", "<h1>Value must be float between 40 and 80</h1>");
  } else {
    EEPROM.begin(128);
    EEPROM.put(0, temperature);
    this->tank->setTargetTemperature(temperature);
    EEPROM.commit();
    EEPROM.end();
    server.send(200, "text/html", "<h1>OK</h1>");
  }
}

void Network::handleTankMode() {
  float mode = server.arg("mode").toInt();
  if (mode < 0 || mode > 3) { // NaN or out of bound
    server.send(400, "text/html", "<h1>Value must be int [0=off, 1=auto_off, 2=on, 3=auto_on]</h1>");
  } else {
    this->tank->setMode(mode);
    server.send(200, "text/html", "<h1>OK</h1>");
  }
}

void Network::handleOnConnect() {
  server.send(200, "text/html", "<h1>OK</h1>");
}

void Network::handleNotFound() {
  server.send(404, "text/html", "<h1>404 Not Found</h1>"); 
}

void Network::handleRestart() {
  server.send(200, "application/json", "{\"response\": \"ok\"}"); 
  this->measure->stopTriac();
  delay(1000);
  ESP.restart();
}

void Network::setupWifi() {
  WiFi.hostname(esp_name);
  log("[WiFi] Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    log("[WiFi] Connection Failed! Rebooting...");
    this->measure->stopTriac();
    delay(5000);
    ESP.restart();
  }

  log("[WiFi] Ready");
  previousWifiMillis = millis();
}

void Network::setupWebServer() {
  //Init Web Server on port 80
  log("[WebServer] Starting HTTP server..");
  server.on("/", [=](){handleOnConnect();}); // lambda function with [=] to pass "this"
  server.on("/data", [=](){generateRestData();});
  server.on("/setTargetTemperature", [=](){handleTargetTemperature();});
  server.on("/setTankMode", [=](){handleTankMode();});
  server.on("/restart", [=](){handleRestart();});
  server.onNotFound([=](){handleNotFound();});
  server.begin();
  log("[WebServer] HTTP server started");
  log("[WebServer] Liveness endpoint : http://" + WiFi.localIP().toString() + ":8080/");
  log("[WebServer] Data endpoint : http://" + WiFi.localIP().toString() + ":8080/data");
  log("[WebServer] Data endpoint : http://" + WiFi.localIP().toString() + ":8080/setTargetTemperature?temperature=XX");
  log("[WebServer] Data endpoint : http://" + WiFi.localIP().toString() + ":8080/setTankMode?mode=[0=off, 1=auto_off, 2=on, 3=auto_on]");
  log("[WebServer] Restart endpoint : http://" + WiFi.localIP().toString() + ":8080/restart");
}


void Network::wifiWatchdog() {
  if (millis() - this->previousWifiMillis > 30000) {
    this->previousWifiMillis = millis();
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      log("[WiFi] Connection Failed! #" + String(WIFIbug));
      this->WIFIbug++;
      if (this->WIFIbug > 20) {
        this->measure->stopTriac();
        delay(1000);
        ESP.restart();
      }
    } else {
      this->WIFIbug = 0;
    }
  }  
}

void Network::handleWebserverClient() {
  server.handleClient();
}

void Network::setup() {
  this->setupWifi();
  this->setupWebServer();
}

void Network::update() {
  this->handleWebserverClient();
  this->wifiWatchdog();
}