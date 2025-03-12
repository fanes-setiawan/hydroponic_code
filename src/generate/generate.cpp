#include "generate.h"

int getTDSLevel(int lowerBound, int upperBound)
{
    return random(lowerBound, upperBound + 1);
}

float calculateTDSLevel(float currentTDS, TDSAdjustmentMode mode)
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
