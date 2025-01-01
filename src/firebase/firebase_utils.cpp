#include "firebase_utils.h"
#include "credentials.h"
#include "time_utils.h"
#include "remote_model.h"
#include "schedule_model.h"
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>

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

ScheduleModel readDataScheduleFromFirestore()
{
  ScheduleModel scheduleModel;
  FirebaseJson structuredQuery;
  String timeNow = getTimestamp();

  structuredQuery.clear();

  FirebaseJsonArray fromArray;
  FirebaseJson fromCollection;
  fromCollection.set("collectionId", FIREBASE_SCHEDULE_COLLECTION);
  fromArray.add(fromCollection);
  structuredQuery.set("from", fromArray);

  FirebaseJson whereCondition;
  FirebaseJson fieldFilter;
  FirebaseJson field;
  FirebaseJson timestampValue;

  field.set("fieldPath", "scheduled_time");

  timestampValue.set("timestampValue", timeNow);

  fieldFilter.set("field", field);
  fieldFilter.set("op", "GREATER_THAN_OR_EQUAL");
  fieldFilter.set("value", timestampValue);

  whereCondition.set("fieldFilter", fieldFilter);
  structuredQuery.set("where", whereCondition);
  structuredQuery.set("limit", 1);

  String jsonString;
  structuredQuery.toString(jsonString, true);
  Serial.println(jsonString);

  if (Firebase.Firestore.runQuery(&firebaseData, FIREBASE_PROJECT_ID, "", "", &structuredQuery))
  {
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, firebaseData.payload());

    if (!error)
    {
      JsonArray documents = doc.as<JsonArray>();
      if (documents.size() > 0)
      {
        JsonObject document = documents[0]["document"];
        JsonObject fields = document["fields"]["settings"]["mapValue"]["fields"];
        serializeJsonPretty(document, Serial);

        scheduleModel.phUp = fields["phUp"]["doubleValue"];
        scheduleModel.phDown = fields["phDown"]["doubleValue"];
        scheduleModel.water = fields["water"]["doubleValue"];
        scheduleModel.nutrisi = fields["nutrisi"]["doubleValue"];
        scheduleModel.mixer = fields["mixer"]["booleanValue"];
        scheduleModel.waterPump = fields["waterPump"]["booleanValue"];
        scheduleModel.autoRemot = fields["auto"]["booleanValue"];
        scheduleModel.scheduled_time = document["fields"]["scheduled_time"]["timestampValue"].as<String>();

        // Cetak data yang diambil
        Serial.println("Data Schedule Model:");
        Serial.println("pH Up: " + String(scheduleModel.phUp));
        Serial.println("pH Down: " + String(scheduleModel.phDown));
        Serial.println("Water: " + String(scheduleModel.water));
        Serial.println("Nutrisi: " + String(scheduleModel.nutrisi));
        Serial.println("Mixer: " + String(scheduleModel.mixer));
        Serial.println("Water Pump: " + String(scheduleModel.waterPump));
        Serial.println("Auto Remote: " + String(scheduleModel.autoRemot));
      }
      else
      {
        Serial.println("No documents found.");
      }
    }
    else
    {
      Serial.println("Error parsing JSON: " + String(error.c_str()));
    }
  }
  else
  {
    Serial.println("Failed to execute query!");
    Serial.println(firebaseData.errorReason());
  }

  return scheduleModel;
}
