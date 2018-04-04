Application.ensure_started(:ex_ncurses)

ExNcurses.initscr()

# ExNcurses.start_color()
# ExNcurses.init_pair(1, :red, :black)
# ExNcurses.attron(1)
w = ExNcurses.newwin(20, 40, 5, 10)
ExNcurses.wborder(w)
ExNcurses.wmove(w, 5, 1)
ExNcurses.waddstr(w, "Hello, color. #{inspect(w)} Now press a key.")
ExNcurses.wrefresh(w)

# ExNcurses.attroff(1)
# ExNcurses.refresh()
ExNcurses.getch()
ExNcurses.endwin()
