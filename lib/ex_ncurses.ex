defmodule ExNcurses do
  @compile {:autoload, false}
  @on_load {:init, 0}

  def init() do
    path = :filename.join(:code.priv_dir(:ex_ncurses), 'ncurses_nif')
    case :erlang.load_nif(path, 0) do
      :ok -> :ok
      error -> {:error, error}
    end
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

  def initscr(),          do: ex_initscr()
  def endwin(),           do: ex_endwin()

  def printw(s),          do: ex_printw(s)
  def mvprintw(y, x, s),  do: ex_mvprintw(y, x, s)

  def refresh(),          do: ex_refresh()
  def clear(),            do: ex_clear()

  def raw(),              do: ex_raw()
  def cbreak(),           do: ex_cbreak()
  def nocbreak(),         do: ex_nocbreak()

  def noecho(),           do: ex_noecho()

  def getx(),             do: ex_getx()
  def gety(),             do: ex_gety()

  def flushinp(),         do: ex_flushinp()
  def keypad(),           do: ex_keypad()

  def start_color(),      do: ex_start_color()
  def init_pair(pair, f, b), do: ex_init_pair(pair, f, b)

  def getch(),            do: ex_getch()

  def ex_initscr(),             do: exit(:nif_library_not_loaded)
  def ex_endwin(),              do: exit(:nif_library_not_loaded)

  def ex_printw(_s),            do: exit(:nif_library_not_loaded)
  def ex_mvprintw(_y, _x, _s),  do: exit(:nif_library_not_loaded)

  def ex_refresh(),             do: exit(:nif_library_not_loaded)
  def ex_clear(),               do: exit(:nif_library_not_loaded)

  def ex_raw(),                 do: exit(:nif_library_not_loaded)
  def ex_cbreak(),              do: exit(:nif_library_not_loaded)
  def ex_nocbreak(),            do: exit(:nif_library_not_loaded)

  def ex_noecho(),              do: exit(:nif_library_not_loaded)

  def ex_getx(),                do: exit(:nif_library_not_loaded)
  def ex_gety(),                do: exit(:nif_library_not_loaded)

  def ex_flushinp(),            do: exit(:nif_library_not_loaded)
  def ex_keypad(),              do: exit(:nif_library_not_loaded)

  def ex_start_color(),         do: exit(:nif_library_not_loaded)
  def ex_init_pair(_pair, _f, _b), do: exit(:nif_library_not_loaded)

  def ex_getch(),               do: exit(:nif_library_not_loaded)
  def cols(),                   do: exit(:nif_library_not_loaded)
  def lines(),                  do: exit(:nif_library_not_loaded)

  def getchar() do
    do_getchar(getch())
  end
  defp do_getchar(-1), do: getchar()
  defp do_getchar(c), do: c

  # not implemented in nif
  def getstr(),   do: do_getstr([], getx(), getchar())
  defp do_getstr(str, _x, 10) do
    String.reverse(List.to_string(['\n' | str]))
  end
  defp do_getstr(str, x, chr) do
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
    cx = getx()
    cy = gety()
    nx = max(x, cx-1)
    mvprintw(cy, nx, " ") # remove ?
    nx = max(x, nx-1)
    mvprintw(cy, nx, " ") # remove ^
    nx = max(x, nx-1)
    mvprintw(cy, nx, " ") # remove <char>
    mvprintw(cy, nx, "")  # move not implemented
  end

  # Common initialization
  def n_begin() do
    initscr()
    raw()
    cbreak()
  end

  def n_end() do
    nocbreak()
    endwin()
  end

end
