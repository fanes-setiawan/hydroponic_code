#ifndef DOCUMENTATION_H
#define DOCUMENTATION_H

#include <HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "credentials.h"


class dataExcel{
  public:
  float pH;
  float ppm;
  float nutrisi;
  float water;
  float phUp;
  float phDown;
  float pH_check;
  float ppm_check;
  String status;
  
  dataExcel(float pH, float ppm, float nutrisi, float water, float phUp, float phDown, float pH_check, float ppm_check, String status) {
    this->pH = pH;
    this->ppm = ppm;
    this->nutrisi = nutrisi;
    this->water = water;
    this->phUp = phUp;
    this->phDown = phDown;
    this->pH_check = pH_check;
    this->ppm_check = ppm_check;
    this->status = status;
  }
  dataExcel() : dataExcel(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, ""){}
};
void uploadData(dataExcel data);
#endif
