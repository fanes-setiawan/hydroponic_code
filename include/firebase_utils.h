#ifndef FIREBASE_UTILS_H
#define FIREBASE_UTILS_H

#include <Firebase_ESP_Client.h>
#include <Arduino.h>
#include <WiFi.h>
#include "credentials.h"
#include "remote_model.h"
#include "schedule_model.h"

// Deklarasi variabel Firebase
extern FirebaseData firebaseData;
extern FirebaseAuth auth;
extern FirebaseConfig config;
extern FirebaseJson firebaseJson;

// Fungsi untuk Firebase
void connectToFirebase();
void sendDataPhToFirestore(float phLevel);
void sendDataTdsToFirestore(float tdsLevel);
RemoteModel readDataRemoteFromFirestore();
ScheduleModel readDataScheduleFromFirestore();

#endif
