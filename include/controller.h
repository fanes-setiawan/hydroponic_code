#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

extern unsigned long pumpStartTime;
extern unsigned long pumpDuration;

// Deklarasi fungsi yang menerima pin sebagai parameter
void startPump(int relayPin , float volume);

#endif
