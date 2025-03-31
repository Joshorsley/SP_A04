/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */

#include "programFunctions.h"
#include "messageFunctions.h"
#include "UIFunctions.h"


void *incomingMessages(void* clientInfo)
{
	//printf("Incoming messages thread started.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY

    return NULL;
}

void drawWin(WINDOW **inWin, WINDOW **outWin, int *msgRow, int *maxPrintRow)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
    // color
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    bkgd(COLOR_PAIR(1));
    
    *inWin = newwin(3, cols, 0, 0);
    *outWin = newwin(rows - 3, cols, 3, 0);
    
    wbkgd(*inWin, COLOR_PAIR(1));
    wbkgd(*outWin, COLOR_PAIR(1));
    
    mvwprintw(*inWin, 1, 2, "> ");
    
    // border
    mvhline(3, 0, '-', cols); // print '-' on 3rd line
    const char *label = " Messages ";
    mvprintw(3, (cols - strlen(label)) / 2, "%s", label); // insert "MESSAGES" in the middle of border
    
    getmaxyx(*outWin, *maxPrintRow, cols);
    *maxPrintRow -= 2;
    
    wmove(*inWin, 1, 4);

    wrefresh(*inWin);
    wrefresh(*outWin);
    refresh();
}

void resetInputWin(WINDOW *inWin)
{
    werase(inWin);
    mvwprintw(inWin, 1, 2, "> ");
    wrefresh(inWin);
}

void getMsg(WINDOW *inWin, char *buf)
{
    resetInputWin(inWin);
    mvwgetnstr(inWin, 1, 4, buf, 80);
}

// void printMsg(WINDOW *outWin, int row, Message msg)
// {
//     char timestamp[10];
//     time_t now = time(NULL);
//     struct tm *t = localtime(&now);
//     strftime(timestamp, sizeof(timestamp), "%H:%M:%S", t);
    
//     char formattedMsg[100];
    
//     snprintf(formattedMsg, sizeof(formattedMsg),"%-15s %s >> %-41s (%s)", 
//     msg.ip, msg.username, msg.message, timestamp);
    
//     mvwprintw(outWin, row, 1, formattedMsg);
//     wrefresh(outWin);
// }

void endProg(WINDOW *inWin, WINDOW *outWin) {
    delwin(inWin); // Delete input window
    delwin(outWin); // Delete output window
    endwin(); // End ncurses mode
}