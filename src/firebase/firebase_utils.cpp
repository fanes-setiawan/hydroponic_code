#include "firebase_utils.h"
#include "credentials.h"
#include "time_utils.h"
#include "remote_model.h"
#include "schedule_model.h"
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include "calibration_tds_model.h"
#include "calibration_ph_model.h"

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
  phLevel = round(phLevel * 100) / 100.0;

  json.set(FIREBASE_PH_VL_FIELD, phLevel);
  json.set(FIREBASE_PH_TM_FIELD, getTimestamp());
  Serial.println(json.raw());

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
  json.set(FIREBASE_TDS_VL_FIELD, String((int)tdsLevel));
  json.set(FIREBASE_TDS_TM_FIELD, getTimestamp());

  Serial.println(json.raw());

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
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (!error)
    {
      // serializeJsonPretty(doc, Serial);

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
        remoteModel.autoCheck = fields["autoCheck"]["integerValue"];
        remoteModel.isRemove = fields["isRemove"]["booleanValue"];

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

// patch data remote to firebase
void patchDataRemoteToFirestore(const String &field, const String &value)
{
  FirebaseJson json;

  // Deteksi tipe data dan set JSON
  if (value.indexOf('.') != -1)
  {
    json.set("/fields/" + field + "/doubleValue", value.toFloat());
  }
  else if (value == "true" || value == "false")
  {
    json.set("/fields/" + field + "/booleanValue", value == "true");
  }
  else if (value.toInt() != 0 || value == "0")
  {
    json.set("/fields/" + field + "/integerValue", value.toInt());
  }
  else
  {
    json.set("/fields/" + field + "/stringValue", value);
  }

  // Menampilkan data JSON untuk debugging
  Serial.println("JSON Payload: ");
  Serial.println(json.raw());

  // Nama koleksi dan dokumen
  const String documentPath = FIREBASE_REMOTE_COLLECTION + String("/") + DOCUMENT_ID;

  // Mengirim data ke Firestore
  if (Firebase.Firestore.patchDocument(&firebaseData, FIREBASE_PROJECT_ID, "", documentPath, json.raw(), field))
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

  FirebaseJson orderBy;
  orderBy.set("field.fieldPath", "scheduled_time");
  orderBy.set("direction", "ASCENDING");
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

CalibrationTdsModel readDataCalibrationTdsFromFirestore()
{
  CalibrationTdsModel calibrationTdsModel;
  Serial.println("Getting TDS calibration data from Firestore...");

  // Ubah path agar langsung ke "calibration/tds_sensor_admin"
  String documentPath = FIREBASE_CALIBRATION_COLLECTION + String("/") + ID_TDS_CALIBRATION;

  if (Firebase.Firestore.getDocument(&firebaseData, FIREBASE_PROJECT_ID, "", documentPath))
  {
    String jsonString = firebaseData.payload();
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    Serial.print("JSON: ");
    serializeJson(doc, Serial);
    Serial.println();

    Serial.print("STATUS ERROR: ");
    Serial.println(error.c_str());

    if (!error)
    {
      if (doc.containsKey("fields"))
      {
        JsonObject fields = doc["fields"];

        Serial.print("document[fields]: ");
        serializeJson(fields, Serial);
        Serial.println();

        // Ambil nilai TDS dari Firestore
        if (fields.containsKey("fluid_ppm"))
        {
          if (fields["fluid_ppm"].containsKey("integerValue"))
          {
            calibrationTdsModel.fluid_ppm = fields["fluid_ppm"]["integerValue"].as<float>();
          }
          else if (fields["fluid_ppm"].containsKey("doubleValue"))
          {
            calibrationTdsModel.fluid_ppm = fields["fluid_ppm"]["doubleValue"];
          }
        }

        // Ambil status kalibrasi dari Firestore
        if (fields.containsKey("status"))
        {
          if (fields["status"].containsKey("booleanValue"))
          {
            calibrationTdsModel.status = fields["status"]["booleanValue"].as<bool>();
          }
        }

        // Ambil waktu update dari Firestore
        if (fields.containsKey("updatedAt"))
        {
          if (fields["updatedAt"].containsKey("timestampValue"))
          {
            calibrationTdsModel.updateAt = fields["updatedAt"]["timestampValue"].as<String>();
          }
        }

        Serial.print("[TDS] Value: ");
        Serial.println(calibrationTdsModel.fluid_ppm);
        Serial.print("[TDS] Status: ");
        Serial.println(calibrationTdsModel.status);
        Serial.print("[TDS] Updated At: ");
        Serial.println(calibrationTdsModel.updateAt);
      }
      else
      {
        Serial.println("No 'fields' key found in JSON!");
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
    Serial.println("Failed to get document from Firestore!");
    Serial.println(firebaseData.errorReason());
  }

  return calibrationTdsModel;
}

// patch status
void patchDataCalibrationTdsToFirestore(const String &field, const String &value)
{
  FirebaseJson json;

  // Menentukan tipe data yang sesuai
  if (value.indexOf('.') != -1)
  {
    json.set("/fields/" + field + "/doubleValue", value.toFloat());
  }
  else if (value == "true" || value == "false")
  {
    json.set("/fields/" + field + "/booleanValue", value == "true");
  }
  else if (value.toInt() != 0 || value == "0")
  {
    json.set("/fields/" + field + "/integerValue", value.toInt());
  }
  else
  {
    json.set("/fields/" + field + "/stringValue", value);
  }

  Serial.println("JSON Payload: ");
  Serial.println(json.raw());

  // Ubah path agar langsung ke "calibration/tds_sensor_admin"
  String documentPath = FIREBASE_CALIBRATION_COLLECTION + String("/") + ID_TDS_CALIBRATION;

  if (Firebase.Firestore.patchDocument(&firebaseData, FIREBASE_PROJECT_ID, "", documentPath, json.raw(), field))
  {
    Serial.println("[TDS] Data sent successfully!");
    Serial.println(firebaseData.payload());
  }
  else
  {
    Serial.println("[TDS] Failed to send data!");
    Serial.println(firebaseData.errorReason());
  }
}

CalibrationPhModel readDataCalibrationPhFromFirestore()
{
  CalibrationPhModel calibrationPhModel;
  Serial.println("Getting pH calibration data from Firestore...");

  // Ubah path agar langsung ke "calibration/ph_sensor_admin"
  String documentPath = FIREBASE_CALIBRATION_COLLECTION + String("/") + ID_PH_CALIBRATION;

  if (Firebase.Firestore.getDocument(&firebaseData, FIREBASE_PROJECT_ID, "", documentPath))
  {
    String jsonString = firebaseData.payload();
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    Serial.print("JSON: ");
    serializeJson(doc, Serial);
    Serial.println();

    Serial.print("STATUS ERROR: ");
    Serial.println(error.c_str());

    if (!error)
    {
      if (doc.containsKey("fields"))
      {
        JsonObject fields = doc["fields"];

        Serial.print("document[fields]: ");
        serializeJson(fields, Serial);
        Serial.println();

        // Ambil nilai pH dari Firestore
        if (fields.containsKey("ph_value"))
        {
          if (fields["ph_value"].containsKey("doubleValue"))
          {
            calibrationPhModel.fluid_ph = fields["ph_value"]["doubleValue"];
          }
          else if (fields["ph_value"].containsKey("integerValue"))
          {
            calibrationPhModel.fluid_ph = fields["ph_value"]["integerValue"].as<float>();
          }
        }

        // Ambil status kalibrasi dari Firestore
        if (fields.containsKey("status"))
        {
          if (fields["status"].containsKey("booleanValue"))
          {
            calibrationPhModel.status = fields["status"]["booleanValue"].as<bool>();
          }
        }

        // Ambil waktu update dari Firestore
        if (fields.containsKey("updatedAt"))
        {
          if (fields["updatedAt"].containsKey("timestampValue"))
          {
            calibrationPhModel.updateAt = fields["updatedAt"]["timestampValue"].as<String>();
          }
        }

        Serial.print("[pH] Value: ");
        Serial.println(calibrationPhModel.fluid_ph);
        Serial.print("[pH] Status: ");
        Serial.println(calibrationPhModel.status);
        Serial.print("[pH] Updated At: ");
        Serial.println(calibrationPhModel.updateAt);
      }
      else
      {
        Serial.println("No 'fields' key found in JSON!");
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
    Serial.println("Failed to get document from Firestore!");
    Serial.println(firebaseData.errorReason());
  }

  return calibrationPhModel;
}

void patchDataCalibrationPhToFirestore(const String &field, const String &value)
{
  FirebaseJson json;

  // Menentukan tipe data yang sesuai
  if (value.indexOf('.') != -1)
  {
    json.set("/fields/" + field + "/doubleValue", value.toFloat());
  }
  else if (value == "true" || value == "false")
  {
    json.set("/fields/" + field + "/booleanValue", value == "true");
  }
  else if (value.toInt() != 0 || value == "0")
  {
    json.set("/fields/" + field + "/integerValue", value.toInt());
  }
  else
  {
    json.set("/fields/" + field + "/stringValue", value);
  }

  Serial.println("JSON Payload: ");
  Serial.println(json.raw());

  // Ubah path agar langsung ke "calibration/ph_sensor_admin"
  String documentPath = FIREBASE_CALIBRATION_COLLECTION + String("/") + ID_PH_CALIBRATION;

  if (Firebase.Firestore.patchDocument(&firebaseData, FIREBASE_PROJECT_ID, "", documentPath, json.raw(), field))
  {
    Serial.println("[pH] Data sent successfully!");
    Serial.println(firebaseData.payload());
  }
  else
  {
    Serial.println("[pH] Failed to send data!");
    Serial.println(firebaseData.errorReason());
  }
}
