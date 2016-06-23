defmodule ExNcursesNifNotLoaded do
  defexception [:message]

  def exception(thing) do
    IO.inspect thing
  end
end
