#include <Arduino.h>
#include "water_controller.h"


void startWaterPump(int relayPin){
  digitalWrite(relayPin, LOW);
}

void stopWaterPump(int relayPin){
  digitalWrite(relayPin, HIGH);
}