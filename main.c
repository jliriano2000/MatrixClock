/** ********************************************************************************
*@file main.c
*
*@date February 5th, 2026
*
*@author julio.liriano (julio.liriano@gmail.com)
*
*@brief


/* Includes ------------------------------------------------------------------*/

#include "ledMatrix.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
/* Imported variables --------------------------------------------------------*/

/* Imported function prototypes ----------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Definitions ---------------------------------------------------------------*/


int main(void) {
    // Initialize the LED matrix
    clearMatrix();

    // Set the characters at the specified positions
    setCharacterAtPosition(ONE_CHAR, POS1);
    setCharacterAtPosition(ONE_CHAR, POS2);
    setCharacterAtPosition(COLON_CHAR, COLON);
    setCharacterAtPosition(ONE_CHAR, POS3);
    setCharacterAtPosition(ONE_CHAR, POS4);
    printMatrix();

    sleep(2);
    return 0;
}

