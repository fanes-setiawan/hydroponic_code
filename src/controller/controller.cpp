#include <Arduino.h>
#include "controller.h"

#define FLOW_RATE 2150.0    

unsigned long pumpStartTime = 0;
unsigned long pumpDuration = 0;

void startPump(int relayPin , float volume) {
    pumpDuration = (volume / FLOW_RATE) * 60.0 * 1000.0; 
    pumpStartTime = millis();  

    Serial.print("Menambahkan air sebanyak: ");
    Serial.print(volume);
    Serial.println(" mL");

    Serial.print("Pompa menyala selama: ");
    Serial.print(pumpDuration / 1000.0);
    Serial.println(" detik");

    digitalWrite(relayPin, LOW); 
    delay(pumpDuration);             
    digitalWrite(relayPin, HIGH);  
    delay(1000);
    runMixer10s();
}
