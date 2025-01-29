#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class Notification
{
private:
    String firestoreUrl;
    String serverUrl;
    String fcmToken;

    void fetchToken();

public:
    Notification();
    void sendNotification(String title, String body);
};

#endif
