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
#include <stdbool.h>


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

/**
 * @brief Used in display of matrix in terminal. Clears the last n lines printed to the terminal.
 * 
 * @param n - number of lines to clear
 */
static void clearLastLines(int n); 
/* Definitions ---------------------------------------------------------------*/
// Main LED Matrix representation. This frame may be sent as is to the hardware 
// driving the LED matrix.
uint8_t ledMatrix[MATRIX_HEIGHT][MATRIX_WIDTH] = {0};

static const coordinate_t charPositions[NUM_POSITIONS] = {
    [POS1] = {.row = 1, .col = 1},   // POS1
    [POS2] = {.row = 1, .col = 5},   // POS2
    [COLON] = {.row = 1, .col = 8},  // COLON
    [POS3] = {.row = 1, .col = 11},  // POS3
    [POS4] = {.row = 1, .col = 15},  // POS4
    [ALARM_DOT] = {.row = 0, .col = MATRIX_WIDTH - 1} // ALARM_DOT Position (top-right corner)
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
    
    // Copy over coordinates for the position
    memcpy(&target, &charPositions[position], sizeof(coordinate_t));

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
        case ALARM_CHAR_SET:
            ledMatrix[target.row][target.col] = 1; // Set the alarm dot (top-right corner)
            return; // No need to copy a sprite for the alarm dot, so we can return early
            break; 
        case ALARM_CHAR_CLR:
            ledMatrix[target.row][target.col] = 0; // Clear the alarm dot (top-right corner)
            return; // No need to copy a sprite for the alarm dot, so we can return early
            break;
        default:
            // Invalid character, should not happen due to prior checks
            // but will handle here. 
            return;
    }

    // Copy over the sprite for the character to the target position on the matrix
    for(uint8_t i = 0; i < SPRITE_HEIGHT; i++) {
        for(uint8_t j = 0; j < SPRITE_WIDTH; j++) {
            ledMatrix[target.row + i][target.col + j] = (*spritePtr)[i][j];
        }   
    }
    return; 
}

static void clearLastLines(int n) 
{
    if (n < 1) return;

    // Move cursor up N lines, clear the line, move cursor to the start of the line
    for (int i = 0; i < n; ++i) {
        // Move cursor up one line: \033[1A or \x1b[1A
        printf("\x1b[1A");
        // Clear the entire current line: \033[2K or \x1b[2K
        printf("\x1b[2K");
    }
    // Optional: Move the cursor to the beginning of the line (left margin)
    printf("\r");

    // Flush stdout to ensure the commands are sent to the terminal immediately
    fflush(stdout);
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

        // Only alarm dot can be set at ALARM_DOT position
        if (position == ALARM_DOT && (character != ALARM_CHAR_CLR && character != ALARM_CHAR_SET) )
        {
            printf("Invalid argument: Only alarm dot can be set at ALARM_DOT position. character=%d, position=%d\n", character, position);
            status = LED_ARG_ERROR;
            break;
        }

        // Update the matrix with the character sprite at the specified position
        setCharAtPosition(character, position);

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

void printMatrix(void) {
    static bool firstPrint = true;
    
    // Determine if this is first print
    if (!firstPrint) {
        // clear the last printed matrix from the terminal before printing the new one
        clearLastLines(MATRIX_HEIGHT);
    } else {
        firstPrint = false;
    }

    // Print the matrix
    for (uint8_t i = 0; i < MATRIX_HEIGHT; i++) {
        for (uint8_t j = 0; j < MATRIX_WIDTH; j++) {
            // Green filled circle for 1, gray hollow circle for 0. 
            // I found this pleaseing though UI/UX folks may not :). 
            printf("%s ", ledMatrix[i][j] != 0 ? "\x1b[32m\u25CF\x1b[0m" : "\033[90m\xe2\x97\xa6\033[0m"); //
        }
        printf("\r\n");
    }
}
