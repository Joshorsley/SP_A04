/* FILE :       UIFunctions.h
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :    Kalina Cathcart, Josh Horsley, John Paventi, Daimon Quinn, Jiwoo (Tony) Yang
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds the function prototypes for UIFunctions.c. These functions handle the ncurse library and related window functionability. 
 */

#include <stdbool.h>
#include <ncurses.h>
#include "main.h"

#ifndef UI_FUNCTIONS_H
#define UI_FUNCTIONS_H

void *incomingMessages(void* clientInfo);
void drawWin(WINDOW **inWin, WINDOW **outWin, int *maxPrintRow);
void drawBorder(WINDOW *win, const char *label);
void resetInputWin(WINDOW *inWin);
void getMsg(WINDOW *inWin, char *buf);
void printMsg(WINDOW *outWin, int row, char message[]);
void endProg(WINDOW *inWin, WINDOW *outWin);

#endif // UI_FUNCTIONS_H
