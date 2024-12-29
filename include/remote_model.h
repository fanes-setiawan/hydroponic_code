#ifndef REMOTE_MODEL_H
#define REMOTE_MODEL_H

#include <Arduino.h>
#include <FirebaseJson.h>
class RemoteModel {
public:
    float phUp;
    float phDown;
    float water;
    float nutrisi;
    bool mixer;
    bool waterPump;
    bool autoMode;

    // Constructor
    RemoteModel(
        float phUp,
        float phDown,
        float water,
        float nutrisi,
        bool mixer,
        bool waterPump,
        bool autoMode
    ) {
        this->phUp = phUp;
        this->phDown = phDown;
        this->water = water;
        this->nutrisi = nutrisi;
        this->mixer = mixer;
        this->waterPump = waterPump;
        this->autoMode = autoMode;
    }

    // Default constructor
    RemoteModel() : RemoteModel(0.0, 0.0, 0.0, 0.0, false, false, false) {}
};


#endif // REMOTE_MODEL_H
