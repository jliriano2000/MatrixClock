# MatrixClock
Creates a simple clock using an LED matrix display. The clock can display the current time, an alarm time, and individual digits for testing purposes. The user can interact with the clock using buttons to switch between different display modes. The clock uses a state machine to manage the different display states and timers to control how long each state is displayed.

## Button Functionality
The buttons on the clock are simulated using keyboard input, where the following keys are used:
- 'a' : Clears the alarm time. 
- 'A' : Sets the alarm time to 5:30 (as an example).
- 'n' or 'N': Switch to displaying a single digit for testing purposes (the digit displayed corresponds to the number of button presses, cycling from 0 to 9)
- 'd' or 'D': Switch back to displaying the single digit mode.
- 'q' or 'Q': Quit the program. 

## Compiling and Running
To compile the program, use the following command in the terminal:

```gcc main.c ledMatrix.c timeFuncs.c -lpthread -Wno-comment -o ledMatrix.out ```

To run the program, use the following command:

```./ledMatrix.out```

This was tested and compiled on a MacBook Pro running macOS Sonoma. The program uses the ncurses library for handling keyboard input and display, so make sure to have it installed on your system.

This has not been tested on Windows, but the code exists to compile and run on Windows as well. You may need to adjust the compilation command to link against the appropriate libraries for Windows (e.g., using MinGW or Visual Studio).