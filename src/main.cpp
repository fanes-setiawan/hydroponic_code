#include <Arduino.h>
#include "wifi_config.h"
#include "firebase_utils.h"
#include "remote_model.h"
#include "schedule_model.h"
#include "sensor_tds.h"



const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0; 

//sensor pin
TdsSensor tdsSensor(34);

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
  float tdsValue = tdsSensor.read();  // Membaca nilai TDS dari sensor
  Serial.print("TDS Value: ");
  Serial.println(tdsValue);  // Menampilkan nilai TDS pada serial monitor

  // RemoteModel remote =  readDataRemoteFromFirestore();
  // ScheduleModel schedule = readDataScheduleFromFirestore();
  // Serial.println("MAIN TIME"+schedule.scheduled_time);
  delay(10000);
}
