#include <Arduino.h>
#include "wifi_config.h"
#include "firebase_utils.h"
#include "remote_model.h"
#include "schedule_model.h"


RemoteModel remoteModel = RemoteModel();
ScheduleModel scheduleModel = ScheduleModel();

const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0; 


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
  // ScheduleModel schedule = readDataScheduleFromFirestore();
  // Serial.println("MAIN TIME"+schedule.scheduled_time);
  delay(10000);
}
