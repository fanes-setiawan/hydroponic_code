#include <Arduino.h>
#include "wifi_config.h"
#include "firebase_utils.h"
#include "remote_model.h"
#include "schedule_model.h"
// #include "sensor_tds.h"
#include "sensor_ph.h"
#include "controller.h"
#include "water_controller.h"
#include "sensor_tds_lib.h"
#include "sensor_ph_lib.h"
#include "notification.h"
#include "time_utils.h"


#define RELAY_NUTRISI 2
#define RELAY_PHUP 4
#define RELAY_PHDOWN 16
#define RELAY_WATERPUMP 17
#define RELAY_MIXER 5
#define RELAY_WATER 18

int pH_Value;
float Voltage;


const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0; 

//sensor pin
#define TDS_SENSOR_PIN 35
float waterTemp = 25.0;

Notification notifMain;


void setup()
{
  Serial.begin(115200);
  connectToWiFi();
  int sensorValue = analogRead(34);
  Notification();


  // setupTdsSensor(TDS_PIN); 
  pinMode(RELAY_NUTRISI, OUTPUT);
  pinMode(RELAY_PHUP, OUTPUT);
  pinMode(RELAY_PHDOWN, OUTPUT);
  pinMode(RELAY_WATERPUMP, OUTPUT);
  pinMode(RELAY_MIXER, OUTPUT);
  pinMode(RELAY_WATER, OUTPUT);

  digitalWrite(RELAY_NUTRISI, HIGH);
  digitalWrite(RELAY_PHUP, HIGH);
  digitalWrite(RELAY_PHDOWN, HIGH);
  digitalWrite(RELAY_WATERPUMP, HIGH);
  digitalWrite(RELAY_MIXER, HIGH);
  digitalWrite(RELAY_WATER, HIGH);

  delay(1000);
  setupTdsSensor(TDS_SENSOR_PIN); 
  setupPhSensor();
}

void loop()
{

//lisner
CalibrationTdsModel calibrationTds = readDataCalibrationTdsFromFirestore();
CalibrationPhModel calibrationPh = readDataCalibrationPhFromFirestore();
RemoteModel remote =  readDataRemoteFromFirestore();
ScheduleModel schedule = readDataScheduleFromFirestore();

Serial.print("[REMOTE][CHEKING] :: ");
Serial.println(remote.autoCheck);
String timeNow = getTimestamp();
Serial.println(timeNow);


if(calibrationTds.status == true){
Serial.println("[DR.ROBOT] Kalibrasi TDS: ");
  setupTdsSensor(TDS_SENSOR_PIN); 
}
if(calibrationPh.status == true){
  Serial.println("[DR.ROBOT] Kalibrasi pH: ");
  setupPhSensor();
}
delay(1000);

//read sensor
float tdsValueFiltered = readFilteredTdsValue();
float phValue = readFilteredPhValue();
Serial.print("Nutrisi: ");
Serial.print(tdsValueFiltered);
Serial.println(" ppm");

Serial.print("pH: ");
Serial.print(phValue);
Serial.println(" pH");


// RemoteModel remote =  readDataRemoteFromFirestore();
if(remote.waterPump == true && !pumpRunning){
  pumpRunning = true;
  startWaterPump(RELAY_WATERPUMP);
}else if(remote.waterPump == false && pumpRunning){
  pumpRunning = false;
  stopWaterPump(RELAY_WATERPUMP);
}else{
}
delay(1000);

  if(remote.phDown != 0){
    Serial.print("Data pH DOWN AKAN DI RESTART");
    patchDataRemoteToFirestore("phDown", "0.0");
  }
  // Serial.println("MAIN TIME"+schedule.scheduled_time);
  
}
