#include "schedule_run.h"

void runSchedule(ScheduleModel setSchedule)
{
    if (setSchedule.waterPump == true)
    {
        runWaterPump();
    }
    if (setSchedule.waterPump == false)
    {
        stopWaterPump();
    }
    if (setSchedule.mixer == true)
    {
        runMixer();
    }
    if (setSchedule.mixer == false)
    {
        stopMixer();
    }
    if ((double)setSchedule.nutrisi != 0.0)
    {
        startPump(RELAY_NUTRISI, setSchedule.nutrisi);
        patchDataRemoteToFirestore("nutrisi", "0.0");
    }
    if ((double)setSchedule.phUp != 0.0)
    {
        startPump(RELAY_PHUP, setSchedule.phUp);
        patchDataRemoteToFirestore("phUp", "0.0");
    }
    if ((double)setSchedule.phDown != 0.0)
    {
        startPump(RELAY_PHDOWN, setSchedule.phDown);
        patchDataRemoteToFirestore("phDown", "0.0");
    }
    if ((double)setSchedule.water != 0.0)
    {
        startPump(RELAY_WATER, setSchedule.water);
        patchDataRemoteToFirestore("water", "0.0");
    }
}