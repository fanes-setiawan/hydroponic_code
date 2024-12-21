#include <Arduino.h>
#include "wifi_config.h"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println("starting");
  connectToWiFi();
  }

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World");
  delay(1000);
}

