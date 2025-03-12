#include <Arduino.h>
#include "controller_mixer.h"

void runMixer(){
    digitalWrite(RELAY_MIXER,  LOW);
}

void stopMixer(){
    digitalWrite(RELAY_MIXER,  HIGH);
}

void runMixer10s(){
    digitalWrite(RELAY_MIXER,  LOW);
    delay(10000);
    digitalWrite(RELAY_MIXER,  HIGH);
}