/* FILE :	UIFunctions.c
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :	Kalina Cathcart, Josh Horsley, John Paventi, Daimon Quinn, Jiwoo (Tony) Yang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds the functions related to the UI windows and the ncurse library functionality for the program. 
 */

#include "programFunctions.h"
#include "messageFunctions.h"
#include "UIFunctions.h"
#include "main.h"

/*
* FUNCTION:     incomingMessages
* DESCRIPTION:  Thread function that continuously receives incoming messages from the server.
*               Passes the ClientInfo structure to the receiveMessages function.
* PARAMETERS:   void* clientInfo - pointer to ClientInfo structure (cast from thread argument)
* RETURNS:      NULL (void pointer)
*/
void *incomingMessages(void* clientInfo)
{
    ClientInfo* info = (ClientInfo*)clientInfo;
    receiveMessages(clientInfo);
    return NULL;
}


/*
* FUNCTION:     drawWin
* DESCRIPTION:  This function initializes and draws the ncurses windows for the chat interface.
*               It creates input and output windows with proper sizing and formatting.
*               The function will also set up color schemes and borders for the UI.
* PARAMETERS:   WINDOW **inWin - pointer to input window pointer
*               WINDOW **outWin - pointer to output window pointer
*               int *msgRow - pointer to store starting message row
*               int *maxPrintRow - pointer to store maximum printable rows
* RETURNS:      void - nothing is returned 
*/
void drawWin(WINDOW **inWin, WINDOW **outWin, int *msgRow, int *maxPrintRow)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    bkgd(COLOR_PAIR(1));
    
    *inWin = newwin(3, cols, 0, 0);
    *outWin = newwin(rows - 3, cols, 3, 0);
    
    wbkgd(*inWin, COLOR_PAIR(1));
    wbkgd(*outWin, COLOR_PAIR(1));
    
    mvhline(3, 0, '-', cols); 
    const char *label = " Messages ";
    mvprintw(3, (cols - strlen(label)) / 2, "%s", label); 
    
    getmaxyx(*outWin, *maxPrintRow, cols);
    *maxPrintRow -= 2;
    
    keypad(*inWin, TRUE);

    wrefresh(*outWin);
    refresh();

    mvwprintw(*inWin, 1, 2, "> ");
    wmove(*inWin, 1, 4);
    curs_set(1);
    wrefresh(*inWin);
}


/*
* FUNCTION:     resetInputWin
* DESCRIPTION:  This function clears and resets the input window to its initial state.
*               It will redraw the prompt and positions the cursor for new input.
* PARAMETERS:   WINDOW *inWin - pointer to the input window
* RETURNS:      void - nothing is returned
*/
void resetInputWin(WINDOW *inWin)
{
    werase(inWin);
    mvwprintw(inWin, 1, 2, "> ");
    wmove(inWin, 1, 4);  
    wrefresh(inWin);     
}



/*
* FUNCTION:     getMsg
* DESCRIPTION:  This function gets user input from the input window and stores it in a buffer.
*               It will handle cursor positioning and echo settings to properly handle input.
* PARAMETERS:   WINDOW *inWin - pointer to the input window
*               char *buf - buffer to store the user's input message
* RETURNS:      void - nothing is returned
*/
void getMsg(WINDOW *inWin, char *buf)
{
    resetInputWin(inWin);
    echo();            
    curs_set(1);       
    wmove(inWin, 1, 4); 
    wrefresh(inWin);    
    mvwgetnstr(inWin, 1, 4, buf, 80);
}


/*
* FUNCTION:     printMsg
* DESCRIPTION:  This function prints a formatted message to the output window at the specified row.
*               It also ensures proper formatting and refreshes the window to display the message.
* PARAMETERS:   WINDOW *outWin - pointer to the output window
*               int row - row number where the message should be printed
*               char message[] - message string to be printed
* RETURNS:      void - nothing is returned
*/
void printMsg(WINDOW *outWin, int row, char message[])
{
    char formattedMsg[BUFFER_SIZE];
    snprintf(formattedMsg, sizeof(formattedMsg), "%s", message);

    mvwprintw(outWin, row, 1, formattedMsg);
    wrefresh(outWin);
}


/*
* FUNCTION:     endProg
* DESCRIPTION:  This final function cleans up and terminates the ncurses interface properly.
*               It will deallocate the 2 windows and ends ncurses mode.
* PARAMETERS:   WINDOW *inWin - pointer to the input window
*               WINDOW *outWin - pointer to the output window
* RETURNS:      void - nothing is returned
*/
void endProg(WINDOW *inWin, WINDOW *outWin)
{
    delwin(inWin); 
    delwin(outWin); 
    endwin(); 
}
