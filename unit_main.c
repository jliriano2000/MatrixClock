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
#include "timeFuncs.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Imported variables --------------------------------------------------------*/

/* Imported function prototypes ----------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define MAX_CHAR_TEST_CASES 6
#define NUM_TEST_CASES 2
/* Private types -------------------------------------------------------------*/
typedef struct {
    character_t character;
    char_pos_t position;
} char_test_case_t;


typedef struct {
    char_test_case_t charTestCases[MAX_CHAR_TEST_CASES];
    size_t numCharTestCases;
    uint8_t const (*expectedMatrix)[MATRIX_HEIGHT][MATRIX_WIDTH];
} test_case_t;

/* Private variables ---------------------------------------------------------*/
const uint8_t ledMatrixExpected[NUM_TEST_CASES][MATRIX_HEIGHT][MATRIX_WIDTH] = {

    [0] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },

    [1] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }

};

// Define 
test_case_t testCases[NUM_TEST_CASES] = {
    [0] = {
        .charTestCases = {
            {ZERO_CHAR, POS1},
            {ZERO_CHAR, POS2},
            {COLON_CHAR, COLON},
            {ZERO_CHAR, POS3},
            {ZERO_CHAR, POS4},
        },
        .numCharTestCases = 5,
        .expectedMatrix = (uint8_t const (*)[MATRIX_HEIGHT][MATRIX_WIDTH]) &ledMatrixExpected[0]
    },
    [1] = {
        .charTestCases = {
            {ONE_CHAR, POS1},
            {ONE_CHAR, POS2},
            {COLON_CHAR, COLON},
            {ONE_CHAR, POS3},
            {ONE_CHAR, POS4},
            {ALARM_CHAR_SET, ALARM_DOT}
        },
        .numCharTestCases = 6,
        .expectedMatrix = (uint8_t const (*)[MATRIX_HEIGHT][MATRIX_WIDTH]) &ledMatrixExpected[1]
    }
    
};

/* Private functions ---------------------------------------------------------*/


/* Definitions ---------------------------------------------------------------*/

int main(void) {
    uint8_t testMatrix[MATRIX_HEIGHT][MATRIX_WIDTH] = {0};

    for(uint8_t i =0; i < NUM_TEST_CASES; i++) {
        clearMatrix();
        for(size_t j = 0; j < testCases[i].numCharTestCases; j++) {
            char_test_case_t testCase = testCases[i].charTestCases[j];
            led_matrix_err_t status = setCharacterAtPosition(testCase.character, testCase.position);
            if(status != LED_OK) {
                printf("Error setting character at position. character=%d, position=%d, status=%d\n", testCase.character, testCase.position, status);
            }
        }
        getMatrix(testMatrix);
        // Compare testMatrix with expected matrix
        if(memcmp(testMatrix, testCases[i].expectedMatrix, sizeof(testMatrix)) == 0) {
            printf("Test case %d passed.\n", i);
        } else {
            printf("Test case %d failed.\n", i);
        }
    }
    return 0;
}

