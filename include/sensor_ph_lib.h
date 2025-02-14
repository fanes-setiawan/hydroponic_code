#ifndef SENSOR_PH_LIB_H
#define SENSOR_PH_LIB_H

#include "DFRobot_PH.h"
#include <EEPROM.h>
#include "calibration_ph_model.h"
#include "notification.h"

void setupPhSensor();
CalibrationPhModel readDataCalibrationPhFromFirestore();
float readPhValue();
void calibratePhSensor(float calibrationValue, const char* typ);
float readFilteredPhValue();
float getMedianValues(float data[], int len);
extern Notification notification;

// Namespace opsional untuk menghindari konflik
namespace PhSensors{
    extern DFRobot_PH phSensor;
    extern float waterTemperature;
    
}

#endif