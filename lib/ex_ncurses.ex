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
  def clr(:BLACK), do: 0
  def clr(:COLOR_RED), do: 1
  def clr(:RED), do: 1
  def clr(:COLOR_GREEN), do: 2
  def clr(:GREEN), do: 2
  def clr(:COLOR_YELLOW), do: 3
  def clr(:YELLOW), do: 3
  def clr(:COLOR_BLUE), do: 4
  def clr(:BLUE), do: 4
  def clr(:COLOR_MAGENTA), do: 5
  def clr(:MAGENTA), do: 5
  def clr(:COLOR_CYAN), do: 6
  def clr(:CYAN), do: 6
  def clr(:COLOR_WHITE), do: 7
  def clr(:WHITE), do: 7

  def initscr(), do: exit(:nif_library_not_loaded)
  def endwin(), do: exit(:nif_library_not_loaded)

  def printw(_s), do: exit(:nif_library_not_loaded)
  def mvprintw(_y, _x, _s), do: exit(:nif_library_not_loaded)

  def refresh(), do: exit(:nif_library_not_loaded)
  def clear(), do: exit(:nif_library_not_loaded)

  def raw(), do: exit(:nif_library_not_loaded)
  def cbreak(), do: exit(:nif_library_not_loaded)
  def nocbreak(), do: exit(:nif_library_not_loaded)

  def noecho(), do: exit(:nif_library_not_loaded)

  def getx(), do: exit(:nif_library_not_loaded)
  def gety(), do: exit(:nif_library_not_loaded)

  def flushinp(), do: exit(:nif_library_not_loaded)
  def keypad(), do: exit(:nif_library_not_loaded)

  def start_color(), do: exit(:nif_library_not_loaded)
  def has_colors(), do: exit(:nif_library_not_loaded)
  def init_pair(_pair, _f, _b), do: exit(:nif_library_not_loaded)
  def attron(_pair), do: exit(:nif_library_not_loaded)
  def attroff(_pair), do: exit(:nif_library_not_loaded)

  def getch(), do: exit(:nif_library_not_loaded)
  def cols(), do: exit(:nif_library_not_loaded)
  def lines(), do: exit(:nif_library_not_loaded)

  def getchar() do
    do_getchar(getch())
  end

  defp do_getchar(-1), do: getchar()
  defp do_getchar(c), do: c

  # not implemented in nif
  def getstr(), do: do_getstr([], getx(), getchar())

  defp do_getstr(str, _x, 10) do
    String.reverse(List.to_string(['\n' | str]))
  end

  defp do_getstr(str, x, chr) do
    case chr do
      127 ->
        handle_delete(x)
        [_h | t] = str
        do_getstr(t, x, getchar())

      -1 ->
        do_getstr(str, x, getchar())

      _ ->
        do_getstr([chr | str], x, getchar())
    end
  end

  # Simple minded positioning. Does not account for line wrap.
  defp handle_delete(x) do
    cx = getx()
    cy = gety()
    nx = max(x, cx - 1)
    # remove ?
    mvprintw(cy, nx, " ")
    nx = max(x, nx - 1)
    # remove ^
    mvprintw(cy, nx, " ")
    nx = max(x, nx - 1)
    # remove <char>
    mvprintw(cy, nx, " ")
    # move not implemented
    mvprintw(cy, nx, "")
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
