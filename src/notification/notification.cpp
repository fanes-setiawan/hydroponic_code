#include "notification.h"
#include "credentials.h"

Notification::Notification() {
    firestoreUrl = String("https://firestore.googleapis.com/v1/projects/") + FIREBASE_PROJECT_ID + "/databases/(default)/documents/fcmTokens/fcm-token?key=gcmToken";
    serverUrl = API_NOTIF;
    fcmToken = "";
}

void Notification::fetchToken() {
    HTTPClient http;
    http.begin(firestoreUrl);  // Mengakses URL Firestore
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Token Response:");
        Serial.println(response);

        // Parsing JSON
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, response);
    
        if (!error) {
            fcmToken = doc["fields"]["gcmToken"]["stringValue"].as<String>();
            Serial.println("GCM Token:");
            Serial.println(fcmToken);
        } else {
            Serial.println("Error parsing JSON: " + String(error.c_str()));
        }
    } else {
        Serial.print("Error fetching token, HTTP response code: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void Notification::sendNotification(String title, String body) {
    fetchToken();

    if (fcmToken.isEmpty()) {
        Serial.println("FCM Token is empty. Cannot send notification.");
        return;
    }

    // JSON payload
    String jsonPayload = "{";
    jsonPayload += "\"token\": \"" + fcmToken + "\",";
    jsonPayload += "\"title\": \"" + title + "\",";
    jsonPayload += "\"body\": \"" + body + "\"";
    jsonPayload += "}";

    HTTPClient http;
    http.begin(API_NOTIF);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Response Code: " + String(httpResponseCode));
        Serial.println("Response: " + response);
    } else {
        Serial.println("Error in sending POST: " + String(httpResponseCode));
    }

    http.end();
}
