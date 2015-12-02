defmodule Mix.Tasks.Tester do
  use Mix.Task

  @moduledoc "a tester module"
  @shortdoc "a test app"
  @doc "tester"
  def run(_x) do
    IO.puts "tester just ran"
  end
end
