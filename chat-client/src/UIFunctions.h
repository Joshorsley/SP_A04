/* FILE :
 * PROJECT :    SP A4 : Can We Talk System
 * AUTHORS :
 * DATE :       2025 - 03 -22
 * DESCRIPTION : This file holds
 */

#include <stdbool.h>
#include <ncurses.h>

typedef struct {
    int height;
    int width;
    int starty;
    int startx;
} UIProps;

bool incomingThreadCreate();
bool outgoingThreadCreate();
bool displayMessage();

WINDOW *createNewWin(int, int, int, int);
bool byeCheck(char *str);
void input(WINDOW *Win, char *buf);
void output(WINDOW *Win, char *buf);
void endProg(WINDOW *inWin, WINDOW *outWin);