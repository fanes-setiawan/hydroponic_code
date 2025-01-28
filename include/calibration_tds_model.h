#ifndef CALIBRATION_TDS_MODEL_H
#define CALIBRATION_TDS_MODEL_H

#include <Arduino.h>
#include <FirebaseJson.h>

class CalibrationTdsModel {
public:
    float fluid_ppm;
    bool status;
    String updateAt;

    // Constructor
    CalibrationTdsModel(
        float fluid_ppm,
        bool status,
        String updateAt
    ) {
        this->fluid_ppm = fluid_ppm;
        this->status = status;
        this->updateAt = updateAt;
    }

    // Default Constructor
    CalibrationTdsModel() : CalibrationTdsModel(0.0, false, "") {}
};

#endif
