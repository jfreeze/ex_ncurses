defmodule ExNcurses do
  alias ExNcurses.Server

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

  def initscr(), do: Server.invoke(:initscr)
  def endwin(), do: Server.invoke(:endwin)

  def printw(s), do: Server.invoke(:printw, {s})
  def mvprintw(y, x, s), do: Server.invoke(:mvprintw, {y, x, s})

  def refresh(), do: Server.invoke(:refresh)
  def clear(), do: Server.invoke(:clear)

  def raw(), do: Server.invoke(:raw)
  def cbreak(), do: Server.invoke(:cbreak)
  def nocbreak(), do: Server.invoke(:nocbreak)

  def noecho(), do: Server.invoke(:noecho)

  def getx(), do: Server.invoke(:getx)
  def gety(), do: Server.invoke(:gety)

  def flushinp(), do: Server.invoke(:flushinp)
  def keypad(), do: Server.invoke(:keypad)

  def start_color(), do: Server.invoke(:start_color)
  def has_colors(), do: Server.invoke(:has_colors)
  def init_pair(pair, f, b), do: Server.invoke(:init_pair, {pair, f, b})
  def attron(pair), do: Server.invoke(:attron, {pair})
  def attroff(pair), do: Server.invoke(:attroff, {pair})

  def cols(), do: Server.invoke(:cols)
  def lines(), do: Server.invoke(:lines)

  def getch() do
    listen()
    c =
      receive do
        {:ex_ncurses, :key, key} -> key
      end
    stop_listening()
    c
  end

  # not implemented in nif
  #def getstr(), do: do_getstr([], getx(), getchar())

  #defp do_getstr(str, _x, 10) do
  #  String.reverse(List.to_string(['\n' | str]))
  #end

  #defp do_getstr(str, x, chr) do
  #  case chr do
  #    127 ->
  #      handle_delete(x)
  #      [_h | t] = str
  #      do_getstr(t, x, getchar())
  #
  #    -1 ->
  #      do_getstr(str, x, getchar())
  #
  #    _ ->
  #      do_getstr([chr | str], x, getchar())
  #  end
  #end

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

  defdelegate listen(), to: Server
  defdelegate stop_listening(), to: Server
end
