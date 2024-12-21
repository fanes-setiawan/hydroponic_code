#include <Arduino.h>
#include "wifi_config.h"
#include "firebase_utils.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("starting");
  connectToWiFi();
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.println("looping");
  // sendDataPhToFirestore(99.0);
  // sendDataTdsToFirestore(99.0);
  delay(1000);
}
