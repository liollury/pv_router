/**
  * Board : NodeMCU-32S de la lib esp32 espressif 1.0.6 https://dl.espressif.com/dl/package_esp32_index.json
  * Board : NodeMCU-32S de la lib esp32 espressif 2.0.7 (experimental) https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  * Network gere le wifi et l'API rest
  * Measure gere les mesure et le triac
  * Time gere le NTP et les heures de fonctionnement
  * Tank represente le ballon avec ses temperatures
  * Const les constantes
  * Logger le logger qui utilise Serial et RemoteDebug
  *  => pour accéder aux log en remote => putty en telnet
  *
  * Upload par wifi possible (sketch => port => wifi)
  *
  *
  *
  * Fonctionnement :
  *  - lecture pendant 20ms de la tension et du courant (une oscillation de 50Hz)
  *  - analyse du dephasage (puissance positive ou negative)
  *  - augmentation ou basse du délai de déclenchement du triac a chaque oscillation
  *
  * API /data pour récupérer les données
  * API /setTargetTemperature?temperature=XX pour définir la temperature cible/max du ballin
  * API /restart pour redémarrer l'ESP
  *
  * Libraries
  * RemoteDebug v2.1.2
  * ESP8266WiFi
  * WiFiUdp
  * EEPROM
  * ArduinoJson v6.20.0
  * NTPClient v3.2.1
  */


#include "network.h"
#include "measure.h"
#include "clock.h"
#include "logger.h"
#include "const.h"
#include <EEPROM.h>
#include <ArduinoOTA.h>

Temperature temperature;
Tank tank(&temperature);
Measure measure(&tank, &temperature);
Network network(&measure, &tank);
Clock routerClock(&measure, &tank);

unsigned long previousBlinkMillis;
bool blink = false;

void readEEPROM() {
  EEPROM.begin(128);
  float tankTargetTemperature;
  EEPROM.get(0, tankTargetTemperature);
  if (tankTargetTemperature != tankTargetTemperature || tankTargetTemperature < 40 || tankTargetTemperature > 80) { // NaN or out of bound
    tankTargetTemperature = 55;
  }
  tank.setTargetTemperature(tankTargetTemperature);
  EEPROM.end();
}

void initSequence() {
  digitalWrite(LedRed, HIGH);
  delay(500);
  digitalWrite(LedRed, LOW);
  digitalWrite(LedGreen, HIGH);
  delay(500);
  digitalWrite(LedGreen, LOW);
  digitalWrite(LedBlue, HIGH);
  delay(500);
  digitalWrite(LedBlue, LOW);
}

void setup() { 
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(LedBlue, OUTPUT);
  initSequence();
  Debug.begin(esp_name);
  Serial.begin(115200);
  log("[Sys] Booting");
  readEEPROM();
  // mandatory in 2.0.7 NodeMCU-32S board, interrupt cannot be set before setup
  log("[Sys] Measures setup");
  measure.setup();
  log("[Sys] Network setup");
  network.setup();
  log("[Sys] One wire temperature setup");
  temperature.setup();
  previousBlinkMillis = millis();
  tank.update();
  initOTA();
}

void loop() {
  measure.update();
  network.update();
  routerClock.update();
  temperature.update();
  tank.update();

   if (millis() - previousBlinkMillis >= 2000) {
    blink = !blink;
    if (blink) {
      previousBlinkMillis = millis() - 1950;
    } else {
      previousBlinkMillis = millis();
    }
    if (!measure.isPowerConnected) {
      digitalWrite(LedRed, blink);
      digitalWrite(LedGreen, blink);        
    } else if (measure.overProduction) {
      digitalWrite(LedRed, LOW);
      digitalWrite(LedGreen, blink);
    } else {
      digitalWrite(LedRed, blink);
      digitalWrite(LedGreen, LOW);
    }
    if ((tank.getMode() & TANK_MODE_ON_MASK) > 0) {
      digitalWrite(LedBlue, HIGH);      
    } else {
      digitalWrite(LedBlue, LOW);
    }
  }
  Debug.handle();
  ArduinoOTA.handle();
}

void initOTA() {
  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname("ESP32-PVRouter");
  ArduinoOTA.setPassword(OTA_PASSWD);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    log("Start updating " + type);
  })
  .onEnd([]() {
    log("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) { log("Auth Failed"); }
    else if (error == OTA_BEGIN_ERROR) { log("Begin Failed"); }
    else if (error == OTA_CONNECT_ERROR) { log("Connect Failed"); }
    else if (error == OTA_RECEIVE_ERROR) { log("Receive Failed"); }
    else if (error == OTA_END_ERROR) { log("End Failed"); }
  });

  ArduinoOTA.begin();
}