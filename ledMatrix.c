/** ********************************************************************************
*@file ledMatrix.c
*
*@date February 5th, 2026
*
*@author julio.liriano (julio.liriano@gmail.com)
*
*@brief
********************************************************************************

/* Includes ------------------------------------------------------------------*/
#include "ledMatrix.h"
#include <stdio.h>
#include <string.h>

// Could include the hardware-specific LED matrix driver headers here
// to send the ledMatrix frame to the actual hardware.
// #include "hardwareLedDriver.h"

/* Imported variables --------------------------------------------------------*/

/* Imported function prototypes ----------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

typedef struct {
    uint8_t row;
    uint8_t col;
} coordinate_t;

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Updates matrix with sprite of character at given position.
 * 
 * @param character - character to display
 * @param position - position on the matrix
 */
static void setCharAtPosition(character_t character, char_pos_t position);


/* Definitions ---------------------------------------------------------------*/
// Main LED Matrix representation. This frame may be sent as is to the hardware 
// driving the LED matrix.
uint8_t ledMatrix[MATRIX_WIDTH][MATRIX_HEIGHT] = {0};

static const coordinate_t charPositions[NUM_POSITIONS] = {
    [POS1] = {.row = 0, .col = 0},   // POS1
    [POS2] = {.row = 4, .col = 0},   // POS2
    [COLON] = {.row = 8, .col = 0},   // COLON
    [POS3] = {.row = 11, .col = 0},  // POS3
    [POS4] = {.row = 15, .col = 0}   // POS4
};

static const uint8_t colonSprite[SPRITE_HEIGHT][SPRITE_WIDTH] = {
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0}
};

static const uint8_t dashSprite[SPRITE_HEIGHT][SPRITE_WIDTH] = {
    {0, 0, 0},
    {0, 0, 0},
    {1, 1, 1},
    {0, 0, 0},
    {0, 0, 0}
};

static const uint8_t numberSprites[10][SPRITE_HEIGHT][SPRITE_WIDTH] = {
    // 0
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1}
    },
    // 1
    {
        {0, 1, 0},
        {1, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
        {1, 1, 1}
    },
    // 2
    {
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1}
    },
    // 3
    {
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1}
    },
    // 4
    {
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 0, 1},
        {0, 0, 1}
    },
    // 5
    {
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1}
    },
    // 6
    {
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1}
    },
    // 7
    {
        {1, 1, 1},
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0},
        {1, 0, 0}
    },
    // 8
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1}
    },
    // 9
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1}
    }
};

static void setCharAtPosition(character_t character, char_pos_t position)
{
    coordinate_t target = {0}; 
    uint8_t const (*spritePtr)[SPRITE_HEIGHT][SPRITE_WIDTH] = NULL; 
    
    // which sprite to use
    switch(character) {
        case ZERO_CHAR:
        case ONE_CHAR:
        case TWO_CHAR:
        case THREE_CHAR:
        case FOUR_CHAR:
        case FIVE_CHAR:
        case SIX_CHAR:
        case SEVEN_CHAR:
        case EIGHT_CHAR:
        case NINE_CHAR:
            // Intentional fallthrough
            spritePtr = &numberSprites[character];
            break;
        case COLON_CHAR:
            spritePtr = &colonSprite;
            break;
        case DASH_CHAR:
            spritePtr = &dashSprite;
            break;
        default:
            // Invalid character, should not happen due to prior checks
            // but will handle here. 
            return;
    }

    // Copy over coordinates for the position
    memcpy(&target, &charPositions[position], sizeof(coordinate_t));

    // Copy over the sprite
    for(uint8_t i = 0; i < SPRITE_HEIGHT; i++) {
        for(uint8_t j = 0; j < SPRITE_WIDTH; j++) {
            ledMatrix[target.row + i][target.col + j] = (*spritePtr)[i][j];
        }   
    }
    return; 
}

led_matrix_err_t setCharacterAtPosition(character_t character, char_pos_t position)
{
    led_matrix_err_t status = LED_OK;

    do
    {
        //Check for valid arguments
        if (character < 0 || character >= NUM_CHARACTERS ||
            position < 0 || position >= NUM_POSITIONS)
        {
            printf("Invalid argument: character=%d, position=%d\n", character, position);
            status = LED_ARG_ERROR;
            break;
        }

        // Only colon can be set at COLON position
        if (position == COLON && character != COLON_CHAR)
        {
            printf("Invalid argument: Only colon can be set at COLON position. character=%d, position=%d\n", character, position);
            status = LED_ARG_ERROR;
            break;
        }

        // Update the matrix with the character sprite at the specified position
        setCharacterAtPosition(character, position);

    } while (0);
    return status;
}

void clearMatrix(void) {
    memset(ledMatrix, 0, sizeof(ledMatrix));
}

led_matrix_err_t getMatrix(uint8_t matrixOut[MATRIX_WIDTH][MATRIX_HEIGHT]) {

    // Check for NULL pointer
    if(matrixOut == NULL) {
        return LED_ARG_ERROR;
    }
    memcpy(matrixOut, ledMatrix, sizeof(ledMatrix));
    return LED_OK;
}

led_matrix_err_t sendMatrix(void) {
    led_matrix_err_t status = LED_OK;
    // This function would interface with the hardware-specific LED matrix driver
    // to send the current ledMatrix frame to the actual LED matrix hardware.
    // I would update this function to return error codes based on the hardware driver feedback.
    return status; 
}