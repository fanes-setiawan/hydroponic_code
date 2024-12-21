#include <WiFi.h>
#include "credentials.h"
#include "wifi_config.h"
#include "firebase_utils.h"

void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
        delay(1000);
        Serial.println("Connecting...");
        retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        connectToFirebase();
    } else {
        Serial.println("Failed to connect to WiFi after 10 attempts.");
    }
}

