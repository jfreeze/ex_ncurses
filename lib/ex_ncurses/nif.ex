defmodule ExNcurses.Nif do
  @moduledoc false

  @compile {:autoload, false}
  @on_load {:load_nif, 0}
  def load_nif() do
    Application.app_dir(:ex_ncurses, "priv/ex_ncurses")
    |> to_charlist
    |> :erlang.load_nif(0)
  end

  @doc """
  Initialize ncurses on the specified terminal.
  """
  def newterm(_term, _ttyname), do: :erlang.nif_error(:nif_not_loaded)

  @doc """
  Poll for events from ncurses. When an event is ready,
  {:select, _res, _ref, :ready_input} will be sent back and
  then `read/0` should be called to get the event.
  """
  def poll(), do: :erlang.nif_error(:nif_not_loaded)

  @doc """
  Read an event.
  """
  def read(), do: :erlang.nif_error(:nif_not_loaded)

  @doc """
  Stop using ncurses.
  """
  def endwin(), do: :erlang.nif_error(:nif_not_loaded)

  @doc """
  Invoke an ncurses function
  """
  def invoke(_function, _args), do: :erlang.nif_error(:nif_not_loaded)
end
