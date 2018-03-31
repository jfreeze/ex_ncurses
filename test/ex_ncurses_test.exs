defmodule ExNcursesTest do
  use ExUnit.Case
  doctest ExNcurses

  test "that initscr needs to be called first" do
    assert ExNcurses.refresh() == {:error, :uninitialized}
  end
end
