defmodule Mix.Tasks.Compile.ExNcurses do
  use Mix.Task

  @shortdoc "Compiles ExNcurses"

  def run(_) do
    {result, _error_code} = System.cmd("make", ["priv/ex_ncurses.so"], stderr_to_stdout: true)
    Mix.shell.info result
  end
end
