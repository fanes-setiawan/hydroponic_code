#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>
#include <time.h>

// Deklarasi fungsi
String getTimestamp();
String getTimeNow();
String formatScheduleTime(String rawTime);
void setupTime();

#endif
