#include <Arduino.h>
#include "wifi_config.h"
#include "firebase_utils.h"
#include "remote_model.h"
#include "schedule_model.h"
#include "sensor_tds.h"
#include "sensor_ph.h"



const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0; 

//sensor pin
TdsSensor tdsSensor(34);
PhSensor phSensor(35); 

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("starting");
  connectToWiFi();
}

void loop()
{
  Serial.println("looping");
  float tdsValue = tdsSensor.read();
  float phValue = phSensor.read();  // Membaca nilai pH

  //tds
  Serial.print("TDS Value: ");
  Serial.println(tdsValue); 
  //ph
  Serial.print("Nilai pH: ");
  Serial.println(phValue);
  // RemoteModel remote =  readDataRemoteFromFirestore();
  // ScheduleModel schedule = readDataScheduleFromFirestore();
  // Serial.println("MAIN TIME"+schedule.scheduled_time);
  delay(10000);
}
