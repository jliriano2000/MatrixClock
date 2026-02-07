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
        default:
            break;
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
    // Set the characters at the specified positions
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

int main(void) {
    pthread_t getInputThread;
    int threadStatus = 0; 

    // Create a thread to manage user input
    threadStatus = pthread_create(&getInputThread, NULL, input_thread, NULL);
    if (threadStatus != 0) {
        printf("Error creating input thread: %d\n", threadStatus);
        return -1; 
    }

    // Initialize the tick counter
    initTick();

    // Initialize the LED matrix
    clearMatrix();    

    while(!isQuit) {
        // Get the current time
        getTime(&localTime);

        switch(clockState) {
            case DISPLAY_TIME:
                setTimeDisplay(&localTime);
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

