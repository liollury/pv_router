/**
  * Network gere le wifi et l'API rest
  * Measure gere les mesure et le triac
  * TODO : Timer interruption quand ESP32
  * Time gere le NTP et les heures de fonctionnement
  * Tank represente le ballon avec ses temperatures
  * Const les constantes
  * Logger le logger qui utilise Serial et RemoteDebug
  *  => pour accéder aux log en remote => putty en telnet
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
  */


#include "network.h"
#include "measure.h"
#include "time.h"
#include "logger.h"
#include "const.h"
#include <EEPROM.h>


Tank tank;
Measure measure(&tank);
Network network(&measure, &tank);
Time ntpTime(&measure, &tank);

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

void setup() { 
  Debug.begin(esp_name);
  Serial.begin(115200);
  log("[Sys] Booting");
  readEEPROM();
  network.setup();
  previousBlinkMillis = millis();
}

void loop() {
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  measure.update();
  network.update();
  ntpTime.update();

  if (millis() - previousBlinkMillis >= 2000) {
    blink = !blink;
    if (blink) {
      previousBlinkMillis = millis() - 1950;
    } else {
      previousBlinkMillis = millis();
    }
    if (measure.overProduction) {
      digitalWrite(LedRed, LOW);
      digitalWrite(LedGreen, blink);
    } else {
      digitalWrite(LedRed, blink);
      digitalWrite(LedGreen, LOW);
    }
  }
  Debug.handle();
}
