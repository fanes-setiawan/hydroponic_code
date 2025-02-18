#ifndef SENSOR_MAIN_H
#define SENSOR_MAIN_H

#include <Arduino.h>
#include "sensor_tds_lib.h"
#include "sensor_ph_lib.h"
#include "time_utils.h"
#include "notification.h"
#include "firebase_utils.h"


void checkingSensor(int intervalMinutes);
void saveTimeToEEPROM(unsigned long time);
unsigned long readTimeFromEEPROM();
String getTimestamp(); 

#endif 
