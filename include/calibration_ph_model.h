#ifndef CALIBRATION_PH_MODEL_H
#define CALIBRATION_PH_MODEL_H

#include <Arduino.h>
#include <FirebaseJson.h>

class CalibrationPhModel {
public:
    float fluid_ph;
    bool status;
    String updateAt;

    // Constructor
    CalibrationPhModel(
        float fluid_ph,
        bool status,
        String updateAt
    ) {
        this->fluid_ph = fluid_ph;
        this->status = status;
        this->updateAt = updateAt;
    }

    // Default Constructor
    CalibrationPhModel() : CalibrationPhModel(0.0, false, "") {}
};

#endif
