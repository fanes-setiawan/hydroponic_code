#ifndef SCHEDULE_MAIN_H
#define SCHEDULE_MAIN_H

#include <Arduino.h>
#include <EEPROM.h>
#include "time_utils.h"
#include "schedule_model.h"

// Alamat EEPROM untuk menyimpan status eksekusi jadwal
#define EEPROM_ADDR 0


// **Fungsi untuk membaca jadwal dari Firestore**
ScheduleModel readDataScheduleFromFirestore();

// **Fungsi untuk mengecek dan menjalankan jadwal**
void checkingSchedule();

// **Fungsi untuk menyimpan jadwal terakhir yang dieksekusi ke EEPROM**
void saveLastExecutedSchedule(String scheduleTime);

// **Fungsi untuk mendapatkan jadwal terakhir yang telah dieksekusi dari EEPROM**
String getLastExecutedSchedule();


// **Fungsi untuk mengecek apakah jadwal telah dieksekusi dalam menit yang sama**
bool isScheduleExecuted(String scheduleTime);

// **Fungsi untuk mengonversi waktu UTC dari Firestore ke waktu lokal**
String convertUTCtoLocal(String utcTime);

// **Fungsi untuk memformat waktu agar hanya menyertakan YYYY-MM-DD HH:MM**
String formatScheduleTime(String rawTime);

#endif 
