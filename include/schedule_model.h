#ifndef SCHEDULE_MODEL_H
#define SCHEDULE_MODEL_H

#include <Arduino.h>

class ScheduleModel{
public:
    bool autoRemot;
    bool mixer;
    bool waterPump;
    float nutrisi;
    float phDown;
    float phUp;
    float water;
    String scheduled_time;

    ScheduleModel(
        bool autoRemot,
        bool mixer,
        bool waterPump,
        float nutrisi,
        float phDown,
        float phUp,
        float water,
        String scheduled_time
    ) {
        this->autoRemot = autoRemot;
        this->mixer = mixer;
        this->waterPump = waterPump;
        this->nutrisi = nutrisi;
        this->phDown = phDown;
        this->phUp = phUp;
        this->water = water;
        this->scheduled_time = scheduled_time;
    }

    ScheduleModel() : ScheduleModel(false,false,false,0.0,0.0,0.0,0.0,""){}
};

#endif