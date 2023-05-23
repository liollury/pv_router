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

#define CONFIG_LWIP_DHCP_DOES_ARP_CHECK 0
#define DHCP_DOES_ARP_CHECK 0

#include "network.h"
#include "measure.h"
#include "clock.h"
#include "logger.h"
#include "const.h"
#include "memory.h"

Temperature temperature;
Tank tank(&temperature);
Measure measure(&tank, &temperature);
Network network(&measure, &tank);
Clock routerClock(&measure, &tank);

TaskHandle_t CriticalThreadTask;
TaskHandle_t AccessoryThreadTask;
TaskHandle_t LauncherThreadTask;

unsigned long previousBlinkMillis;
bool blink = false;


void initSequence() {
  digitalWrite(LedBlue, HIGH);
  delay(500);
  digitalWrite(LedBlue, LOW);
  digitalWrite(LedRed, HIGH);
  delay(500);
  digitalWrite(LedRed, LOW);
  digitalWrite(LedGreen, HIGH);
  delay(500);
  digitalWrite(LedGreen, LOW);
}


void logBootCause() {
  readLogData();
  errorLogData[0]++;
  errorLogData[2 + esp_reset_reason()]++;
  writeLogData(errorLogData);
}

void setup() { 
  logBootCause();
  #ifdef RUN_DUAL_CORE_PROGRAM
  xTaskCreatePinnedToCore(launcherThread, "Launcher thread", 10000, NULL, 2, &LauncherThreadTask, 1); 
  log("[Sys] Start program in dual core mode");
  #else
  log("[Sys] Start program in single core mode");
  launcherWorker();
  #endif
}

void launcherWorker() {
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(LedBlue, OUTPUT);
  initSequence();
  Debug.begin(ESP_NAME);
  Serial.begin(115200);
  log("[Sys] Booting");
  tank.setTargetTemperature(readTemperatureFromEEPROM());
  // setup
  log("[Sys] Network setup");
  network.setup();
  log("[Sys] One wire temperature setup");
  temperature.setup();
  // mandatory in 2.0.7 NodeMCU-32S board, interrupt cannot be set before setup
  log("[Sys] Measures setup");
  measure.setup();
  previousBlinkMillis = millis();
}

#ifdef RUN_DUAL_CORE_PROGRAM
void launcherThread(void* parameter) {
  launcherWorker();
  xTaskCreatePinnedToCore(criticalThread, "Critical thread", 10000, NULL, 2, &CriticalThreadTask, CRITICAL_CORE);
  vTaskDelay(500); 
  xTaskCreatePinnedToCore(accessoryThread, "Accessory thread", 10000, NULL, 0, &AccessoryThreadTask, ACCESSORY_CORE);
  vTaskDelay(500); 
  vTaskDelete(LauncherThreadTask);
}
#endif

void criticalWorker() {
    measure.update();
}

#ifdef RUN_DUAL_CORE_PROGRAM
void criticalThread(void* parameter) {
  log("[Sys] Critical thread created");
  // loop
  for(;;) {
    criticalWorker();
    vTaskDelay(20);
  }
  vTaskDelete(CriticalThreadTask);
}
#endif

void accessoryWorker() {
  tank.update();
  network.update();
  routerClock.update();
  temperature.update();
  if (millis() - previousBlinkMillis >= 2000) {
    blink = !blink;
    if (blink) {
      previousBlinkMillis = millis() - 1900;
    } else {
      previousBlinkMillis = millis();
    }
    if (!network.isConnected()) {
      digitalWrite(LedBlue, blink);
    } else {
      digitalWrite(LedBlue, LOW);
    }
    if (!measure.isPowerConnected) {
      digitalWrite(LedRed, !blink);
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
}

#ifdef RUN_DUAL_CORE_PROGRAM
void accessoryThread(void* parameter) {
  log("[Sys] Accessory thread created");
  // loop
  for(;;) {
    accessoryWorker();
    vTaskDelay(50);
  }
  vTaskDelete(AccessoryThreadTask);
}
#endif

void loop() {
  #ifdef RUN_DUAL_CORE_PROGRAM
  vTaskDelay(1000);
  #else
  criticalWorker();
  accessoryWorker();
  vTaskDelay(20);
  #endif
}
