# PV Router

## Introduction

**PV Router** is an Arduino based code for esp32. It measures the extra production coming from solar panels to inject it into a consumer supporting variable consumption by angle modulation.
In the standard case, it allows the surplus solar energy to be injected into a heating resistance, a water heater.

## Safety reminder
Before connecting the router to the electrical grid, ensure compliance with local electrical safety standards:

- Use properly insulated cables to avoid short circuits.
- Install protective devices, such as circuit breakers, to prevent overloads and short circuits.
- If unsure, seek the assistance of a qualified professional for installation.

## Wire

![](https://github.com/liollury/pv_router/blob/master/schema.png)

Router a some probe to be fully operationnal : 
 - Voltage probe, it measure grid voltage by using a 8V transformer
 - A CT Clamp is use to get current of grid (it can be clamp just before or juste after linky)
 - A long cable temperature probe with a DS18B20 (to the water tank)
 - A short cable temperature probe with a DS18B20 for the triac temperature
 - An output to the robotdyn triac to control heater
 - 3 leds to show router status

## Main features

To improve performance, this code use the two CPU cores of the esp32.

### Detect overproduction

To detect overproduction, the router will detect phase shift between voltage and current.
Simply : 
 - if votage and current are positive or negative at the same time, there is no overproduction
 - if voltage and current are **not** positive of negative at the same time, there is overproduction

### Clock

PV router is based on a relatime clock (editable in **Clock.cpp**) to be independant.
It auto turn off injection after 7PM and turn on after 8AM. It allow to start additionnal heater.

### Temperature sensor

Based on own limit, PV Router will start and stop heating depending of tank target temperature.

### JSON Webservice

A JSON webservice is implemented to be more universal than MQTT.

 - http://[IP]:8080/ is a simple liveness endpoint
 - http://[IP]:8080/data allow to get all data
   - tankTemperature : current tank temperature
   - tankTargetTemperature : max tank temperature
   - triacTemperature : triac temperature
   - triacPosition : triac position (0 - 100%)
   - overProduction : boolean to know if something is injected to heater
   - consumption : grid estimated consumption in kWh
   - tankMode : [0=off, 1=auto_off, 2=on, 3=auto_on], off and on mode are force, auto mode are based on clock
   - powerConnected : detected power from grid
   - instant : instant consumption in W
   - voltage : instant voltage from grid
   - amp : instant current
 - http://[IP]:8080/setTargetTemperature?temperature=XX allow to set maximum tank temperature
 - http://[IP]:8080/setTankMode?mode=[0=off, 1=auto_off, 2=on, 3=auto_on] allow to force mode
 - http://[IP]:8080/restart allow to restart ESP
 - http://[IP]:8080/resetLog clean logs
 - http://[IP]:8080/log get errors logs (causes can be seen in **memory.h**)

### Parameters

All customization are available in **const.h** file.
You must implement a **secret.h** file with your WiFi credentials.
You have to determine your DS18B20 address and put it in TankOneWireTempSensor and TriacOneWireTempSensor.
Finally you have to determine empirical **kV** and **kI** factor.

If your load is at least 2x the solar panel power, you can keep MANAGE_TRIAC_ON_HALF_PERIOD defined.
For example my load is 3kW, my solar panel cannot be more than 1kW. Otherwise, comment this line.

### TODO

 [] Add security with a maximum triac temperature
 


