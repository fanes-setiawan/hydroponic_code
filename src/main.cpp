#include <Arduino.h>
#include "wifi_config.h"
#include "firebase_utils.h"
#include "remote_model.h"


RemoteModel remoteModel = RemoteModel();

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
  RemoteModel remote =  readDataRemoteFromFirestore();
  delay(1000);
}
