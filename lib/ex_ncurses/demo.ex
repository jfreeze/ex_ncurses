defmodule ExNcurses.Demo do

  def main(_args) do
    Code.ensure_loaded(:ex_ncurses)
    |> IO.inspect
    run_samples
    run_c_samples
  end

  def run_c_samples do
    #Border.run()
  end

  def run_samples do
    ExNcurses.n_begin()
    sample_1()
    ExNcurses.clear()
    sample_2()
    ExNcurses.clear()
    sample_3()
    ExNcurses.n_end()
  end

  def print_fox do
    ExNcurses.printw("The quick brown fox jumped over the lazy dog! ")
  end

  def sample_1 do
    IO.puts "Running ExNcurses Sample 1"
    cols = ExNcurses.cols()
    lines = ExNcurses.lines()
    ExNcurses.cbreak()
    for _x <- (1..4), do: print_fox()
    ExNcurses.printw("\n")
    ExNcurses.printw("AA\n")
    ExNcurses.printw("BB\n")
    ExNcurses.printw("CC\n")
    ExNcurses.printw("lines: #{lines}")
    ExNcurses.printw("\n")
    ExNcurses.printw("cols: #{cols}\n")
    ExNcurses.refresh()
    :timer.sleep 2000
  end

  def sample_2 do
    welcome = "Running ExExNcurses Sample 2"
    ExNcurses.mvprintw(div(ExNcurses.lines(),2),
                div(ExNcurses.cols() - String.length(welcome),2), welcome)

    msg = "F1 to exit"
    ExNcurses.mvprintw(div(ExNcurses.lines(),2)+1,
                div(ExNcurses.cols() - String.length(msg),2),
                msg)
    sample_2a()
  end

  def sample_2a do
    msg = "Enter a character please: "
    y = div(ExNcurses.lines, 4)
    x = div(ExNcurses.cols - String.length(msg), 2)
    ExNcurses.mvprintw(y, x, msg)
    ExNcurses.refresh()

    ExNcurses.keypad()   # accept Function keys
    ExNcurses.flushinp() # clear input
    char = ExNcurses.getchar()
    ExNcurses.mvprintw(y+1, x, "You entered '#{char}'\n")
    # F1 exits
    if char != ExNcurses.fun(:F1), do: sample_2a()
  end

  def sample_3 do
    mesg = "Enter a string: "
    row = ExNcurses.lines()
    col = ExNcurses.cols()
    ExNcurses.mvprintw( div(row,2), div(col-String.length(mesg),2), mesg);
    #print the message at the center of the screen
    str = ExNcurses.getstr()
    ExNcurses.mvprintw(row - 2, 0, "You Entered: #{inspect str}")
    ExNcurses.getchar()
  end
end
