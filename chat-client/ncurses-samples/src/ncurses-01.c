/* splitWin.c is a simple example to show how to deal with split screens.
   Due to the limited time, this program is not finished yet.

   To compile:   gcc splitWin.c -lncurses

		Sam Hsu (11/17/10)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>
     
WINDOW *create_newwin(int, int, int, int);
void destroy_win(WINDOW *);
void input_win(WINDOW *, char *);
void display_win(WINDOW *, char *, int, int);
void destroy_win(WINDOW *win);
void blankWin(WINDOW *win);
     
int main(void)
{
  WINDOW *chat_win, *msg_win;
  int chat_startx, chat_starty, chat_width, chat_height;
  int msg_startx, msg_starty, msg_width, msg_height, i;
  int shouldBlank;
  char buf[BUFSIZ];

  initscr();                      /* Start curses mode            */
  cbreak();
  noecho();
  refresh();
  
  shouldBlank = 0;

  chat_height = 5;
  chat_width  = COLS - 2;
  chat_startx = 1;        
  chat_starty = LINES - chat_height;        
     
  msg_height = LINES - chat_height - 1;
  msg_width  = COLS;
  msg_startx = 0;        
  msg_starty = 0;        
  
  /* create the input window */
  msg_win = create_newwin(msg_height, msg_width, msg_starty, msg_startx);
  scrollok(msg_win, TRUE);

  /* create the output window */
  chat_win = create_newwin(chat_height, chat_width, chat_starty, chat_startx);
  scrollok(chat_win, TRUE);

  /* allow the user to input 5 messages for display */ 
  for(i=0; i<5; i++)
  {    
    input_win(chat_win, buf);
    display_win(msg_win, buf, i, shouldBlank);
  }
  sleep(3);                   /* to get a delay */

  /* tell the user that the 5 messages are done ... */
  shouldBlank = 1;
  sprintf(buf,"Messaging is complete ... destroying window in 5 seconds");
  display_win(msg_win, buf, 0, shouldBlank);
  
  sleep(5);                   /* to get a delay */
     
  destroy_win(chat_win);
  destroy_win(msg_win);
  endwin();
}
     
WINDOW *create_newwin(int height, int width, int starty, int startx)
{       
  WINDOW *local_win;
     
  local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);               /* draw a box */
  wmove(local_win, 1, 1);             /* position cursor at top */
  wrefresh(local_win);     
  return local_win;
}
     
/* This function is for taking input chars from the user */
void input_win(WINDOW *win, char *word)
{
  int i, ch;
  int maxrow, maxcol, row = 1, col = 0;
     
  blankWin(win);                          /* make it a clean window */
  getmaxyx(win, maxrow, maxcol);          /* get window size */
  bzero(word, BUFSIZ);
  wmove(win, 1, 1);                       /* position cusor at top */
  for (i = 0; (ch = wgetch(win)) != '\n'; i++) 
  {
    word[i] = ch;                       /* '\n' not copied */
    if (col++ < maxcol-2)               /* if within window */
    {
      wprintw(win, "%c", word[i]);      /* display the char recv'd */
    }
    else                                /* last char pos reached */
    {
      col = 1;
      if (row == maxrow-2)              /* last line in the window */
      {
        scroll(win);                    /* go up one line */
        row = maxrow-2;                 /* stay at the last line */
        wmove(win, row, col);           /* move cursor to the beginning */
        wclrtoeol(win);                 /* clear from cursor to eol */
        box(win, 0, 0);                 /* draw the box again */
      } 
      else
      {
        row++;
        wmove(win, row, col);           /* move cursor to the beginning */
        wrefresh(win);
        wprintw(win, "%c", word[i]);    /* display the char recv'd */
      }
    }
  }
}  /* input_win */
     
void display_win(WINDOW *win, char *word, int whichRow, int shouldBlank)
{
  if(shouldBlank == 1) blankWin(win);                /* make it a clean window */
  wmove(win, (whichRow+1), 1);                       /* position cusor at approp row */
  wprintw(win, word);
  wrefresh(win);
} /* display_win */
     
void destroy_win(WINDOW *win)
{
  delwin(win);
}  /* destory_win */
     
void blankWin(WINDOW *win)
{
  int i;
  int maxrow, maxcol;
     
  getmaxyx(win, maxrow, maxcol);
  for (i = 1; i < maxcol-2; i++)  
  {
    wmove(win, i, 1);
    refresh();
    wclrtoeol(win);
    wrefresh(win);
  }
  box(win, 0, 0);             /* draw the box again */
  wrefresh(win);
}  /* blankWin */

