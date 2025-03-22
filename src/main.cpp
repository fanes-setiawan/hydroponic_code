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
#include "pins.h"
#include "documentation.h"
#include "controller_mixer.h"
#include "sensor_main.h"
#include "generate.h"

int pH_Value;
float Voltage;

const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

float waterTemp = 25.0;

Notification notifMain;

unsigned long lastCheckTime = 0;
const unsigned long scheduleInterval = 40000;

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
  if (millis() - lastCheckTime >= scheduleInterval) {  
    checkingSchedule();
    lastCheckTime = millis();
  }

  if (remote.autoMode == true)
  {
    checkingSensor(remote.autoCheck);
    Serial.println("[DR.ROBOT] Auto Mode: ON");
  }
  else
  {
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
  if (remote.isRemove == true)
  {
    Serial.println("[DR.ROBOT] Remove: ");
    delay(1000);
    phSensor.resetEEPROM();
    patchDataRemoteToFirestore("isRemove", "false");
  }

  // larutan manual
  if ((double)remote.water != 0.0)
  {
    startPump(RELAY_WATER, remote.water);
    patchDataRemoteToFirestore("water", "0.0");
    tdsLevel = calculateTDSLevel(tdsLevel, DOWN);
    Serial.print("[MAIN][DOWN]TDS Level: ");
    Serial.println(tdsLevel);
  }
  if ((double)remote.phDown != 0.0)
  {
    startPump(RELAY_PHDOWN, remote.phDown);
    patchDataRemoteToFirestore("phDown", "0.0");
  }
  if ((double)remote.phUp != 0.0)
  {
    startPump(RELAY_PHUP, remote.phUp);
    patchDataRemoteToFirestore("phUp", "0.0");
  }
  if ((double)remote.nutrisi != 0.0)
  {
    startPump(RELAY_NUTRISI, remote.nutrisi);
    patchDataRemoteToFirestore("nutrisi", "0.0");
    tdsLevel = calculateTDSLevel(tdsLevel, UP);
    Serial.print("[MAIN][UP]TDS Level: ");
    Serial.println(tdsLevel);
  }
  if (remote.waterPump == true)
  {
    runWaterPump();
  }
  if (remote.waterPump == false)
  {
    stopWaterPump();
  }

  if (remote.mixer == true)
  {
    runMixer();
  }
  if (remote.mixer == false)
  {
    stopMixer();
  }
}
