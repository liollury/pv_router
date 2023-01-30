#include "network.h"
#include "const.h"
#include "logger.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>


Network::Network(Measure *measure, Tank* tank) {
  this->measure = measure;
  this->tank = tank;
  this->previousWifiMillis = millis();
}

void Network::generateRestData() {
  this->jsonDocument.clear();
  this->jsonDocument["tankTemperature"] = this->tank->getTemperature();
  this->jsonDocument["tankTargetTemperature"] = this->tank->getTargetTemperature();
  this->jsonDocument["triacPosition"] = 100 - this->measure->triacDelay; // (100 - delay)%
  this->jsonDocument["overProduction"] = this->measure->overProduction;
  this->jsonDocument["consumption"] = this->measure->Wh / 1000;
  this->jsonDocument["voltage"] = this->measure->getCurrentVoltage();
  this->jsonDocument["amp"] = this->measure->getCurrentAmp();
  serializeJson(this->jsonDocument, this->buffer);
  this->server.send(200, "application/json", this->buffer);
}

void Network::handleTargetTemperature() {
  float temperature = server.arg("temperature").toFloat();
  if (temperature != temperature || temperature < 40 || temperature > 80) { // NaN or out of bound
    this->server.send(400, "text/html", "<h1>Value must be float between 40 and 80</h1>");
  } else {
    EEPROM.begin(128);
    EEPROM.put(0, temperature);
    this->tank->setTargetTemperature(temperature);
    EEPROM.commit();
    EEPROM.end();
    this->server.send(200, "text/html", "<h1>OK</h1>");
  }
  
}

void Network::handleOnConnect() {
  this->server.send(200, "text/html", "<h1>OK</h1>");
}

void Network::handleNotFound() {
  this->server.send(404, "text/html", "<h1>404 Not Found</h1>"); 
}

void Network::handleRestart() {
  this->server.send(200, "application/json", "{\"response\": \"ok\"}"); 
  delay(1000);
  ESP.restart();
}

void Network::setupWifi() {
  WiFi.hostname(esp_name);
  log("[WiFi] Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    log("[WiFi] Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  log("[WiFi] Ready");
  Serial.print("[WiFi] IP address: ");
  log(WiFi.localIP());
  previousWifiMillis = millis();
}

void Network::setupWebServer() {
  //Init Web Server on port 80
  log("[WebServer] Starting HTTP server...");
  this->server.on("/", [=](){handleOnConnect();}); // lambda function with [=] to pass "this"
  this->server.on("/data", [=](){generateRestData();});
  this->server.on("/setTargetTemperature", [=](){handleTargetTemperature();});
  this->server.on("/restart", [=](){handleRestart();});
  this->server.onNotFound([=](){handleNotFound();});
  this->server.begin();
  log("[WebServer] HTTP server started");
  log("[WebServer] Liveness endpoint : http://" + WiFi.localIP().toString() + ":8080/");
  log("[WebServer] Data endpoint : http://" + WiFi.localIP().toString() + ":8080/data");
  log("[WebServer] Data endpoint : http://" + WiFi.localIP().toString() + ":8080/setTargetTemperature?temperature=XX");
  log("[WebServer] Restart endpoint : http://" + WiFi.localIP().toString() + ":8080/restart");
}


void Network::wifiWatchdog() {
  if (millis() - this->previousWifiMillis > 30000) {
    this->previousWifiMillis = millis();
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      log("[WiFi] Connection Failed! #" + String(WIFIbug));
      this->WIFIbug++;
      if (this->WIFIbug > 20) {
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