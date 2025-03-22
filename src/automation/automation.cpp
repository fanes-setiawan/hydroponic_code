#include "automation.h"

const float pHChangePerMlUp = 9.48;
const float pHChangePerMlDown = -9.48;
const float pHtarget = 6.5;
const float tdsTarget = 700;
const float volumeOfWater = 6000; // ml
const float tdsMax = 840;
const float tdsMin = 560;

dataExcel dataEx;
dataExcel oldStatusExcel;

void automationPH(float phValue, float tdsValue)
{
    if((double)phValue >= 0.0){
        Serial.println("<===========>Automation===========>");
    float phDifference = pHtarget - phValue;
    float mlToAdd = 0;
    Notification notifAuto;

    float scaleFactor = volumeOfWater / 100.0;
    dataEx.pH = phValue;
    dataEx.ppm = tdsValue;
    if (phDifference > 0)
    {
        mlToAdd = (phDifference / pHChangePerMlUp) * scaleFactor;
        Serial.print("Add ");
        Serial.print(mlToAdd);
        Serial.println(" ml of pH Up.");
        dataEx.phUp = mlToAdd;
        startPump(RELAY_PHUP, mlToAdd);
    }
    else
    {
        mlToAdd = (phDifference / pHChangePerMlDown) * scaleFactor;
        Serial.print("Add ");
        Serial.print(abs(mlToAdd));
        Serial.println(" ml of pH Down.");
        dataEx.phDown = abs(mlToAdd);
        startPump(RELAY_PHDOWN, abs(mlToAdd));
    }
    delay(500);
    float tdsCheck = calculateTDSLevel(tdsValue, NORMAL);
    ;
    float phCheck = readFilteredPhValue();
    dataEx.pH_check = phCheck;
    dataEx.ppm_check = tdsCheck;
    if ((double)phCheck > 6.0 && (double)phCheck < 7.0 && (double)tdsCheck > 560 && (double)tdsCheck < 840)
    {
        dataEx.status = "BERHASIL";
    }
    else
    {
        dataEx.status = "GAGAL";
    }
    oldStatusExcel.status = dataEx.status;
    uploadData(dataEx);
    dataEx.phDown = abs(0.0);
    dataEx.phUp = abs(0.0);
    dataEx.water = abs(0.0);
    dataEx.nutrisi = abs(0.0);
    delay(2000);
    }
}
float normalizeTDS(float phValue, float currentTDS)
{
   if((double)phValue >= 0.0){
    Notification notifSensor;
    Serial.println("<===========> TDS Normalization <===========>");

    float tdsDifference = tdsTarget - currentTDS;
    float adjustmentVolume = 0;
    float mlToAdd = 0;

    dataEx.ppm = currentTDS;
    dataEx.pH = phValue;

    if (tdsDifference > 0 && currentTDS < tdsMin)
    {
        //
        adjustmentVolume = calculateVolumeToAdd(currentTDS);
        dataEx.nutrisi = abs(adjustmentVolume);
        startPump(RELAY_NUTRISI, adjustmentVolume);
    }
    else if (tdsDifference < 0 && currentTDS > tdsMax)
    {

        adjustmentVolume = calculateWaterFilter(currentTDS);
        dataEx.water = abs(adjustmentVolume);
        startPump(RELAY_WATER, adjustmentVolume);
    }
    delay(500);
    float tdsCheck = calculateTDSLevel(currentTDS, NORMAL);
    float phCheck = readFilteredPhValue();
    dataEx.pH_check = phCheck;
    dataEx.ppm_check = tdsCheck;
    if ((double)tdsCheck > 560 && (double)tdsCheck < 840 && (double)phCheck > 6.0 && (double)phCheck < 7.0)
    {
        dataEx.status = "BERHASIL";
    }
    else
    {
        dataEx.status = "GAGAL";
    }
    oldStatusExcel.status = dataEx.status;
    uploadData(dataEx);
    dataEx.phDown = abs(0.0);
    dataEx.phUp = abs(0.0);
    dataEx.water = abs(0.0);
    dataEx.nutrisi = abs(0.0);
    delay(2000);

    currentTDS = calculateTDSLevel(currentTDS, NORMAL);

    return currentTDS;
   }
}

float calculateVolumeToAdd(float currentPPM)
{
    float ppmDifference = tdsTarget - currentPPM;
    float efficiency = 1536.67;
    float volumeNeeded = (ppmDifference * volumeOfWater) / (efficiency * 100);
    return volumeNeeded;
}

float calculateWaterFilter(float currentTDS)
{
    if (tdsTarget >= currentTDS)
    {
        Serial.println("No additional water needed, or target TDS is not lower than current TDS.");
        return 0.0;
    }
    float additionalWater = volumeOfWater * ((currentTDS - tdsTarget) / tdsTarget);
    return additionalWater;
}

void checkStatus()
{
    Serial.println("Check Status ::: -x-x-x-x-x-x-x-x-x");
    Serial.println("Old Status : " + oldStatusExcel.status);
    if (oldStatusExcel.status == "NORMAL")
    {
        dataEx.status = "STABIL";
    }
}
