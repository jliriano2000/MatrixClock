/** ********************************************************************************
*@file ledMatrix.h
*@date February 5th, 2026
*@author julio.liriano (julio.liriano@gmail.com)
*@brief

********************************************************************************** */
#ifndef __LEDMATRIX_H
#define __LEDMATRIX_H
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define MATRIX_WIDTH 19
#define MATRIX_HEIGHT 7

#define SPRITE_WIDTH 3
#define SPRITE_HEIGHT 5
/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
// These define the enums for the starting positions of each character on the matrix. 
typedef enum {
    POS1 = 0,
    POS2,
    COLON,
    POS3,
    POS4,
    ALARM_DOT,
    NUM_POSITIONS // should always be last
} char_pos_t;

typedef enum {
    ZERO_CHAR = 0,
    ONE_CHAR,
    TWO_CHAR,
    THREE_CHAR,
    FOUR_CHAR,
    FIVE_CHAR,
    SIX_CHAR,
    SEVEN_CHAR,
    EIGHT_CHAR,
    NINE_CHAR,
    COLON_CHAR, 
    DASH_CHAR,
    ALARM_CHAR_SET,
    ALARM_CHAR_CLR,
    NUM_CHARACTERS // should always be last 
} character_t;

typedef enum {
    LED_OK = 0, 
    LED_BUSY = -1, 
    LED_OVERHEAT = -2, 
    LED_ARG_ERROR = -3
} led_matrix_err_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Clears LED matrix. Sets all positions to zero. 
 * 
 */
void clearMatrix(void);

/**
 * @brief Set the character at the specified position on the LED matrix.
 * 
 * @param character 
 * @param position 
 */
led_matrix_err_t setCharacterAtPosition(character_t character, char_pos_t position);

/**
 * @brief Get the current LED matrix frame.
 * 
 * @param matrixOut - Output parameter to hold the current LED matrix frame. Must be a 2D array of size MATRIX_WIDTH x MATRIX_HEIGHT. 
 * @return led_matrix_err_t - Status of the operation.
 */
led_matrix_err_t getMatrix(uint8_t matrixOut[MATRIX_WIDTH][MATRIX_HEIGHT]); 

/**
 * @brief Sends the current ledMatrix frame to the LED matrix hardware.
 * 
 * @return led_matrix_err_t Status of the operation.
 */
led_matrix_err_t sendMatrix(void); 

/**
 * @brief Prints the current LED matrix to the terminal. This is a utility function for testing and visualization purposes.
 * 
 */
void printMatrix(void);
#endif /* __LEDMATRIX_H */



