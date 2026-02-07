/** ********************************************************************************
*@file main.c
*
*@date February 5th, 2026
*
*@author julio.liriano (julio.liriano@gmail.com)
*
*@brief - Main entry point for the LED matrix clock application. Emulates a clock 
*         with an alarm feature using a LED matrix display. The application runs in a loop,
*         updating the display based on the current time and user input. User can set/clear
*         the alarm and toggle between displaying the current time, alarm time, or a test digit.
********************************************************************************


/* Includes ------------------------------------------------------------------*/
#include "ledMatrix.h"
#include "timeFuncs.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

// For non-blocking input (is OS dependent)
#if defined(_WIN64) || defined(_WIN32)
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h> // for read()
#endif

/* Imported variables --------------------------------------------------------*/

/* Imported function prototypes ----------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
#define ALARM_DISPLAY_DURATION_MS 2000 // Duration to display the alarm time in milliseconds
#define DIGIT_DISPLAY_DURATION_MS 5000 // Duration to display a single digit in milliseconds
/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

// State machine states for what to display on the LED matrix
typedef enum {
    DISPLAY_TIME = 0,
    DISPLAY_ALARM_TIME = 1, 
    DISPLAY_DIGIT = 2
} display_state_t;

/* Private variables ---------------------------------------------------------*/
static struct tm localTime;
static bool isQuit = false; // Flag to signal the input thread to exit
static bool isAlarmSet = false; // Flag to track if the alarm is set or not
static bool isDisplayAlarm = false; // Flag to track whether we are currently displaying the alarm time or not.
static bool isDisplayDigit = false; // Flag to track whether we are currently displaying a single digit for testing purposes.   
static uint8_t buttonCnt = 0; // Counter to track the number of button presses for testing purposes.
static display_state_t clockState = DISPLAY_TIME;
/* Private functions ---------------------------------------------------------*/

/**
 * @brief Processes a button press input.
 * 
 * @param button - Character representing the button pressed.
 */
static void processButtonPress(char button);

/**
 * @brief Thread to manage receiving input from the user. Models an EXTI interrupt from 
 *        a button press on hardware. 
 * 
 * @param ptr 
 * @return void* 
 */
static void *input_thread(void *ptr); 

/**
 * @brief Sets the characters on the LED matrix to display the time based on the provided tm structure.
 * 
 * @param timeInfo - Pointer to a tm structure that contains the time information to be displayed.
 */
static void setTimeDisplay(struct tm *timeInfo); 

/**
 * @brief Display the alarm time on the LED matrix. 
 * 
 */
static void setAlarmDisplay(void); 

/**
 * @brief Sets a single digit character on the LED matrix.
 * 
 * @param digit - The digit (0-9) to be displayed on the LED matrix. 
 */
static void setDigitDisplay(uint8_t digit); 

/* Definitions ---------------------------------------------------------------*/
static void processButtonPress(char button)
{
    switch(button) {
        case 'q': // Quit on 'q' key press
        case 'Q':
            //Intentional fall-through to set quit flag
            isQuit = true;
            break;
        case 'a': // lower case 'a' will clear the alarm. 
            isAlarmSet = false;
            break;
        case 'A': // Upper case 'A' will set the alarm.
            isAlarmSet = true;
            break;
        case 'd':
        case 'D':
            // Intentional fall-through. 
            // Set flag to display alarm time.             
            isDisplayAlarm = true;
            break;
        case 'n':
        case 'N':
            // Intentional fall-through. 
            // Set flag to display a single digit for testing purposes.             
            isDisplayDigit = true;
            break;
        default:
            break;
    }
    // Count number of button presses. 
    buttonCnt++;
    if(buttonCnt > 10) {
        buttonCnt = 0;
    }
}

// Strictly for making getting input from the user non-blocking. 
// This is unfortunately OS dependent. I have implemented for both Windows and Unix-like systems.
#if defined(_WIN64) || defined(_WIN32)
void *input_thread(void *ptr) {
    char button = 0; 
    
    while(!isQuit) {
        button = _getch(); // Read a single character from stdin
        processButtonPress(button); // Process the button press (e.g., toggle alarm dot)

    }
    return NULL;
}
#else
void *input_thread(void *ptr) {
    char button = 0; 
    struct termios oldt, newt;
    ssize_t bytesRead = 0;

    // Get current terminal settings and save them
    tcgetattr(STDIN_FILENO, &oldt);

    // Modify settings: disable canonical mode (ICANON) and echo (ECHO)
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    
    // Apply the new settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while(!isQuit) {
        bytesRead = read(STDIN_FILENO, &button, 1); // Read a single character from stdin
        if(bytesRead > 0) {
            processButtonPress(button); // Process the button press (e.g., toggle alarm dot)
        }
    }

    // Restore the old terminal settings before exiting the thread
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return NULL;
}
#endif

static void setTimeDisplay(struct tm *timeInfo) {
    // Set the time characters at the specified positions
    setCharacterAtPosition(timeInfo->tm_hour / 10, POS1);
    setCharacterAtPosition(timeInfo->tm_hour % 10, POS2);
    setCharacterAtPosition(COLON_CHAR, COLON);
    setCharacterAtPosition(timeInfo->tm_min / 10, POS3);
    setCharacterAtPosition(timeInfo->tm_min % 10, POS4);

    // Set the alarm dot based on the isAlarmSet flag
    if(isAlarmSet) {
        setCharacterAtPosition(ALARM_CHAR_SET, ALARM_DOT);
    } else {
        setCharacterAtPosition(ALARM_CHAR_CLR, ALARM_DOT);
    }
}

static void setAlarmDisplay(void)
{
    if(isAlarmSet) {
        // Display example alarm time of 5:30 am. 
        setCharacterAtPosition(FIVE_CHAR, POS2);
        setCharacterAtPosition(COLON_CHAR, COLON);
        setCharacterAtPosition(THREE_CHAR, POS3);
        setCharacterAtPosition(ZERO_CHAR, POS4);

        setCharacterAtPosition(ALARM_CHAR_SET, ALARM_DOT);
    }
    else {
        setCharacterAtPosition(DASH_CHAR, POS1);
        setCharacterAtPosition(DASH_CHAR, POS2);
        setCharacterAtPosition(COLON_CHAR, COLON);
        setCharacterAtPosition(DASH_CHAR, POS3);
        setCharacterAtPosition(DASH_CHAR, POS4);

        setCharacterAtPosition(ALARM_CHAR_CLR, ALARM_DOT);
    }
}

static void setDigitDisplay(uint8_t digit) {
    // For testing purposes, we can set a single digit (e.g., '5') at POS1
    digit = digit % 10; // Ensure the input is a single digit (0-9)
    switch(digit) {
        case 0:
            setCharacterAtPosition(ZERO_CHAR, POS4);
            break;
        case 1:
            setCharacterAtPosition(ONE_CHAR, POS4);
            break;
        case 2:
            setCharacterAtPosition(TWO_CHAR, POS4);
            break;
        case 3:
            setCharacterAtPosition(THREE_CHAR, POS4);
            break;
        case 4:
            setCharacterAtPosition(FOUR_CHAR, POS4);
            break;
        case 5:
            setCharacterAtPosition(FIVE_CHAR, POS4);
            break;
        case 6:
            setCharacterAtPosition(SIX_CHAR, POS4);
            break;
        case 7:
            setCharacterAtPosition(SEVEN_CHAR, POS4);
            break;
        case 8:
            setCharacterAtPosition(EIGHT_CHAR, POS4);
            break;
        case 9:
            setCharacterAtPosition(NINE_CHAR, POS4);
            break;
        default:
            // If the input is not a valid digit, we can choose to clear the position or do nothing
            // For this example, let's clear the position by setting it to a blank character (e.g., DASH)
            setCharacterAtPosition(DASH_CHAR, POS4);
            break;
    }
}

int main(void) {
    pthread_t getInputThread;
    int threadStatus = 0;
    uint64_t stateTimer = 0; 

    // Create a thread to manage user input
    threadStatus = pthread_create(&getInputThread, NULL, input_thread, NULL);
    if (threadStatus != 0) {
        printf("Error creating input thread: %d\n", threadStatus);
        return -1; 
    }

    // Initialize the tick counter
    initTick();

    while(!isQuit) {
        // Get the current time
        getTime(&localTime);

        // Initialize the LED matrix
        clearMatrix();    

        switch(clockState) {
            case DISPLAY_TIME:
                setTimeDisplay(&localTime);
                if(isDisplayAlarm) {
                    clockState = DISPLAY_ALARM_TIME;
                    stateTimer = getTick(); // Reset the timer when we switch to alarm display
                    isDisplayAlarm = false; // Reset the flag to display alarm time so that we only switch to alarm display once per button press.
                }
                else if (isDisplayDigit) {
                    clockState = DISPLAY_DIGIT;
                    stateTimer = getTick(); // Reset the timer when we switch to digit display
                    isDisplayDigit = false; // Reset the flag to display digit so that we only switch
                }  
                break;
            case DISPLAY_ALARM_TIME:
                setAlarmDisplay();
                // After 2 seconds of displaying the alarm time, switch back to displaying the current time
                if(getTick() - stateTimer >= ALARM_DISPLAY_DURATION_MS) {
                    clockState = DISPLAY_TIME;
                }
                break;
            case DISPLAY_DIGIT:
                setDigitDisplay(buttonCnt); // For testing purposes, we can display the digit '5
                // After 2 seconds of displaying the digit, switch back to displaying the current time
                if(getTick() - stateTimer >= DIGIT_DISPLAY_DURATION_MS) {
                    clockState = DISPLAY_TIME;
                }
                break;
            default:
                // Should never be here but force state to display clock
                clockState = DISPLAY_TIME;
                continue;
                break;
        }

        // Print the LED matrix to the terminal for visualization
        printMatrix();

        // Update every 100 msec. 
        delayMsec(100); 
    }

    // Join the input thread
    pthread_join(getInputThread, NULL);
    return 0;
}

