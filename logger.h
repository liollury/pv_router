#ifndef Logger_h
#define Logger_h
#include "const.h"
#include <RemoteDebug.h>

extern RemoteDebug Debug;
static String logMsgTmp;
static SemaphoreHandle_t logMutex = xSemaphoreCreateMutex();

#define log(a) xSemaphoreTake(logMutex, portMAX_DELAY); Serial.println(a); Debug.println(a); xSemaphoreGive(logMutex);

// #define log(a) xSemaphoreTake(logMutex, portMAX_DELAY); logMsgTmp = String("[Core") + String(xPortGetCoreID()) + String("]") + String(a); Serial.println(logMsgTmp); Debug.println(logMsgTmp); xSemaphoreGive(logMutex);

#endif
