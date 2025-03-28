/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */

#include "programFunctions.h"
#include "messageFunctions.h"
#include "UIFunctions.h"


bool incomingThreadCreate()
{
	// Create a thread to handle incoming message window
	// If the thread is created, return true
	// If the thread is not created, return false
	return true;
}

bool outgoingThreadCreate()
{
	// Create a thread to handle outgoing message window 
	// If the thread is created, return true
	// If the thread is not created, return false
	return true;
}

bool displayMessage()
{

	// Display the message in the Output Window
	// If the message is displayed, return true
	// If the message is not displayed, return false
	/*
	 __
 .--()°'.'
'|, . ,'		NCurses Be needed here
 !_-(_\

*/
	return true;
}

WINDOW *createNewWin(int height, int width, int starty, int startx) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx); // Create a new window
    box(local_win, 0, 0); // Draw a box around the window
    wrefresh(local_win); // Refresh the window to show changes

    return local_win; // Return the created window
}

bool byeCheck(char *str) {
    // Check if the string is "exit" (case insensitive)
    if (strncasecmp(str, ">>bye<<", 4) == 0) {
        return true;
    }
    return false; // Return false if not "exit"
}

void input(WINDOW *Win, char *buf) {
    mvwgetnstr(Win, 1, 1, buf, 255); // Get user input (max 255 characters)
    wclear(Win);                     // Clear the input window
    box(Win, 0, 0);                  // Redraw the border
    wrefresh(Win);                   // Refresh the window
}

void output(WINDOW *Win, char *buf) {
    static int outRow = 1; // Start printing from the first row
    if (outRow >= getmaxy(Win) - 1) { // If outWin is full, scroll
        scroll(Win);
        outRow = getmaxy(Win) - 2; // Adjust row after scrolling
    }
    mvwprintw(Win, outRow++, 1, "%s", buf); // Print input on the current row
    wrefresh(Win);                          // Refresh the output window
}

void endProg(WINDOW *inWin, WINDOW *outWin) {
    delwin(inWin); // Delete input window
    delwin(outWin); // Delete output window
    endwin(); // End ncurses mode
}
