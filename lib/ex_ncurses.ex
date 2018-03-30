defmodule ExNcurses do
  alias ExNcurses.{Getstr, Server}

  @moduledoc """
  ExNcurses lets Elixir programs create text-based user interfaces using ncurses.

  Aside from keyboard input, ExNcurses looks almost like a straight translation of the C-based
  ncurses API. ExNcurses sends key events via messages. See `listen/0` for this.

  See http://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html for ncurses documentation.

  NOTE: if you're using the IEx prompt, ExNcurses will quickly take it over. Calling `endwin/0`
  will let you return the the IEx prompt. You may see some errors momentarily though when the
  Erlang VM detects two uses of stdin.
  """

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

  @doc """
  Initialize ncurses
  """
  def initscr(), do: Server.invoke(:initscr)

  @doc """
  Stop using ncurses and clean the terminal back up.
  """
  def endwin(), do: Server.invoke(:endwin)

  @doc """
  """
  def printw(s), do: Server.invoke(:printw, {s})
  def addstr(s), do: Server.invoke(:addstr, {s})
  def mvprintw(y, x, s), do: Server.invoke(:mvprintw, {y, x, s})
  def mvaddstr(y, x, s), do: Server.invoke(:mvaddstr, {y, x, s})

  def mvcur(oldrow, oldcol, newrow, newcol),
    do: Server.invoke(:mvcur, {oldrow, oldcol, newrow, newcol})

  @doc """
  Refresh the display.
  """
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
  def scrollok(), do: Server.invoke(:scrollok)

  def start_color(), do: Server.invoke(:start_color)
  def has_colors(), do: Server.invoke(:has_colors)
  def init_pair(pair, f, b), do: Server.invoke(:init_pair, {pair, f, b})
  def attron(pair), do: Server.invoke(:attron, {pair})
  def attroff(pair), do: Server.invoke(:attroff, {pair})

  def cols(), do: Server.invoke(:cols)
  def lines(), do: Server.invoke(:lines)

  @doc """
  Poll for a key press.

  See `listen/0` for a better way of getting keyboard input.
  """
  def getch() do
    listen()

    c =
      receive do
        {:ex_ncurses, :key, key} -> key
      end

    stop_listening()
    c
  end

  @doc """
  Poll for a string.
  """
  def getstr() do
    listen()

    noecho()

    str = getstr_loop(Getstr.init(gety(), getx(), 60))

    stop_listening()
    str
  end

  defp getstr_loop(state) do
    receive do
      {:ex_ncurses, :key, key} ->
        case Getstr.process(state, key) do
          {:done, str} ->
            str

          {:not_done, new_state} ->
            getstr_loop(new_state)
        end
    end
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

  @doc """
  Listen for events.

  Events will be sent as messages of the form:

  `{ex_ncurses, :key, key}`
  """
  defdelegate listen(), to: Server

  @doc """
  Stop listening for events
  """
  defdelegate stop_listening(), to: Server
end
