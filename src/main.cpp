#include <Arduino.h>
#include "wifi_config.h"
#include "firebase_utils.h"
#include "remote_model.h"
#include "schedule_model.h"
#include "controller.h"
#include "water_controller.h"
#include "sensor_tds_lib.h"
#include "sensor_ph_lib.h"
#include "notification.h"
#include "time_utils.h"
#include "sensor_main.h"
#include "schedule_main.h"

#define RELAY_NUTRISI 18
#define RELAY_PHUP 2
#define RELAY_PHDOWN 5
#define RELAY_WATERPUMP 17
#define RELAY_MIXER 16
#define RELAY_WATER 4

#define TDS_SENSOR_PIN 35
#define TXS_OE 22

int pH_Value;
float Voltage;

const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

// sensor pin
float waterTemp = 25.0;

Notification notifMain;

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_NUTRISI, OUTPUT);
  pinMode(RELAY_PHUP, OUTPUT);
  pinMode(RELAY_PHDOWN, OUTPUT);
  pinMode(RELAY_WATERPUMP, OUTPUT);
  pinMode(RELAY_MIXER, OUTPUT);
  pinMode(RELAY_WATER, OUTPUT);
  pinMode(TXS_OE, OUTPUT);
  digitalWrite(RELAY_NUTRISI, HIGH);
  digitalWrite(RELAY_PHUP, HIGH);
  digitalWrite(RELAY_PHDOWN, HIGH);
  digitalWrite(RELAY_WATERPUMP, HIGH);
  digitalWrite(RELAY_MIXER, HIGH);
  digitalWrite(RELAY_WATER, HIGH);
  digitalWrite(TXS_OE, HIGH);
  

  connectToWiFi();
  int sensorValue = analogRead(34);
  Notification();

  setupTdsSensor(TDS_SENSOR_PIN);
  setupPhSensor();
  setupTime();
  notification.sendNotification("mulai", "sistem hydroponik dimulai");
}

void loop()
{
  CalibrationTdsModel calibrationTds = readDataCalibrationTdsFromFirestore();
  CalibrationPhModel calibrationPh = readDataCalibrationPhFromFirestore();
  RemoteModel remote = readDataRemoteFromFirestore();

  // checkingSchedule();

  if (remote.autoMode == true)
  {
    checkingSensor(remote.autoCheck);
    Serial.println("[DR.ROBOT] Auto Mode: ON");
  }else{
    checkingSensor(30);
    Serial.println("[DR.ROBOT] Auto Mode: OFF");
    }

  if (calibrationTds.status == true)
  {
    Serial.println("[DR.ROBOT] Kalibrasi TDS: ");
    setupTdsSensor(TDS_SENSOR_PIN);
  }
  if (calibrationPh.status == true)
  {
    Serial.println("[DR.ROBOT] Kalibrasi pH: ");
    delay(1000);
    setupPhSensor();
  }

  // // read sensor
  // float tdsValueFiltered = readFilteredTdsValue();
  // Serial.print("Nutrisi: ");
  // Serial.print(tdsValueFiltered);
  // Serial.println(" ppm");

  if(remote.autoMode){
    float phValue = readFilteredPhValue();
    Serial.print("------------>pH: ");
    Serial.print(phValue);
    Serial.println(" pH");
  }
  

  // // RemoteModel remote =  readDataRemoteFromFirestore();
  // // digitalWrite(RELAY_WATERPUMP, remote.waterPump ? LOW : HIGH);
  // digitalWrite(RELAY_MIXER, remote.mixer ? LOW : HIGH);
  // if(remote.waterPump == true){
  //   startPump(RELAY_WATER,100);
  // }
  // if(remote.mixer == true){
  //   startPump(RELAY_NUTRISI,100);
  // }


  // delay(1000);

  // // if (remote.phDown != 0)
  // // {
  // //   Serial.print("Data pH DOWN AKAN DI RESTART");
  // //   patchDataRemoteToFirestore("phDown", "0.0");
  // // }
}
