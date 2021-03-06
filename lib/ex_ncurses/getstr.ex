defmodule ExNcurses.Getstr do
  defstruct y: 0, x: 0, field_length: 0, cursor: 0, chars: []

  @doc """
  Initialize state for string entry
  """
  @spec init(non_neg_integer(), non_neg_integer(), pos_integer()) :: %ExNcurses.Getstr{}
  def init(y, x, length) do
    %ExNcurses.Getstr{y: y, x: x, field_length: length}
  end

  @doc """
  Process a key press

  Returns:

  * `{:done, string}` when the user presses enter
  * `:cancelled` if the user presses ESCape
  * `{:not_done, state}` if more is coming
  """
  @spec process(%ExNcurses.Getstr{}, non_neg_integer()) ::
          {:done, String.t()} | :cancelled | {:not_done, %ExNcurses.Getstr{}}
  def process(state, ?\n) do
    str =
      state.chars
      |> Enum.reverse()
      |> to_string

    {:done, str}
  end

  def process(_state, 27), do: :cancelled

  def process(%{chars: []} = state, key) when key == 127 or key == :backspace do
    {:not_done, state}
  end

  def process(%{chars: [_c | rest]} = state, key) when key == 127 or key == :backspace do
    new_cursor = state.cursor - 1
    ExNcurses.mvprintw(state.y, state.x + new_cursor, " \b")
    ExNcurses.refresh()
    {:not_done, %{state | chars: rest, cursor: new_cursor}}
  end

  def process(%{chars: chars, field_length: field_length} = state, key)
      when length(chars) < field_length and key < 127 do
    ExNcurses.mvprintw(state.y, state.x + state.cursor, <<key>>)
    ExNcurses.refresh()
    {:not_done, %{state | chars: [key | chars], cursor: state.cursor + 1}}
  end

  def process(state, _key) do
    {:not_done, state}
  end
end
