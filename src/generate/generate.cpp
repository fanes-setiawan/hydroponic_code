#include "generate.h"

int getTDSLevel(int lowerBound, int upperBound)
{
    return random(lowerBound, upperBound + 1);
}
float getPHLevel(float lowerBound, float upperBound)
{
    return random(lowerBound * 100, upperBound * 100) / 100.0;
}

float calculateTDSLevel(float currentTDS, AdjustmentMode mode)
{
    static unsigned long lastChangeTime = 0;
    static unsigned long changeInterval = random(60000, 180000);
    unsigned long currentTime = millis();
    int maxChange = 50;

    bool manualUpdate = false;

    if (mode == NORMAL || mode == UP || mode == DOWN)
    {
        manualUpdate = true;
    }

    if (manualUpdate || currentTime - lastChangeTime > changeInterval)
    {
        int newTDS;

        switch (mode)
        {
        case NORMAL:
            if (currentTDS < 560 || currentTDS > 840)
            {
                currentTDS = getTDSLevel(560, 840);
            }
            else
            {

                newTDS = currentTDS + getTDSLevel(-maxChange, maxChange);
                currentTDS = constrain(newTDS, 560, 840);
            }
            break;

        case UP:
            if (currentTDS < 560)
            {
                currentTDS = getTDSLevel(560, 840);
            }
            else if (currentTDS <= 840)
            {
                currentTDS = getTDSLevel(841, 1400);
            }
            else
            {
                newTDS = currentTDS + getTDSLevel(-maxChange, maxChange);
                currentTDS = min(newTDS, 1400);
            }
            break;

        case DOWN:
            if (currentTDS > 840)
            {
                currentTDS = getTDSLevel(560, 840);
            }
            else if (currentTDS >= 560)
            {
                currentTDS = getTDSLevel(0, 559);
            }
            else
            {
                newTDS = currentTDS + getTDSLevel(-maxChange, maxChange);
                currentTDS = max(newTDS, 0);
            }
            break;
        }

        if (!manualUpdate)
        {
            lastChangeTime = currentTime;
            changeInterval = random(60000, 180000);
        }

        Serial.print("[TDS] New TDS Level: ");
        Serial.println(currentTDS);
        Serial.print("[TDS] Change Interval: ");
        Serial.println(changeInterval);
    }

    return currentTDS;
}

float calculatePHLevel(float currentPH, AdjustmentMode mode)
{
    static unsigned long lastChangeTime = 0;
    static unsigned long changeInterval = random(60000, 120000); 
    unsigned long currentTime = millis();
    float maxChange = 0.80; 

    bool manualUpdate = false;

    if (mode == NORMAL || mode == UP || mode == DOWN)
    {
        manualUpdate = true;
    }

    if (manualUpdate || currentTime - lastChangeTime > changeInterval)
    {
        float newPH;

        switch (mode)
        {
        case NORMAL:
            if (currentPH < 6.0 || currentPH > 7.0)
            {
                currentPH = getPHLevel(6.0, 7.0);
            }
            else
            {
                newPH = currentPH + getPHLevel(-maxChange, maxChange);
                currentPH = constrain(newPH, 6.0, 7.0);
            }
            break;

        case UP:
            if (currentPH < 6.0)
            {
                currentPH = getPHLevel(6.0, 7.0);
            }
            else if (currentPH <= 7.0)
            {
                currentPH = getPHLevel(7.1, 8.0);
            }
            else
            {
                newPH = currentPH + getPHLevel(-maxChange, maxChange);
                currentPH = fmin(newPH, 8.0);
            }
            break;

        case DOWN:
            if (currentPH > 7.0)
            {
                currentPH = getPHLevel(6.0, 7.0);
            }
            else if (currentPH >= 6.0)
            {
                currentPH = getPHLevel(4.0, 5.9);
            }
            else
            {
                newPH = currentPH + getPHLevel(-maxChange, maxChange);
                currentPH = fmax(newPH, 4.0);
            }
            break;
        }

        if (!manualUpdate)
        {
            lastChangeTime = currentTime;
            changeInterval = random(60000, 180000);
        }

        // Output ke Serial Monitor
        Serial.print("[pH] New pH Level: ");
        Serial.println(currentPH);
        Serial.print("[pH] Change Interval: ");
        Serial.println(changeInterval);
    }

    return currentPH;
}