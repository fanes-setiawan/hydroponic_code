#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// WiFi credentials
#define WIFI_SSID "Tselhome-0A06"
#define WIFI_PASSWORD "44226533"

// #define WIFI_SSID "Carakan-Developer"
// #define WIFI_PASSWORD "2wsx1qaz"

// #define WIFI_SSID "KONTRAKKAN 01"
// #define WIFI_PASSWORD "KEPOAMAT"


// Firebase credentials
#define FIREBASE_HOST "hydsmartfire.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyCBD0g2s5pAVGQ-Fue464ZgyHZRDtdioj4"
#define FIREBASE_PROJECT_ID "hydsmartfire"

// Email/password Firebase Auth
#define AUTH_EMAIL "admin@gmail.com"
#define AUTH_PASSWORD "2wsx1qaz"

//fields for the database

//ph
#define FIREBASE_PH_COLLECTION "EX-ph"
#define FIREBASE_PH_VL_FIELD "fields/phLevel/integerValue"
#define FIREBASE_PH_TM_FIELD "fields/createdAt/timestampValue"
//tds
#define FIREBASE_TDS_COLLECTION "EX-tds"
#define FIREBASE_TDS_VL_FIELD "fields/tdsLevel/integerValue"
#define FIREBASE_TDS_TM_FIELD "fields/createdAt/timestampValue"
//remote
#define FIREBASE_REMOTE_COLLECTION "remote"
#define DOCUMENT_ID "OrNOJUjtHdOhIgvnP2Yq"
//schedule
#define FIREBASE_SCHEDULE_COLLECTION "schedule"
//calibration
#define FIREBASE_CALIBRATION_COLLECTION "calibration"
#define ID_TDS_CALIBRATION "tds_sensor_admin"



#endif // CREDENTIALS_H
