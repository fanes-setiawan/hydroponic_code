#include "firebase_utils.h"
#include "credentials.h"
#include "time_utils.h"
#include "remote_model.h"
#include <ArduinoJson.h>

// Inisialisasi variabel Firebase
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Fungsi untuk menghubungkan ke Firebase
void connectToFirebase()
{
  Serial.println("Connecting to Firebase...");
  config.api_key = FIREBASE_API_KEY;
  config.host = FIREBASE_HOST;

  auth.user.email = AUTH_EMAIL;
  auth.user.password = AUTH_PASSWORD;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);
}

// post data Ph to firebase
void sendDataPhToFirestore(float phLevel)
{
  FirebaseJson json;

  // Menambahkan field ke JSON
  json.set(FIREBASE_PH_VL_FIELD, String((int)phLevel));
  json.set(FIREBASE_PH_TM_FIELD, getTimestamp());

  // Menampilkan data JSON
  Serial.println(json.raw());

  // Mengirim data ke Firestore
  if (Firebase.Firestore.createDocument(&firebaseData, FIREBASE_PROJECT_ID, "", FIREBASE_PH_COLLECTION, json.raw()))
  {
    Serial.println("Data sent successfully!");
    Serial.println(firebaseData.payload());
  }
  else
  {
    Serial.println("Failed to send data!");
    Serial.println(firebaseData.errorReason());
  }
}

// post data Tds to firebase
void sendDataTdsToFirestore(float tdsLevel)
{
  FirebaseJson json;

  // Menambahkan field ke JSON
  json.set(FIREBASE_TDS_VL_FIELD, String((int)tdsLevel));
  json.set(FIREBASE_TDS_TM_FIELD, getTimestamp());

  // Menampilkan data JSON
  Serial.println(json.raw());

  // Mengirim data ke Firestore
  if (Firebase.Firestore.createDocument(&firebaseData, FIREBASE_PROJECT_ID, "", FIREBASE_TDS_COLLECTION, json.raw()))
  {
    Serial.println("Data sent successfully!");
    Serial.println(firebaseData.payload());
  }
  else
  {
    Serial.println("Failed to send data!");
    Serial.println(firebaseData.errorReason());
  }
}

// read remote data from firebase
RemoteModel readDataRemoteFromFirestore()
{
    RemoteModel remoteModel;

    if (Firebase.Firestore.getDocument(&firebaseData, FIREBASE_PROJECT_ID, "", FIREBASE_REMOTE_COLLECTION))
    {
        String jsonString = firebaseData.payload();
        StaticJsonDocument<4096> doc; // Tingkatkan ukuran jika data JSON besar

        // Parsing JSON
        DeserializationError error = deserializeJson(doc, jsonString);

        if (!error)
        {
            // Cetak seluruh dokumen JSON untuk debugging
            Serial.println("Dokumen Firestore:");
            serializeJsonPretty(doc, Serial);
            Serial.println();

            // Iterasi melalui array "documents"
            JsonArray documents = doc["documents"].as<JsonArray>();
            for (JsonObject document : documents)
            {
                JsonObject fields = document["fields"];
                remoteModel.phUp = fields["phUp"]["doubleValue"];
                remoteModel.phDown = fields["phDown"]["doubleValue"];
                remoteModel.water = fields["water"]["doubleValue"];
                remoteModel.nutrisi = fields["nutrisi"]["doubleValue"];
                remoteModel.mixer = fields["mixer"]["booleanValue"];
                remoteModel.waterPump = fields["waterPump"]["booleanValue"];
                remoteModel.autoMode = fields["auto"]["booleanValue"];

                // Cetak data yang diambil
                Serial.println("Data Remote Model:");
                Serial.print("phUp: ");
                Serial.println(remoteModel.phUp);
                Serial.print("phDown: ");
                Serial.println(remoteModel.phDown);
                Serial.print("water: ");
                Serial.println(remoteModel.water);
                Serial.print("nutrisi: ");
                Serial.println(remoteModel.nutrisi);
                Serial.print("mixer: ");
                Serial.println(remoteModel.mixer);
                Serial.print("waterPump: ");
                Serial.println(remoteModel.waterPump);
                Serial.print("autoMode: ");
                Serial.println(remoteModel.autoMode);
            }
        }
        else
        {
            Serial.println("Failed to parse JSON!");
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.println("Failed to read data!");
        Serial.println(firebaseData.errorReason());
    }

    return remoteModel;
}
