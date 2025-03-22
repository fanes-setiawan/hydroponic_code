#include "schedule_main.h"
#include "schedule_run.h"

#define UTC_OFFSET_HOURS 7
#define EEPROM_ADDR 0
String scheduleRun = "";

unsigned long convertTimeToTimestamp(String timeString)
{
    int year, month, day, hour, minute;
    sscanf(timeString.c_str(), "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute);
    return year * 100000000 + month * 1000000 + day * 10000 + hour * 100 + minute;
}

String convertUTCtoLocal(String utcTime)
{
    int year, month, day, hour, minute;
    sscanf(utcTime.c_str(), "%d-%d-%dT%d:%d", &year, &month, &day, &hour, &minute);

    hour += UTC_OFFSET_HOURS;
    if (hour >= 24)
    {
        hour -= 24;
        day += 1;
    }
    else if (hour < 0)
    {
        hour += 24;
        day -= 1;
    }

    char localTime[17];
    sprintf(localTime, "%04d-%02d-%02d %02d:%02d", year, month, day, hour, minute);
    return String(localTime);
}

void saveLastExecutedSchedule(String scheduleTime)
{

    char timeBuffer[17];
    scheduleTime.toCharArray(timeBuffer, 17);

    EEPROM.put(EEPROM_ADDR, timeBuffer);
    EEPROM.commit();
}

String getLastExecutedSchedule()
{
    char storedTime[17] = "";
    EEPROM.get(EEPROM_ADDR, storedTime);
    return String(storedTime);
}

void checkingSchedule()
{
    static bool eepromInitialized = false;
    if (!eepromInitialized)
    {
        EEPROM.begin(32);
        eepromInitialized = true;
    }

    ScheduleModel schedule = readDataScheduleFromFirestore();

    if (schedule.scheduled_time == "")
    {

        return;
    }

    String localScheduleTime = convertUTCtoLocal(schedule.scheduled_time);
    Serial.print("[FANES][TIME LOCAL] ");
    Serial.println(localScheduleTime);

    String timeNow = getTimeNow().substring(0, 16);
    Serial.print("[FANES][NOW] ");
    Serial.println(timeNow);

    String lastExecuted = getLastExecutedSchedule();

    if (convertTimeToTimestamp(timeNow) > convertTimeToTimestamp(lastExecuted) + 1)
    {
        Serial.println("[EEPROM] Reset jadwal terakhir karena sudah lebih dari 1 menit.");
        saveLastExecutedSchedule("");
        lastExecuted = "";
    }

    if (scheduleRun == timeNow && lastExecuted != localScheduleTime)
    { 
        notification.sendNotification("✅Schedule!!! ", "Schedule Dijalankan");
        runSchedule(schedule);
    }
    else
    {
        Serial.println("[FANES] ❌ Jadwal Tidak Dijalankan / Sudah Pernah Dijalankan");
    }

    if (scheduleRun != localScheduleTime)
    {
        scheduleRun = localScheduleTime;
    }
}
