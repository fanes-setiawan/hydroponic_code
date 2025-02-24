#ifndef SENSOR_PH_LIB_H
#define SENSOR_PH_LIB_H

#include "DFRobot_PH.h"
#include <EEPROM.h>
#include "calibration_ph_model.h"
#include "notification.h"

#define CALIBRATION_FLAG_ADDR 0
#define CALIBRATION_VALUE_ADDR sizeof(int)
#define PH_PIN 34  // Pin ADC untuk sensor pH

void setupPhSensor();
void calibratePhSensor(float voltage, float calibrationValue, const char* type);
float readFilteredPhValue();
float readPhValue();
float getMedianValues(float data[], int len);
extern Notification notificationPH;

#endif
