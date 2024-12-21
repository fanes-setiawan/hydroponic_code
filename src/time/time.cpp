#include <Arduino.h>
#include <time.h>
#include "time_utils.h"

String getTimestamp() {
  time_t now = time(nullptr);
  struct tm* timeInfo = gmtime(&now);
  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", timeInfo);
  return String(buffer);
}
