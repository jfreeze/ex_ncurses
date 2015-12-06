defmodule Mix.Tasks.Demo.Build do
  use Mix.Task

  @shortdoc "Compile and Build Demo"
  def run(_) do
    Mix.Tasks.Compile.ExNcurses.run :true
    {result, _error_code} = System.cmd("mix", ["escript.build"], stderr_to_stdout: true)
    Mix.shell.info result
  end
end
