#include <EEPROM.h>

#ifndef Memory_h
#define Memory_h

static int errorLogData[ERROR_LOG_SIZE];

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
 * 13- Last Wifi disconnect reason
 * 14- temperature sensor failures bus 1
 * 15- temperature sensor failures bus 2
 * 16- temperature sensor last error code
 * 
 * Wifi disconnect reason list :
* 
 * WIFI_REASON_UNSPECIFIED              = 1,
 * WIFI_REASON_AUTH_EXPIRE              = 2,
 * WIFI_REASON_AUTH_LEAVE               = 3,
 * WIFI_REASON_ASSOC_EXPIRE             = 4,
 * WIFI_REASON_ASSOC_TOOMANY            = 5,
 * WIFI_REASON_NOT_AUTHED               = 6,
 * WIFI_REASON_NOT_ASSOCED              = 7,
 * WIFI_REASON_ASSOC_LEAVE              = 8,
 * WIFI_REASON_ASSOC_NOT_AUTHED         = 9,
 * WIFI_REASON_DISASSOC_PWRCAP_BAD      = 10,
 * WIFI_REASON_DISASSOC_SUPCHAN_BAD     = 11,
 * WIFI_REASON_IE_INVALID               = 13,
 * WIFI_REASON_MIC_FAILURE              = 14,
 * WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,
 * WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
 * WIFI_REASON_IE_IN_4WAY_DIFFERS       = 17,
 * WIFI_REASON_GROUP_CIPHER_INVALID     = 18,
 * WIFI_REASON_PAIRWISE_CIPHER_INVALID  = 19,
 * WIFI_REASON_AKMP_INVALID             = 20,
 * WIFI_REASON_UNSUPP_RSN_IE_VERSION    = 21,
 * WIFI_REASON_INVALID_RSN_IE_CAP       = 22,
 * WIFI_REASON_802_1X_AUTH_FAILED       = 23,
 * WIFI_REASON_CIPHER_SUITE_REJECTED    = 24,
 * WIFI_REASON_BEACON_TIMEOUT           = 200,
 * WIFI_REASON_NO_AP_FOUND              = 201,
 * WIFI_REASON_AUTH_FAIL                = 202,
 * WIFI_REASON_ASSOC_FAIL               = 203,
 * WIFI_REASON_HANDSHAKE_TIMEOUT        = 204,
 */
static void writeLogData(int data[]) {
  EEPROM.begin(128);
  int address = sizeof(float);
  for (int i = 0; i < ERROR_LOG_SIZE; i++) {
    EEPROM.writeInt(address, data[i]);
    errorLogData[i] = data[i];
    address += sizeof(int);
  }
  EEPROM.commit();
  EEPROM.end();
}

static void resetLogData() {
  for(int i = 0; i < ERROR_LOG_SIZE; i++) { errorLogData[i] = 0; }
  writeLogData(errorLogData);
}

static void readLogData() {
  EEPROM.begin(128);
  int address = sizeof(float);
  for (int i = 0; i < ERROR_LOG_SIZE; i++) {
    EEPROM.get(address, errorLogData[i]);
    address += sizeof(int);
  }
  EEPROM.end();
}

#endif