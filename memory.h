#include <EEPROM.h>

#ifndef Memory_h
#define Memory_h

static float readTemperatureFromEEPROM() {
  EEPROM.begin(128);
  float tankTargetTemperature;
  EEPROM.get(0, tankTargetTemperature);
  if (tankTargetTemperature != tankTargetTemperature || tankTargetTemperature < 40 || tankTargetTemperature > 80) { // NaN or out of bound
    tankTargetTemperature = 55;
  }
  EEPROM.end();
  return tankTargetTemperature;
}

static void writeTemperatureToEEPROM(float temperature) {
  EEPROM.begin(128);
  EEPROM.writeFloat(0, temperature);
  EEPROM.commit();
  EEPROM.end();
}

/*
 * 0 - nombre de redemarrage
 * 1 - erreur wifi
 * 2 - ESP_RST_UNKNOWN,    //!< Reset reason can not be determined
 * 3 - ESP_RST_POWERON,    //!< Reset due to power-on event
 * 4 - ESP_RST_EXT,        //!< Reset by external pin (not applicable for ESP32)
 * 5 - ESP_RST_SW,         //!< Software reset via esp_restart
 * 6 - ESP_RST_PANIC,      //!< Software reset due to exception/panic
 * 7 - ESP_RST_INT_WDT,    //!< Reset (software or hardware) due to interrupt watchdog
 * 8 - ESP_RST_TASK_WDT,   //!< Reset due to task watchdog
 * 9 - ESP_RST_WDT,        //!< Reset due to other watchdogs
 * 10- ESP_RST_DEEPSLEEP,  //!< Reset after exiting deep sleep mode
 * 11- ESP_RST_BROWNOUT,   //!< Brownout reset (software or hardware)
 * 12- ESP_RST_SDIO,       //!< Reset over SDIO
 */
static void writeLogData(int data[], int size) {
  EEPROM.begin(128);
  int address = sizeof(float);
  for (int i = 0; i < size; i++) {
    EEPROM.writeInt(address, data[i]);
    address += sizeof(int);
  }
  EEPROM.commit();
  EEPROM.end();
}

static void readLogData(int data[], int size) {
  EEPROM.begin(128);
  int address = sizeof(float);
  for (int i = 0; i < size; i++) {
    EEPROM.get(address, data[i]);
    address += sizeof(int);
  }
  EEPROM.end();
}

#endif