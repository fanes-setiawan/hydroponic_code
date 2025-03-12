#include "documentation.h"


void uploadData(dataExcel dataEx) {
    HTTPClient http;
    http.begin(GSHEETS);
    http.addHeader("Content-Type", "application/json");

    // Buat objek JSON untuk dikirim
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["pH"] = String(dataEx.pH,2);
    jsonDoc["ppm"] =String(dataEx.ppm,2); 
    jsonDoc["nutrisi"] =String(dataEx.nutrisi,2); 
    jsonDoc["air"] =String(dataEx.water,2); 
    jsonDoc["pH_up"] =String(dataEx.phUp,2); 
    jsonDoc["pH_down"] =String(dataEx.phDown,2); 
    jsonDoc["pH_check"] =String(dataEx.pH_check,2); 
    jsonDoc["ppm_check"] =String(dataEx.ppm_check,2); 
    jsonDoc["status"] =dataEx.status; 

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    // Kirim data ke Google Apps Script
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
        Serial.print("Data uploaded successfully, response: ");
        Serial.println(http.getString());
    } else {
        Serial.print("Error uploading data, HTTP response code: ");
        Serial.println(httpResponseCode);
    }
    http.end();  
}
