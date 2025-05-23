#include <EEPROM.h>
#include "GravityTDS.h"

#define EEPROM_write(address, p)              \
    {                                         \
        int i = 0;                            \
        byte *pp = (byte *)&(p);              \
        for (; i < sizeof(p); i++)            \
            EEPROM.write(address + i, pp[i]); \
    }
#define EEPROM_read(address, p)               \
    {                                         \
        int i = 0;                            \
        byte *pp = (byte *)&(p);              \
        for (; i < sizeof(p); i++)            \
            pp[i] = EEPROM.read(address + i); \
    }

GravityTDS::GravityTDS()
{
    this->pin = 35;
    this->temperature = 25.0;
    this->aref = 3.3;
    this->adcRange = 4095.0;
    this->kValueAddress = 8;
    this->kValue = 1.0;
}

GravityTDS::~GravityTDS() {}

void GravityTDS::setPin(int pin) { this->pin = pin; }
void GravityTDS::setTemperature(float temp) { this->temperature = temp; }
void GravityTDS::setAref(float value) { this->aref = value; }
void GravityTDS::setAdcRange(float range) { this->adcRange = range; }
void GravityTDS::setKvalueAddress(int address) { this->kValueAddress = address; }

void GravityTDS::begin()
{
    pinMode(this->pin, INPUT);
    readKValues();
}

float GravityTDS::getKvalue() { return this->kValue; }

void GravityTDS::update()
{

    this->analogValue = analogRead(this->pin);
    this->voltage = this->analogValue / this->adcRange * this->aref;
    Serial.print("Analog: ");
    Serial.print(this->analogValue);
    Serial.print(" | Voltage: ");
    Serial.print(this->voltage, 3);
    Serial.print(" | EC: ");
    Serial.print(this->ecValue25, 3);
    Serial.print(" | TDS: ");
    Serial.println(this->tdsValue, 2);
    this->analogValue = analogRead(this->pin);
    this->voltage = this->analogValue / this->adcRange * this->aref;
    this->ecValue = (133.42 * this->voltage * this->voltage * this->voltage - 255.86 * this->voltage * this->voltage + 857.39 * this->voltage) * this->kValue;
    this->ecValue25 = this->ecValue / (1.0 + 0.02 * (this->temperature - 25.0));
    this->tdsValue = ecValue25 * TdsFactor;

    if (cmdSerialDataAvailable() > 0)
    {
        ecCalibration(cmdParse());
    }
}

float GravityTDS::getTdsValue() { return tdsValue; }
float GravityTDS::getEcValue() { return ecValue25; }

void GravityTDS::readKValues()
{
    EEPROM_read(this->kValueAddress, this->kValue);
    if (EEPROM.read(this->kValueAddress) == 0xFF && EEPROM.read(this->kValueAddress + 1) == 0xFF)
    {
        this->kValue = 1.0;
        EEPROM_write(this->kValueAddress, this->kValue);
    }
}

boolean GravityTDS::cmdSerialDataAvailable()
{
    char cmdReceivedChar;
    static unsigned long cmdReceivedTimeOut = millis();
    while (Serial.available() > 0)
    {
        if (millis() - cmdReceivedTimeOut > 500U)
        {
            cmdReceivedBufferIndex = 0;
            memset(cmdReceivedBuffer, 0, (ReceivedBufferLength + 1));
        }
        cmdReceivedTimeOut = millis();
        cmdReceivedChar = Serial.read();
        if (cmdReceivedChar == '\n' || cmdReceivedBufferIndex == ReceivedBufferLength)
        {
            cmdReceivedBufferIndex = 0;
            strupr(cmdReceivedBuffer);
            return true;
        }
        else
        {
            cmdReceivedBuffer[cmdReceivedBufferIndex] = cmdReceivedChar;
            cmdReceivedBufferIndex++;
        }
    }
    return false;
}

byte GravityTDS::cmdParse()
{
    if (strstr(cmdReceivedBuffer, "ENTER") != NULL)
        return 1;
    else if (strstr(cmdReceivedBuffer, "EXIT") != NULL)
        return 3;
    else if (strstr(cmdReceivedBuffer, "CAL:") != NULL)
        return 2;
    return 0;
}

void GravityTDS::ecCalibration(byte mode)
{
    char *cmdReceivedBufferPtr;
    float KValueTemp, rawECsolution;

    switch (mode)
    {
    case 1: // ENTER CALIBRATION MODE
        Serial.println(F(">>>Enter Calibration Mode<<<"));
        break;

    case 2: // CALIBRATE
        cmdReceivedBufferPtr = strstr(cmdReceivedBuffer, "CAL:");
        this->analogValue = analogRead(this->pin);
        this->voltage = this->analogValue / this->adcRange * this->aref;

        if (cmdReceivedBufferPtr)
        {
            cmdReceivedBufferPtr += strlen("CAL:");
            rawECsolution = strtod(cmdReceivedBufferPtr, NULL) / (float)(TdsFactor);
            rawECsolution = rawECsolution * (1.0 + 0.02 * (temperature - 25.0));

            KValueTemp = rawECsolution / (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage);

            if (rawECsolution > 0 && rawECsolution < 2000 && KValueTemp > 0.25 && KValueTemp < 4.0)
            {
                kValue = KValueTemp;
                Serial.print(F(">>>Calibration Confirmed, K Value: "));
                Serial.println(kValue);
            }
            else
            {
                Serial.println(F(">>>Calibration Failed, Invalid Solution or K Value<<<"));
            }
        }
        break;

    case 3: // EXIT AND SAVE
        if (kValue >= 0.25 && kValue <= 4.0)
        {
            EEPROM_write(kValueAddress, kValue);
            Serial.println(F(">>>Calibration Complete, K Value Saved<<<"));
        }
        else
        {
            Serial.println(F(">>>Calibration Aborted, Invalid K Value Not Saved<<<"));
        }
        break;
    }
}
