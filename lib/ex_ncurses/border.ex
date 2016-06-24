defmodule ExNcurses.Border do
  @moduledoc """
  Replica of border.c in sample_csrc directory

  This code is meant to follow the C code as
  close as reasonable.
  """

  def run do
	ExNcurses.initscr()			# Start curses mode
	ExNcurses.start_color();	# Start the color functionality
	ExNcurses.cbreak();			# Line buffering disabled, Pass on
                    # everty thing to me
	ExNcurses.keypad()		    # I need that nifty F1
	ExNcurses.noecho()
	ExNcurses.init_pair(1, ExNcurses.clr(:COLOR_CYAN), ExNcurses.clr(:COLOR_BLACK))

#	/* Initialize the window parameters */
#	init_win_params(&win);
#	print_win_params(&win);
#
#	attron(COLOR_PAIR(1));
	ExNcurses.printw("Press F1 to exit")
	ExNcurses.refresh()
  :timer.sleep 3000
#	attroff(COLOR_PAIR(1));
#
#	create_box(&win, TRUE);
#	while((ch = getch()) != KEY_F(1))
#	{	switch(ch)
#		{	case KEY_LEFT:
#				create_box(&win, FALSE);
#				--win.startx;
#				create_box(&win, TRUE);
#				break;
#			case KEY_RIGHT:
#				create_box(&win, FALSE);
#				++win.startx;
#				create_box(&win, TRUE);
#				break;
#			case KEY_UP:
#				create_box(&win, FALSE);
#				--win.starty;
#				create_box(&win, TRUE);
#				break;
#			case KEY_DOWN:
#				create_box(&win, FALSE);
#				++win.starty;
#				create_box(&win, TRUE);
#				break;
#		}
#	}
	ExNcurses.endwin()			# End curses mode
  end
end
