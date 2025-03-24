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
#include "generate.h"

int pH_Value;
float Voltage;
float waterTemp = 25.0;

Notification notifMain;

unsigned long lastCheckTime = 0;
const unsigned long scheduleInterval = 40000;
unsigned long lastFirebaseReadTime = 0;
const unsigned long firebaseReadInterval = 5000;

RemoteModel remote;
CalibrationTdsModel calibrationTds;
CalibrationPhModel calibrationPh;

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
  setupTdsSensor(TDS_SENSOR_PIN);
  setupPhSensor();
  setupTime();
  notifMain.sendNotification("mulai", "Sistem hydroponik dimulai");
}

void loop()
{
  // Baca data dari Firestore hanya setiap 5 detik
  if (millis() - lastFirebaseReadTime >= firebaseReadInterval)
  {
    calibrationTds = readDataCalibrationTdsFromFirestore();
    calibrationPh = readDataCalibrationPhFromFirestore();
    remote = readDataRemoteFromFirestore();
    lastFirebaseReadTime = millis();
  }

  // Jalankan jadwal hanya setiap interval tertentu
  if (millis() - lastCheckTime >= scheduleInterval)
  {
    checkingSchedule();
    lastCheckTime = millis();
  }

  // Mode otomatis
  if (remote.autoMode)
  {
    checkingSensor(remote.autoCheck);
    Serial.println("[DR.ROBOT] Auto Mode: ON");
  }
  else
  {
    checkingSensor(30);
    Serial.println("[DR.ROBOT] Auto Mode: OFF");
  }

  // Kalibrasi sensor hanya jika diperlukan
  if (calibrationTds.status)
  {
    Serial.println("[DR.ROBOT] Kalibrasi TDS: ");
    setupTdsSensor(TDS_SENSOR_PIN);
  }
  if (calibrationPh.status)
  {
    Serial.println("[DR.ROBOT] Kalibrasi pH: ");
    setupPhSensor();
  }
  if (remote.isRemove)
  {
    Serial.println("[DR.ROBOT] Remove: ");
    phSensor.resetEEPROM();
    patchDataRemoteToFirestore("isRemove", "false");
  }

  // Kontrol larutan manual hanya jika ada perubahan
  if (remote.water > 0.0)
  {
    startPump(RELAY_WATER, remote.water);
    patchDataRemoteToFirestore("water", "0.0");
    tdsLevel = calculateTDSLevel(tdsLevel, DOWN);
  }
  if (remote.phDown > 0.0)
  {
    startPump(RELAY_PHDOWN, remote.phDown);
    patchDataRemoteToFirestore("phDown", "0.0");
  }
  if (remote.phUp > 0.0)
  {
    startPump(RELAY_PHUP, remote.phUp);
    patchDataRemoteToFirestore("phUp", "0.0");
  }
  if (remote.nutrisi > 0.0)
  {
    startPump(RELAY_NUTRISI, remote.nutrisi);
    patchDataRemoteToFirestore("nutrisi", "0.0");
    tdsLevel = calculateTDSLevel(tdsLevel, UP);
  }

  // Kontrol pompa air dan mixer
  if (remote.waterPump)
    runWaterPump();
  else
    stopWaterPump();

  if (remote.mixer)
    runMixer();
  else
    stopMixer();
  delay(100);
}
