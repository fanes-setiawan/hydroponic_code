#include <Arduino.h>
#include "wifi_config.h"
#include "firebase_utils.h"
#include "remote_model.h"
#include "schedule_model.h"
#include "sensor_tds.h"
#include "sensor_ph.h"
#include "controller.h"
#include "water_controller.h"
#include "sensor_tds_lib.h"


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

#define TDS_PIN 35
#define TEMPERATURE 25

PhSensor phSensor(35); 


void setup()
{
  Serial.begin(115200);
  Serial.println("starting");
  int sensorValue = analogRead(34); 
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue); 
  delay(500);
  setupTdsSensor(TDS_PIN); 
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

  connectToWiFi();
  delay(1000);
  setupTdsSensor(TDS_SENSOR_PIN, waterTemp); 
  // startPump(RELAY_WATER);
  // startPump(RELAY_NUTRISI);

}

void loop()
{
CalibrationTdsModel calibrationTds = readDataCalibrationTdsFromFirestore();
if(calibrationTds.status == true){
  setupTdsSensor(TDS_SENSOR_PIN, waterTemp); 
}

//ph
loopTdsSensor(TEMPERATURE);
float tdsValue = readTdsValue();
Serial.print("[DR.ROBOT] TDS Value: ");
Serial.print(tdsValue);
Serial.println(" ppm");

//............
  // Serial.println("looping");
  // Serial.print(pumpDuration);
  // digitalWrite(RELAY_NUTRISI, HIGH); 
  // digitalWrite(RELAY_PHUP, HIGH);
  // Serial.print("Relay NUTRISI ON");
  // delay(1000);
  // digitalWrite(RELAY_PHUP, LOW);
  // Serial.print("Relay NUTRISI OFF");
  // delay(1000);




  // float tdsValue = tdsSensor.read();
  // float phValue = phSensor.read();

  //tds
  // Serial.print("TDS Value: ");
  // Serial.println(tdsValue); 
  // delay(1000);
  

  //ph
  // Serial.print("Nilai pH: ");
  // Serial.println(phValue);

  RemoteModel remote =  readDataRemoteFromFirestore();
  if(remote.waterPump == true && !pumpRunning){
    pumpRunning = true;
    startWaterPump(RELAY_WATERPUMP);
  }else if(remote.waterPump == false && pumpRunning){
    pumpRunning = false;
    stopWaterPump(RELAY_WATERPUMP);
  }else{
  }

  if(remote.phDown != 0){
    Serial.print("Data pH DOWN AKAN DI RESTART");
    patchDataRemoteToFirestore("phDown", "0.0");
  }
  // ScheduleModel schedule = readDataScheduleFromFirestore();
  // Serial.println("MAIN TIME"+schedule.scheduled_time);
  
}
