#ifndef Const_h
#define Const_h
#include "secret.h"
#include <Arduino.h>

//WIFI
static const char* ssid = SECRET_WIFI_SSID;
static const char* password = SECRET_WIFI_PASSWORD;

//ONE WIRE - to determinate with READ_ONE_WIRE_SENSORS_ADDRESS to print addresses
#define READ_ONE_WIRE_SENSORS_ADDRESS
static uint8_t TankOneWireTempSensor[8] = { 0x28, 0xA3, 0xDC, 0x49, 0xF6, 0x86, 0x3C, 0xD3 };
static uint8_t TriacOneWireTempSensor[8] = { 0x28, 0x29, 0x86, 0x48, 0xF6, 0x33, 0x3C, 0xAC };

//PINS - GPIO
static const int AnalogRef = 35;
static const int AnalogVolt = 32;
static const int AnalogAmp = 33;
static const int ZeroCross = 23;
static const int PulseTriac = 22;
static const int LedRed = 19;
static const int LedGreen = 18;
static const int LedBlue = 5;
static const int dallasOneWire = 14;

//CALIBRATION
static const float kV = 0.18453;    //Calibration coefficient for the voltage. To be adjusted
static const float kI = 0.085; // valeur avec bouilloir : 0.048203 valeur avec linky : 0.052988   //Calibration coefficient for the current. To be adjusted
static const int minTriacDelay = 55; // 0-100; maxLoadPower = 100% - minTriacDelay * loadPower. eg.: minTriacDelay = 80, load = 3kw, maxLoadPower = 3000 * 20% = 600w
static const int loadPower = 3000; // load is 3kW
static const int triacLoadStep = 100; // how many W per %age of triac, lower value is best precision but make "yoyo" triac position, default 200
static const int powerMargin = 2 * loadPower / 100; // Allowable injection (W)

//TANK MODES
static const int TANK_MODE_AUTO_ON = 0b11;
static const int TANK_MODE_AUTO_OFF = 0b01;
static const int TANK_MODE_OFF = 0b00;
static const int TANK_MODE_ON = 0b10;

static const int TANK_MODE_AUTO_MASK = 0b01;
static const int TANK_MODE_ON_MASK = 0b10;

//NAME
static const char* esp_name = "ESP PV Router";
#endif
