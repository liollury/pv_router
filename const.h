#ifndef Const_h
#define Const_h
#include "secret.h"
#include <Arduino.h>

//WIFI
static const char* ssid = SECRET_WIFI_SSID;
static const char* password = SECRET_WIFI_PASSWORD;

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
static const int minTriacDelay = 80; // maxLoadPower = 100% - minTriacDelay * loadPower. eg.: minTriacDelay = 80, load = 3kw, maxLoadPower = 3000 * 20% = 600w

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
