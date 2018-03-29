Application.ensure_started(:ex_ncurses)

ExNcurses.initscr()

if !ExNcurses.has_colors() do
  ExNcurses.endwin()
  IO.puts("Your terminal does not support color")
  exit(:shutdown)
end

ExNcurses.start_color()
ExNcurses.init_pair(1, ExNcurses.clr(:RED), ExNcurses.clr(:BLACK))
ExNcurses.attron(1)
ExNcurses.mvprintw(1, 1, "Hello, color. Now press a key.")
ExNcurses.attroff(1)
ExNcurses.refresh()
ExNcurses.getch()
ExNcurses.endwin()
