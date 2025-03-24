#ifndef SENSOR_MAIN_H
#define SENSOR_MAIN_H

#include <Arduino.h>
#include "sensor_tds_lib.h"
#include "sensor_ph_lib.h"
#include "time_utils.h"
#include "notification.h"
#include "firebase_utils.h"
#include "automation.h"
#include "generate.h"
#include "schedule_run.h"
#include <vector>

void checkingSensor(int intervalMinutes);
void saveTimeToEEPROM(unsigned long time);
unsigned long readTimeFromEEPROM();
String getTimestamp(); 
void updatePhBuffer(float newValue); 
float getFilteredPhValue();
float readStablePhValue();
bool isPhStable(); 
float mostFrequentValue(const std::vector<float>& values);

extern float tdsLevel;

#endif 
