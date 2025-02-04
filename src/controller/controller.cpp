#include <Arduino.h>
#include "controller.h"

#define FLOW_RATE 1250.0    
#define DESIRED_VOLUME 80.0

bool pumpRunning = false;
unsigned long pumpStartTime = 0;
unsigned long pumpDuration = 0;

void startPump(int relayPin) {
    pumpDuration = (DESIRED_VOLUME / FLOW_RATE) * 60.0 * 1000.0; 
    pumpStartTime = millis();  

    Serial.print("Menambahkan air sebanyak: ");
    Serial.print(DESIRED_VOLUME);
    Serial.println(" mL");

    Serial.print("Pompa menyala selama: ");
    Serial.print(pumpDuration / 1000.0);
    Serial.println(" detik");

    digitalWrite(relayPin, LOW); 
    pumpRunning = true;
    delay(pumpDuration);             
    digitalWrite(relayPin, HIGH);  
}
