defmodule ExNcursesNifNotLoaded do
  defexception [:message]

  def message(e), do: e.message || "ncurses nif not loaded"
end
