#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <Arduino.h>
#include "sensor_main.h"
#include "controller.h"
#include "pins.h"
#include "notification.h"
#include "documentation.h"
#include "controller_mixer.h"
#include "notification.h"
#include "generate.h"

void automationPH(float phValue,float tdsValue);
float normalizeTDS(float phValue, float currentTDS);
float calculateVolumeToAdd(float currentTDS);
float calculateWaterFilter(float currentTDS);
void checkStatus();
extern dataExcel oldStatusExcel;
extern dataExcel dataEx;
#endif