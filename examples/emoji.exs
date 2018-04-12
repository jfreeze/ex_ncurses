Application.ensure_started(:ex_ncurses)

ExNcurses.initscr()

ExNcurses.mvprintw(4, 8, "Hello, 🐄\n")
ExNcurses.refresh()
ExNcurses.getch()
ExNcurses.endwin()
