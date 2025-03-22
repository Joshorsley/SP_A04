#include <stdio.h>
#include <ncurses.h>

void main()
{
    WINDOW *vin;
    
    initscr();
    refresh();
    start_color();
    
    init_pair(1, COLOR_YELLOW, COLOR_BLUE);
    init_pair(2, COLOR_BLUE, COLOR_YELLOW);
    init_pair(3, COLOR_BLUE, COLOR_WHITE); 
    
    vin=newwin(12,40,13,0);
    wmove(vin,0,5);		// position output cursor in window
    wprintw(vin,"Hello, World.");
    wbkgd(vin,COLOR_PAIR(1));
    wrefresh(vin);		// present the window
    
    getch();

    wmove(vin,3,5);
    wprintw(vin,"How are you?");
    wbkgd(vin,COLOR_PAIR(2));
    wrefresh(vin);

    getch();

    wmove(vin,5,5);
    wprintw(vin,"The END.");
    wbkgd(vin,COLOR_PAIR(3));
    wrefresh(vin);

    getch();

    delwin(vin);
    endwin();
}

