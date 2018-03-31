Application.ensure_started(:ex_ncurses)

defmodule Border do
  @moduledoc """
  Replica of border.c in sample_csrc directory

  This code is meant to follow the C code as
  close as reasonable.
  """

  def run do
    # Start curses mode
    ExNcurses.initscr()
    # Start the color functionality
    ExNcurses.start_color()
    # Line buffering disabled, Pass on
    ExNcurses.cbreak()
    # everty thing to me

    # I need that nifty F1
    ExNcurses.keypad()
    ExNcurses.noecho()
    ExNcurses.init_pair(1, :cyan, :black)

    # 	/* Initialize the window parameters */
    # 	init_win_params(&win);
    # 	print_win_params(&win);
    #
    ExNcurses.attron(1);
    ExNcurses.border()
    ExNcurses.mvprintw(10, 10, "Press F1 to exit")
    ExNcurses.refresh()
    Process.sleep(3000)
    # 	attroff(COLOR_PAIR(1));
    #
    # 	create_box(&win, TRUE);
    # 	while((ch = getch()) != KEY_F(1))
    # 	{	switch(ch)
    # 		{	case KEY_LEFT:
    # 				create_box(&win, FALSE);
    # 				--win.startx;
    # 				create_box(&win, TRUE);
    # 				break;
    # 			case KEY_RIGHT:
    # 				create_box(&win, FALSE);
    # 				++win.startx;
    # 				create_box(&win, TRUE);
    # 				break;
    # 			case KEY_UP:
    # 				create_box(&win, FALSE);
    # 				--win.starty;
    # 				create_box(&win, TRUE);
    # 				break;
    # 			case KEY_DOWN:
    # 				create_box(&win, FALSE);
    # 				++win.starty;
    # 				create_box(&win, TRUE);
    # 				break;
    # 		}
    # 	}
    # End curses mode
    ExNcurses.endwin()
  end
end

Border.run()
