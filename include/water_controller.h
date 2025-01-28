#ifndef WATER_CONTROLLER_H
#define WATER_CONTROLLER_H

#include <Arduino.h>

void startWaterPump(int relayPin);
void stopWaterPump(int relayPin);

#endif