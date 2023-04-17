#include "network.h"
#include "const.h"
#include "logger.h"
#include <WiFi.h>
#include "memory.h"
#include <WebServer.h>

WebServer server(8080);

Network::Network(Measure *measure, Tank* tank) {
  this->measure = measure;
  this->tank = tank;
  this->previousWifiMillis = millis();
}

void Network::generateRestData() {
  this->jsonDocument.clear();
  if (this->tank->getTemperature() != 999) {
    this->jsonDocument["tankTemperature"] = this->tank->getTemperature();
  }
  this->jsonDocument["tankTargetTemperature"] = this->tank->getTargetTemperature();
  if (this->measure->triacTemperature != 999) {
    this->jsonDocument["triacTemperature"] = this->measure->triacTemperature;
  }
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

void Network::handleErrorLog() {
  int data[13];
  readLogData(data, 13);
  char error[100];
  sprintf(error, "[%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i]", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12]);
  server.send(200, "application/json", error);
}

void Network::handleResetErrorLog() {
  int data[13];
  for(int i = 0; i < 13; i++) { data[i] = 0; }
  writeLogData(data, 13);
  server.send(200, "text/html", "<h1>OK</h1>");
}

void Network::handleTargetTemperature() {
  float temperature = server.arg("temperature").toFloat();
  if (temperature != temperature || temperature < 40 || temperature > 80) { // NaN or out of bound
    server.send(400, "text/html", "<h1>Value must be float between 40 and 80</h1>");
  } else {
    writeTemperatureToEEPROM(temperature);
    this->tank->setTargetTemperature(temperature);
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
  WiFi.setAutoReconnect(true);
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
  server.on("/resetLog", [=](){handleResetErrorLog();});
  server.on("/log", [=](){handleErrorLog();});
  server.onNotFound([=](){handleNotFound();});
  server.begin();
  log("[WebServer] HTTP server started");
  log("[WebServer] Liveness endpoint : http://" + WiFi.localIP().toString() + ":8080/");
  log("[WebServer] Data endpoint : http://" + WiFi.localIP().toString() + ":8080/data");
  log("[WebServer] Tank target temperature endpoint : http://" + WiFi.localIP().toString() + ":8080/setTargetTemperature?temperature=XX");
  log("[WebServer] Tank mode endpoint : http://" + WiFi.localIP().toString() + ":8080/setTankMode?mode=[0=off, 1=auto_off, 2=on, 3=auto_on]");
  log("[WebServer] Restart endpoint : http://" + WiFi.localIP().toString() + ":8080/restart");
  log("[WebServer] Reset log endpoint : http://" + WiFi.localIP().toString() + ":8080/resetLog");
  log("[WebServer] Error log endpoint : http://" + WiFi.localIP().toString() + ":8080/log");
}


void Network::wifiWatchdog() {
  if (millis() - this->previousWifiMillis > 30000) {
    this->previousWifiMillis = millis();
    if (WiFi.waitForConnectResult(30000) != WL_CONNECTED) {
      log("[WiFi] Connection Failed! #" + String(WIFIbug));
      this->WIFIbug++;
      if (this->WIFIbug == 1) {
        int data[13];
        readLogData(data, 13);
        data[1]++;
        writeLogData(data, 13);
      }
      if (this->WIFIbug > 20) {
        this->measure->stopTriac();
        delay(1000);
        ESP.restart();
      }
      WiFi.reconnect();
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