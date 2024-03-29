#ifndef Const_h
#define Const_h
#include "secret.h"
#include <Arduino.h>

//WIFI
static const char* WIFI_SSID = SECRET_WIFI_SSID;
static const char* WIFI_PASSWD = SECRET_WIFI_PASSWORD;
static const char* ESP_NAME = "ESP_PV_Router";
static uint8_t WIFI_BSSID[] = { 0xdc, 0x00, 0xb0, 0xc2, 0x96, 0x92 };
//static uint8_t WIFI_BSSID[] = { 0x00, 0x11, 0x32, 0x75, 0x81, 0xfd }; // router synology


//Dual core configuration
#define MAIN_WIFI_CORE 0
#define SECONDARY_CORE 1
#define RUN_DUAL_CORE_PROGRAM
#ifdef RUN_DUAL_CORE_PROGRAM
static const int CRITICAL_CORE = SECONDARY_CORE;
static const int ACCESSORY_CORE = MAIN_WIFI_CORE;
#endif

//ONE WIRE - to determinate with READ_ONE_WIRE_SENSORS_ADDRESS to print addresses
#define READ_ONE_WIRE_SENSORS_ADDRESS
#define ACCEPTABLE_TEMPERATURE_READING_FAIL 2
static uint8_t TankOneWireTempSensor[8] = { 0x28, 0xA3, 0xDC, 0x49, 0xF6, 0x86, 0x3C, 0xD3 };
static uint8_t TriacOneWireTempSensor[8] = { 0x28, 0x29, 0x86, 0x48, 0xF6, 0x33, 0x3C, 0xAC };
static int TriacOneWireBus = 1;
static int TankOneWireBus = 2;
static int ResolutionOnDallasBus[2] = {9, 11}; // 9, 10, 11, or 12-bits, which correspond to 0.5°C, 0.25°C, 0.125°C, and 0.0625°C
#ifdef RUN_DUAL_CORE_PROGRAM
//#define WAIT_FOR_REQUEST_TEMPERATURE
#endif


//PINS - GPIO
static const int AnalogRef = 35;
static const int AnalogVolt = 32;
static const int AnalogAmp = 33;
static const int ZeroCross = 23;
static const int PulseTriac = 22;
static const int LedRed = 19;
static const int LedGreen = 18;
static const int LedBlue = 5;
static const int dallasOneWireBus1 = 16;
static const int dallasOneWireBus2 = 17;

//CALIBRATION
#define MANAGE_TRIAC_ON_HALF_PERIOD // Triac will run only on half period, it will divide max power by 2 but reduce interference. You must adapt minTriacDelay
static const float kV = 0.18453;    //Calibration coefficient for the voltage. To be adjusted
static const float kI = 0.085; // valeur avec bouilloir : 0.048203 valeur avec linky : 0.052988   //Calibration coefficient for the current. To be adjusted
#ifdef MANAGE_TRIAC_ON_HALF_PERIOD
static const int minTriacDelay = 0; // 0-100; maxLoadPower = 100% - minTriacDelay * loadPower / 2. eg.: minTriacDelay = 80, load = 3kw, maxLoadPower = 3000 * 20% / 2 = 300w
#else
static const int minTriacDelay = 55; // 0-100; maxLoadPower = 100% - minTriacDelay * loadPower. eg.: minTriacDelay = 80, load = 3kw, maxLoadPower = 3000 * 20% = 600w
#endif
static const int loadPower = 3000; // load is 3kW
static const int triacLoadStep = 100; // how many W per %age of triac, lower value is best precision but make "yoyo" triac position, default 200
static const int powerMargin = 3 * loadPower / 100; // Allowable injection (W)

//TANK MODES
static const int TANK_MODE_AUTO_ON = 0b11;
static const int TANK_MODE_AUTO_OFF = 0b01;
static const int TANK_MODE_OFF = 0b00;
static const int TANK_MODE_ON = 0b10;

static const int TANK_MODE_AUTO_MASK = 0b01;
static const int TANK_MODE_ON_MASK = 0b10;

//LOG
static const int ERROR_LOG_SIZE = 17;

#endif
