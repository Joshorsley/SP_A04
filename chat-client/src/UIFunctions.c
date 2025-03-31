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
	printf("Incoming messages thread started.\n"); // ********************************REMOVE BEFORE SUBMISSION - DEBUG LINE ONLY
    ClientInfo* info = (ClientInfo*)clientInfo;

    receiveMessages(info->socketID);

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
    
    // border
    mvhline(3, 0, '-', cols); // print '-' on 3rd line
    const char *label = " Messages ";
    mvprintw(3, (cols - strlen(label)) / 2, "%s", label); // insert "MESSAGES" in the middle of border
    
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

void resetInputWin(WINDOW *inWin)
{
    werase(inWin);
    mvwprintw(inWin, 1, 2, "> ");
    wmove(inWin, 1, 4);  // Position cursor after prompt
    wrefresh(inWin);     // Show the changes
}

void getMsg(WINDOW *inWin, char *buf)
{
    resetInputWin(inWin);
    echo();             // 문자 에코 활성화
    curs_set(1);        // 커서 가시성 설정
    wmove(inWin, 1, 4); // 프롬프트 뒤에 커서 위치
    wrefresh(inWin);    // 커서 보이게 창 업데이트
    mvwgetnstr(inWin, 1, 4, buf, 80);
}

void endProg(WINDOW *inWin, WINDOW *outWin) {
    delwin(inWin); // Delete input window
    delwin(outWin); // Delete output window
    endwin(); // End ncurses mode
}