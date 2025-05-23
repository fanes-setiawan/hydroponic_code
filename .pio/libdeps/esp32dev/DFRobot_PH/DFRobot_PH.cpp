#include "DFRobot_PH.h"
#include <EEPROM.h>

#define EEPROM_write(address, p) { int i = 0; byte *pp = (byte*)&(p); for (; i < sizeof(p); i++) EEPROM.write(address + i, pp[i]); }
#define EEPROM_read(address, p)  { int i = 0; byte *pp = (byte*)&(p); for (; i < sizeof(p); i++) pp[i] = EEPROM.read(address + i); }

char* DFRobot_PH::strupr(char* str) {
    if (str == NULL) return NULL;
    char* ptr = str;
    while (*ptr != ' ') {
        *ptr = toupper((unsigned char)*ptr);
        ptr++;
    }
    return str;
}

DFRobot_PH::DFRobot_PH() {
    this->_temperature = 25.0;
    this->_phValue = 7.0;
    this->_acidVoltage = 2032.44;    // pH 4.0 at 25°C
    this->_neutralVoltage = 1500.0;  // pH 7.0 at 25°C
    this->_voltage = 1500.0;
}

DFRobot_PH::~DFRobot_PH() {}

void DFRobot_PH::resetEEPROM() {
    // Reset pH 7 calibration to default (1500.0 mV)
    this->_neutralVoltage = 1500.0;
    EEPROM_write(PHVALUEADDR, this->_neutralVoltage);

    // Reset pH 4 calibration to default (2032.44 mV)
    this->_acidVoltage = 2032.44;
    EEPROM_write(PHVALUEADDR + 4, this->_acidVoltage);

    EEPROM.commit();

    Serial.println(F("📢 EEPROM Calibration Data Reset to Default."));
}

void DFRobot_PH::begin() {
    EEPROM_read(PHVALUEADDR, this->_neutralVoltage);
    if (EEPROM.read(PHVALUEADDR) == 0xFF) {
        this->_neutralVoltage = 1500.0;
        EEPROM_write(PHVALUEADDR, this->_neutralVoltage);
    }

    EEPROM_read(PHVALUEADDR + 4, this->_acidVoltage);
    if (EEPROM.read(PHVALUEADDR + 4) == 0xFF) {
        this->_acidVoltage = 2032.44;
        EEPROM_write(PHVALUEADDR + 4, this->_acidVoltage);
    }
}

float DFRobot_PH::readPH(float voltage) {
    float slope = (7.0 - 4.0) / ((this->_neutralVoltage - 1500.0) / 3.0 - (this->_acidVoltage - 1500.0) / 3.0);
    float intercept = 7.0 - slope * (this->_neutralVoltage - 1500.0) / 3.0;
    this->_phValue = slope * (voltage - 1500.0) / 3.0 + intercept;
    return _phValue;
}

// ✅ New Auto Calibration for ESP32
void DFRobot_PH::calibration(float voltage, float calibrationValue, const char* type) {
    this->_voltage = voltage;

    if (strcmp(type, "CALIBRATE pH4") == 0) {
        this->_acidVoltage = voltage;
        EEPROM_write(PHVALUEADDR + 4, this->_acidVoltage);
        Serial.println(F("✅ pH 4.0 Calibration Saved to EEPROM"));
    } 
    else if (strcmp(type, "CALIBRATE pH7") == 0) {
        this->_neutralVoltage = voltage;
        EEPROM_write(PHVALUEADDR, this->_neutralVoltage);
        Serial.println(F("✅ pH 7.0 Calibration Saved to EEPROM"));
    } 
    else {
        Serial.println(F("⚠️ Error: Unknown Calibration Type"));
        return;
    }

    EEPROM.commit();
    Serial.println(F("✅ Calibration Complete and Data Saved"));
}

// Original Serial-based calibration methods remain unchanged for compatibility
void DFRobot_PH::calibration(float voltage, float temperature, char* cmd) {
    this->_voltage = voltage;
    this->_temperature = temperature;
    String sCmd = String(cmd);
    sCmd.toUpperCase();
    phCalibration(cmdParse(sCmd.c_str()));
}

void DFRobot_PH::calibration(float voltage, float temperature) {
    this->_voltage = voltage;
    this->_temperature = temperature;
    if (cmdSerialDataAvailable() > 0) {
        phCalibration(cmdParse());
    }
}

boolean DFRobot_PH::cmdSerialDataAvailable() {
    char cmdReceivedChar;
    static unsigned long cmdReceivedTimeOut = millis();
    while (Serial.available() > 0) {
        if (millis() - cmdReceivedTimeOut > 500U) {
            this->_cmdReceivedBufferIndex = 0;
            memset(this->_cmdReceivedBuffer, 0, (ReceivedBufferLength));
        }
        cmdReceivedTimeOut = millis();
        cmdReceivedChar = Serial.read();
        if (cmdReceivedChar == '\n' || this->_cmdReceivedBufferIndex == ReceivedBufferLength - 1) {
            this->_cmdReceivedBufferIndex = 0;
            strupr(this->_cmdReceivedBuffer);
            return true;
        } else {
            this->_cmdReceivedBuffer[this->_cmdReceivedBufferIndex] = cmdReceivedChar;
            this->_cmdReceivedBufferIndex++;
        }
    }
    return false;
}

byte DFRobot_PH::cmdParse(const char* cmd) {
    byte modeIndex = 0;
    if (strstr(cmd, "ENTERPH") != NULL) modeIndex = 1;
    else if (strstr(cmd, "EXITPH") != NULL) modeIndex = 3;
    else if (strstr(cmd, "CALPH") != NULL) modeIndex = 2;
    return modeIndex;
}

byte DFRobot_PH::cmdParse() {
    return cmdParse(this->_cmdReceivedBuffer);
}

void DFRobot_PH::phCalibration(byte mode) {
    char* receivedBufferPtr;
    static boolean phCalibrationFinish = 0;
    static boolean enterCalibrationFlag = 0;

    switch (mode) {
        case 0:
            if (enterCalibrationFlag) Serial.println(F(">>>Command Error<<<"));
            break;

        case 1:
            enterCalibrationFlag = 1;
            phCalibrationFinish = 0;
            Serial.println(F(">>>Enter PH Calibration Mode<<<"));
            break;

        case 2:
            if (enterCalibrationFlag) {
                if (this->_voltage > 1322 && this->_voltage < 1678) {  // pH 7
                    this->_neutralVoltage = this->_voltage;
                    phCalibrationFinish = 1;
                    Serial.println(F(">>>Buffer Solution: 7.0<<<"));
                } else if (this->_voltage > 1854 && this->_voltage < 2210) {  // pH 4
                    this->_acidVoltage = this->_voltage;
                    phCalibrationFinish = 1;
                    Serial.println(F(">>>Buffer Solution: 4.0<<<"));
                } else {
                    Serial.println(F(">>>Buffer Solution Error, Try Again<<<"));
                    phCalibrationFinish = 0;
                }
            }
            break;

        case 3:
            if (enterCalibrationFlag) {
                if (phCalibrationFinish) {
                    if (this->_voltage > 1322 && this->_voltage < 1678) {
                        EEPROM_write(PHVALUEADDR, this->_neutralVoltage);
                    } else if (this->_voltage > 1854 && this->_voltage < 2210) {
                        EEPROM_write(PHVALUEADDR + 4, this->_acidVoltage);
                    }
                    Serial.println(F(">>>Calibration Successful<<<"));
                } else {
                    Serial.println(F(">>>Calibration Failed<<<"));
                }
                EEPROM.commit();
                enterCalibrationFlag = 0;
                phCalibrationFinish = 0;
            }
            break;
    }
}
