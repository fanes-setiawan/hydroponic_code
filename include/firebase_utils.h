#ifndef FIREBASE_UTILS_H
#define FIREBASE_UTILS_H

#include <Firebase_ESP_Client.h>
#include <Arduino.h>
#include <WiFi.h>
#include "credentials.h"
#include "remote_model.h"
#include "schedule_model.h"
#include "calibration_tds_model.h"
#include "calibration_ph_model.h"

// Deklarasi variabel Firebase
extern FirebaseData firebaseData;
extern FirebaseAuth auth;
extern FirebaseConfig config;
extern FirebaseJson firebaseJson;

// Fungsi untuk Firebase
void connectToFirebase();
void sendDataPhToFirestore(float phLevel);
void sendDataTdsToFirestore(float tdsLevel);
void patchDataRemoteToFirestore(const String &field,const String &value);
void patchDataCalibrationTdsToFirestore(const String &field,const String &value);
void patchDataCalibrationPhToFirestore(const String &field,const String &value);
RemoteModel readDataRemoteFromFirestore();
ScheduleModel readDataScheduleFromFirestore();
CalibrationTdsModel readDataCalibrationTdsFromFirestore();
CalibrationPhModel readDataCalibrationPhFromFirestore();


#endif
