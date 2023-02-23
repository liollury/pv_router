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
static const int LedGreen = 21;
static const int dallasOneWire = 14;

//CALIBRATION
static const float kV = 0.18453;    //Calibration coefficient for the voltage. To be adjusted
static const float kI = 0.048203;    //Calibration coefficient for the current. To be adjusted

//TANK MODES
static const int TANK_MODE_AUTO = 0;
static const int TANK_MODE_OFF = 1;

//NAME
static const char* esp_name = "ESP PV Router";
#endif
