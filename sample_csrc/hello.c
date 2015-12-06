#include <ncurses.h>

int main(int argc, char *argv[])
{
	  WINDOW *my_win;
	  int startx, starty, width, height, ch;

    initscr();                      /* Start curses mode            */
    noecho();
    cbreak();                       /* Line buffering disabled, Pass on
                                     * everty thing to me           */
    keypad(stdscr, TRUE);           /* I need that nifty F1         */

    height = 3;
    width = 10;
    starty = (LINES - height) / 2;  /* Calculating for a center placement */
    startx = (COLS - width) / 2;    /* of the window                */
    mvprintw(starty-1, startx, "Hello World");
    mvprintw(starty, startx, "Press any any to exit");
    refresh();

	  ch = getch();

	  endwin();			                  /* End curses mode		  */
    return 0;
}
