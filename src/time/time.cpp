#include <Arduino.h>
#include <time.h>
#include "time_utils.h"

void setupTime()
{

  const long gmtOffset_sec = 7 * 3600;
  const int daylightOffset_sec = 0;
  const char *ntpServer = "pool.ntp.org";

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  Serial.println("Waiting for time");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nTime synced!");
}

String getTimestamp()
{
  time_t now = time(nullptr);
  struct tm timeinfo;

  if (getLocalTime(&timeinfo))
  {
    char localTimeString[50];
    strftime(localTimeString, sizeof(localTimeString), "%B %d, %Y at %I:%M:%S %p UTC+7", &timeinfo);
    Serial.print("Local Time: ");
    Serial.println(localTimeString);
  }

  struct tm *utcTime = gmtime(&now);
  char utcTimeString[25];

  strftime(utcTimeString, sizeof(utcTimeString), "%Y-%m-%dT%H:%M:%SZ", utcTime);

  return String(utcTimeString);
}
