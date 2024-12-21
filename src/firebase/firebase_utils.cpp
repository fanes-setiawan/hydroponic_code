#include "firebase_utils.h"
#include "credentials.h"
#include "time_utils.h"

// Inisialisasi variabel Firebase
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Fungsi untuk menghubungkan ke Firebase
void connectToFirebase() {
  Serial.println("Connecting to Firebase...");
  config.api_key = FIREBASE_API_KEY;
  config.host = FIREBASE_HOST;

  auth.user.email = AUTH_EMAIL;
  auth.user.password = AUTH_PASSWORD;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);
}

//post data Ph to firebase
void sendDataPhToFirestore(float phLevel) {
  FirebaseJson json;

  // Menambahkan field ke JSON
  json.set(FIREBASE_PH_VL_FIELD, String((int)phLevel));
  json.set(FIREBASE_PH_TM_FIELD, getTimestamp());

  // Menampilkan data JSON
  Serial.println(json.raw());

  // Mengirim data ke Firestore
  if (Firebase.Firestore.createDocument(&firebaseData, FIREBASE_PROJECT_ID, "", FIREBASE_PH_COLLECTION, json.raw())) {
    Serial.println("Data sent successfully!");
    Serial.println(firebaseData.payload());
  } else {
    Serial.println("Failed to send data!");
    Serial.println(firebaseData.errorReason());
  }
}

//post data Tds to firebase
void sendDataTdsToFirestore(float tdsLevel) {
  FirebaseJson json;

  // Menambahkan field ke JSON
  json.set(FIREBASE_TDS_VL_FIELD, String((int)tdsLevel));
  json.set(FIREBASE_TDS_TM_FIELD, getTimestamp());

  // Menampilkan data JSON
  Serial.println(json.raw());

  // Mengirim data ke Firestore
  if (Firebase.Firestore.createDocument(&firebaseData, FIREBASE_PROJECT_ID, "", FIREBASE_TDS_COLLECTION, json.raw())) {
    Serial.println("Data sent successfully!");
    Serial.println(firebaseData.payload());
  } else {
    Serial.println("Failed to send data!");
    Serial.println(firebaseData.errorReason());
  }
}

