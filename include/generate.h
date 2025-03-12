#ifndef GENERATE_H
#define GENERATE_H
#include <Arduino.h>

enum TDSAdjustmentMode {
    NORMAL,
    UP,
    DOWN
};

int getTDSLevel(int lowerBound, int upperBound);
float calculateTDSLevel(float currentTDS, TDSAdjustmentMode mode);
int getTDSLevel(int lowerBound, int upperBound);
#endif