#ifndef Logger_h
#define Logger_h
#include "const.h"
#include <RemoteDebug.h>

extern RemoteDebug Debug;

#define log(a) Serial.println(a); Debug.println(a);

#endif