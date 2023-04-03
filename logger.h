#ifndef Logger_h
#define Logger_h
#include "const.h"
#include <RemoteDebug.h>

extern RemoteDebug Debug;
static String logMsgTmp;
static SemaphoreHandle_t logMutex = xSemaphoreCreateMutex();

#define log(a) xSemaphoreTake(logMutex, portMAX_DELAY); logMsgTmp = (String)"[Core" + xPortGetCoreID() + (String)"]" + (String)a; Serial.println(logMsgTmp); Debug.println(logMsgTmp); xSemaphoreGive(logMutex);

#endif