#ifndef FIREBASE_UTILS_H
#define FIREBASE_UTILS_H

#include <Firebase_ESP_Client.h>
#include <WiFi.h>
#include "credentials.h"

// Deklarasi Firebase variabel
extern FirebaseData firebaseData;
extern FirebaseAuth auth;
extern FirebaseConfig config;
extern FirebaseJson firebaseJson;


void connectToFirebase();
void sendDataPhToFirestore(float phLevel);
void sendDataTdsToFirestore(float tdsLevel);

#endif 
