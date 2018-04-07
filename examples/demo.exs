Application.start(:ex_ncurses)

defmodule Demo do
  def run do
    ExNcurses.n_begin()
    sample_1()
    ExNcurses.clear()
    sample_2()
    ExNcurses.clear()
    sample_3()
    ExNcurses.n_end()
    IO.puts("Done!!")
  end

  def print_fox do
    ExNcurses.printw("The quick brown fox jumped over the lazy dog! ")
  end

  def sample_1 do
    cols = ExNcurses.cols()
    lines = ExNcurses.lines()
    ExNcurses.cbreak()
    for _x <- 1..4, do: print_fox()
    ExNcurses.printw("\n")
    ExNcurses.printw("AA\n")
    ExNcurses.printw("BB\n")
    ExNcurses.printw("CC\n")
    ExNcurses.printw("lines: #{lines}")
    ExNcurses.printw("\n")
    ExNcurses.printw("cols: #{cols}\n")
    ExNcurses.refresh()
    ExNcurses.getch()
  end

  def sample_2 do
    center_text(div(ExNcurses.lines(), 2), "Running ExNcurses Sample 2")
    center_text(div(ExNcurses.lines(), 2) + 1, "F1 to exit")

    sample_2a()
  end

  def sample_2a do
    y = div(ExNcurses.lines(), 4)
    center_text(y, "Enter a character please: ")
    ExNcurses.refresh()

    # accept Function keys
    ExNcurses.keypad()
    # clear input
    ExNcurses.flushinp()
    char = ExNcurses.getch()
    center_text(y + 1, "You entered '#{char}'  ")
    ExNcurses.refresh()
    # F1 exits
    if char != ExNcurses.fun(:F1), do: sample_2a()
  end

  def sample_3 do
    row = ExNcurses.lines()
    center_text(div(row, 2), "Enter a string: ")
    ExNcurses.refresh()
    # print the message at the center of the screen

    str = ExNcurses.getstr()
    ExNcurses.mvprintw(row - 2, 0, "You Entered: #{inspect(str)}")
    ExNcurses.refresh()
    ExNcurses.getch()
  end

  defp center_text(row, msg) do
    ExNcurses.mvprintw(
      row,
      div(ExNcurses.cols() - String.length(msg), 2),
      msg
    )
  end
end

Demo.run()
