#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// WiFi credentials
// #define WIFI_SSID "Tselhome-0A06"
// #define WIFI_PASSWORD "44226533"

// #define WIFI_SSID "Carakan-Developer"
// #define WIFI_PASSWORD "2wsx1qaz"

#define WIFI_SSID "KONTRAKKAN 01"
#define WIFI_PASSWORD "KEPOAMAT"


// Firebase credentials
// #define FIREBASE_HOST "hydsmartfire.firebaseio.com"
// #define FIREBASE_API_KEY "AIzaSyCBD0g2s5pAVGQ-Fue464ZgyHZRDtdioj4"
// #define FIREBASE_PROJECT_ID "hydsmartfire"

// back up 
#define FIREBASE_HOST "back-up-hydroponic.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyC0Ilg2sQexL3NKPRLwDZqOXKiHR4W-Sfk"
#define FIREBASE_PROJECT_ID "back-up-hydroponic"

// Email/password Firebase Auth
#define AUTH_EMAIL "admin@gmail.com"
#define AUTH_PASSWORD "2wsx1qaz"

//fields for the database

//ph
// #define FIREBASE_PH_COLLECTION "EX-ph"
#define FIREBASE_PH_COLLECTION "phData"
#define FIREBASE_PH_VL_FIELD "fields/phLevel/doubleValue"
#define FIREBASE_PH_TM_FIELD "fields/createdAt/timestampValue"
//tds
// #define FIREBASE_TDS_COLLECTION "EX-tds"
#define FIREBASE_TDS_COLLECTION "tdsData"
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
#define ID_PH_CALIBRATION "ph_sensor_admin"

//notification
#define API_NOTIF "https://notifikasi-api-v3.vercel.app/api/v1/notifications/send-notification"

//gSheet api
#define GSHEETS "https://script.google.com/macros/s/AKfycbyCjwZG86-uKT_vbe48BEtoro7PCIka6I3fDLxIm4sIniwOolYZTkv5nI3RZ1ca_E7g/exec"



#endif // CREDENTIALS_H
