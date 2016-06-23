defmodule ExNcurses do
  #@compile {:autoload, false}
  @on_load {:init, 0}

  def init() do
    Path.dirname(__DIR__)
    |> Path.join("priv/ex_ncurses")
    |> :erlang.load_nif(0)
  end

  def fun(:F1), do: 265
  def fun(:F2), do: 266
  def fun(:F3), do: 267
  def fun(:F4), do: 268
  def fun(:F5), do: 269
  def fun(:F6), do: 270
  def fun(:F7), do: 271
  def fun(:F8), do: 272
  def fun(:F9), do: 273
  def fun(:F10), do: 274
  def fun(:F11), do: 275
  def fun(:F12), do: 276

  # colors
  def clr(:COLOR_BLACK), do: 0
  def clr(:BLACK),       do: 0
  def clr(:COLOR_RED),   do: 1
  def clr(:RED),         do: 1
  def clr(:COLOR_GREEN), do: 2
  def clr(:GREEN),       do: 2
  def clr(:COLOR_YELLOW),do: 3
  def clr(:YELLOW),      do: 3
  def clr(:COLOR_BLUE),  do: 4
  def clr(:BLUE),        do: 4
  def clr(:COLOR_MAGENTA),do:	5
  def clr(:MAGENTA),     do:	5
  def clr(:COLOR_CYAN),  do: 6
  def clr(:CYAN),        do: 6
  def clr(:COLOR_WHITE), do: 7
  def clr(:WHITE),       do: 7

  def ex_initscr(),  do: raise ExNcursesNifNotLoaded
  def ex_endwin(),   do: raise ExNcursesNifNotLoaded

  def ex_printw(_s), do: raise ExNcursesNifNotLoaded
  def ex_mvprintw(_y, _x, _s), do: raise ExNcursesNifNotLoaded

  def ex_refresh(),  do: raise ExNcursesNifNotLoaded
  def ex_clear(),    do: raise ExNcursesNifNotLoaded

  def ex_raw(),      do: raise ExNcursesNifNotLoaded
  def ex_cbreak(),   do: raise ExNcursesNifNotLoaded
  def ex_nocbreak(), do: raise ExNcursesNifNotLoaded

  def ex_noecho(),   do: raise ExNcursesNifNotLoaded

  def cols(),        do: raise ExNcursesNifNotLoaded
  def lines(),       do: raise ExNcursesNifNotLoaded
  def ex_getx(),     do: raise ExNcursesNifNotLoaded
  def ex_gety(),     do: raise ExNcursesNifNotLoaded

  def ex_flushinp(), do: raise ExNcursesNifNotLoaded
  def ex_keypad(),   do: raise ExNcursesNifNotLoaded

  def ex_start_color(), do: raise ExNcursesNifNotLoaded
  def ex_init_pair(_pair,_f,_b),do: raise ExNcursesNifNotLoaded

  def ex_getch(),    do: raise ExNcursesNifNotLoaded

  def getchar() do
    do_getchar(ex_getch())
  end
  def do_getchar(-1), do: getchar()
  def do_getchar(c), do: c

  # not implemented in nif
  def ex_getstr(),   do: do_getstr([], ex_getx(), getchar())
  def do_getstr(str, _x, 10) do
    String.reverse(List.to_string(['\n' | str]))
  end
  def do_getstr(str, x, chr) do
    case chr do
      127 ->
        handle_delete(x)
        [_h|t] = str
        do_getstr(t, x, getchar())
      -1 ->
        do_getstr(str, x, getchar())
      _  ->
        do_getstr([chr | str], x, getchar())
    end
  end

  # Simple minded positioning. Does not account for line wrap.
  defp handle_delete(x) do
    cx = ex_getx()
    cy = ex_gety()
    nx = max(x, cx-1)
    ex_mvprintw(cy, nx, " ") # remove ?
    nx = max(x, nx-1)
    ex_mvprintw(cy, nx, " ") # remove ^
    nx = max(x, nx-1)
    ex_mvprintw(cy, nx, " ") # remove <char>
    ex_mvprintw(cy, nx, "")  # move not implemented
  end

  # Common initialization
  def ncurses_begin() do
    ex_initscr()
    ex_raw()
    ex_cbreak()
  end

  def ncurses_end() do
    ex_nocbreak()
    ex_endwin()
  end

end
