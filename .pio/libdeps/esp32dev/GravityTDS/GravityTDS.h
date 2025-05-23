#ifndef GRAVITY_TDS_H
#define GRAVITY_TDS_H

#include "Arduino.h"

#define ReceivedBufferLength 15
#define TdsFactor 0.7

class GravityTDS {
public:
    GravityTDS();
    ~GravityTDS();

    void begin();
    void update();
    void setPin(int pin);
    void setTemperature(float temp);
    void setAref(float value);
    void setAdcRange(float range);
    void setKvalueAddress(int address);
    float getKvalue();
    float getTdsValue();
    float getEcValue();
    
    void ecCalibration(byte mode);
    char cmdReceivedBuffer[ReceivedBufferLength + 1]; // Make this public for direct access

private:
    int pin;
    float aref;
    float adcRange;
    float temperature;
    int kValueAddress;
    byte cmdReceivedBufferIndex;

    float kValue;
    float analogValue;
    float voltage;
    float ecValue;
    float ecValue25;
    float tdsValue;

    void readKValues();
    boolean cmdSerialDataAvailable();
    byte cmdParse();
};

#endif
