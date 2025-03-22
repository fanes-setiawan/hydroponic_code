#include <Arduino.h>
#include "controller_mixer.h"

void runMixer(){
    digitalWrite(RELAY_MIXER,  LOW);
}



void stopMixer(){
    digitalWrite(RELAY_MIXER,  HIGH);
}

void runWaterPump(){
    digitalWrite(RELAY_WATERPUMP,  LOW);
}
void stopWaterPump(){
    digitalWrite(RELAY_WATERPUMP,  HIGH);
}

void runMixer10s(){
    digitalWrite(RELAY_MIXER,  LOW);
    delay(2500);
    digitalWrite(RELAY_MIXER,  HIGH);
}