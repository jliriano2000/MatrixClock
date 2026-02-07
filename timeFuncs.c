/** ********************************************************************************
*@file timeFuncs.c
*
*@date February 6th, 2026
*
*@author julio.liriano (julio.liriano@gmail.com)
*
*@brief
********************************************************************************

/* Includes ------------------------------------------------------------------*/
#include "timeFuncs.h"
#include <stdio.h>
/* Imported variables --------------------------------------------------------*/

/* Imported function prototypes ----------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint64_t initTickCount = 0;
/* Private functions ---------------------------------------------------------*/

/* Definitions ---------------------------------------------------------------*/
uint64_t getTimeInMsec(void) {
    struct timespec ts = {0};
    uint64_t absMsec = 0;

    // Get msec since epoch
    if (timespec_get(&ts, TIME_UTC) == TIME_UTC) {
        // Convert the seconds and nanoseconds to a total millisecond value
        absMsec = ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
    }
    return absMsec;
}

void initTick(void) {
    initTickCount = getTimeInMsec();
}

uint64_t getTick(void) {
    uint64_t currentMsec = getTimeInMsec();
    return currentMsec - initTickCount;
}

void delayMsec(uint64_t msec) {
    uint64_t startTick = getTick();
    while ((getTick() - startTick) < msec) {
        // Busy wait
    }
}

void getTime(struct tm *timeInfo) {
    time_t rawTime;
    time(&rawTime);

    // Null check for timeInfo pointer
    if(timeInfo == NULL) {
        printf("Error: timeInfo pointer is NULL\n");
        return;
    }

    // Convert raw time to local time
    localtime_r(&rawTime, timeInfo);

    // Set to standard time format (e.g., 12-hour format)
    if(timeInfo->tm_hour > 12) {
        timeInfo->tm_hour -= 12;
    }
    
    // printf("Current time: %02d:%02d:%02d\n", timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
}



