defmodule ExNcurses.Nif do
  @compile {:autoload, false}
  @on_load {:load_nif, 0}

  @moduledoc false

  def load_nif() do
    Application.app_dir(:ex_ncurses, "priv/ex_ncurses")
    |> to_charlist
    |> :erlang.load_nif(0)
  end

  def poll(), do: :erlang.nif_error(:nif_not_loaded)
  def read(), do: :erlang.nif_error(:nif_not_loaded)
  def stop(), do: :erlang.nif_error(:nif_not_loaded)

  def invoke(_function, _args), do: :erlang.nif_error(:nif_not_loaded)
end
