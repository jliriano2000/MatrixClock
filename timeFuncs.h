/** ********************************************************************************
*@file timeFuncs.h
*@date February 6th, 2026
*@author julio.liriano (julio.liriano@gmail.com)
*@brief
*
********************************************************************************** */
#ifndef __TIMEFUNCS_H
#define __TIMEFUNCS_H
/* Includes ------------------------------------------------------------------*/
#include <time.h>
#include <stdint.h>
/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Gets absolute time in milliseconds since epoch. 
 * 
 * @return uint64_t - Absolute time in milliseconds since epoch.
 */
uint64_t getTimeInMsec(void); 

/**
 * @brief Initializes the tick counter. Should be called once at the start of the program.
 * 
 */
void initTick(void);

/**
 * @brief Returns number of 1 msec ticks since the program started.
 * 
 * @return uint64_t - Number of 1 msec ticks since the program started.
 */
uint64_t getTick(void);

/**
 * @brief Delay execution for a specified number of milliseconds. Uses a busy-wait loop based on the tick counter.
 * 
 * @param msec - Number of milliseconds to delay.
 */
void delayMsec(uint64_t msec);

/**
 * @brief Gets the current local time and fills the provided tm structure. The hour is converted to 12-hour format.
 * 
 * @param timeInfo - Pointer to a tm structure that will be filled with the current local time information.
 */
void getTime(struct tm *timeInfo);
#endif /* __TIMEFUNCS_H */



