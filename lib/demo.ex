defmodule ExNcurses.Demo do
  import ExNcurses

  def main(_args) do
    ncurses_begin()
    sample_1()
    ex_clear()
    sample_2()
    ex_clear()
    sample_3()
    ncurses_end()
  end

  def print_fox do
    ex_printw("The quick brown fox jumped over the lazy dog! ")
  end

  def sample_1 do
    IO.puts "Running ExNcurses Sample 1"
    cols = cols()
    lines = lines()
    ex_cbreak()
    for x <- (1..4), do: print_fox()
    ex_printw("\n")
    ex_printw("AA\n")
    ex_printw("BB\n")
    ex_printw("CC\n")
    ex_printw("lines: #{lines}")
    ex_printw("\n")
    ex_printw("cols: #{cols}\n")
    ex_refresh()
    :timer.sleep 2000
  end

  def sample_2 do
    welcome = "Running ExNcurses Sample 2"
    ex_mvprintw(div(lines(),2),
                div(cols() - String.length(welcome),2), welcome)

    msg = "F1 to exit"
    ex_mvprintw(div(lines(),2)+1,
                div(cols() - String.length(msg),2),
                msg)
    sample_2a()
  end

  def sample_2a do
    msg = "Enter a character please: "
    y = div(lines, 4)
    x = div(cols - String.length(msg), 2)
    ex_mvprintw(y, x, msg)
    ex_refresh()

    ex_keypad()   # accept Function keys
    ex_flushinp() # clear input
    char = getchar()
    ex_mvprintw(y+1, x, "You entered '#{char}'\n")

    # F1 exits
    if char != fun(:F1), do: sample_2a()
  end

  def sample_3 do
    mesg = "Enter a string: "
    row = lines()
    col = cols()
    ex_mvprintw( div(row,2), div(col-String.length(mesg),2), mesg);
    #print the message at the center of the screen
    str = ex_getstr()
    ex_mvprintw(row - 2, 0, "You Entered: #{inspect str}")
    getchar()
  end
end
