#ifndef _DFROBOT_PH_H_
#define _DFROBOT_PH_H_

#include "Arduino.h"

#define ReceivedBufferLength 10
#define PHVALUEADDR 0x00
#define PH7VALUEADDR   0x04  
#define CALIBRATION_FLAG_ADDR 0x10  

class DFRobot_PH {
public:
    DFRobot_PH();
    ~DFRobot_PH();
    void resetEEPROM();

    void begin();
    float readPH(float voltage);
    void calibration(float voltage, float temperature, char* cmd);  // Original Serial-Based Calibration
    void calibration(float voltage, float temperature);             // Original Serial-Based Calibration
    void calibration(float voltage, float calibrationValue, const char* type);  // ✅ New Auto Calibration for ESP32

private:
    float _phValue;
    float _acidVoltage;
    float _neutralVoltage;
    float _voltage;
    float _temperature;

    char _cmdReceivedBuffer[ReceivedBufferLength];
    byte _cmdReceivedBufferIndex;

    boolean cmdSerialDataAvailable();
    void phCalibration(byte mode);
    byte cmdParse(const char* cmd);
    byte cmdParse();
    char* strupr(char* str);
};

#endif
