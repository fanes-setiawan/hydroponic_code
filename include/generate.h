#ifndef GENERATE_H
#define GENERATE_H
#include <Arduino.h>



enum AdjustmentMode{
    NORMAL,
    UP,
    DOWN
};

int getTDSLevel(int lowerBound, int upperBound);
float calculateTDSLevel(float currentTDS, AdjustmentMode mode);
float calculatePHLevel(float currentPH, AdjustmentMode mode);
#endif