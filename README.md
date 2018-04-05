# ex_ncurses

[![Hex version](https://img.shields.io/hexpm/v/ex_ncurses.svg "Hex version")](https://hex.pm/packages/ex_ncurses)

Ncurses NIF for Elixir

## What It Does

ExNcurses is a NIF that let's you create text-based user interfaces and games.
It connects Elixir to the [ncurses
library](https://www.gnu.org/software/ncurses/ncurses.html). Not all of ncurses
is available, but quite a bit is including the ability to receive key presses as
they happen.

## Usage

For usage, you can look at the code in the `examples` directory or check out the
QuickieSynth sample project (@elixirsips) where we show a version using
ExNcurses.

   [github.com/jfreeze/ex_ncurses](https://github.com/jfreeze/ex_ncurses)

## Installation

The package is available on [hex.pm](https://hex.pm/packages/ex_ncurses) and can
be installed by adding `ex_ncurses` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:ex_ncurses, "~> 0.1"}
  ]
end
```

## Compiling

You will need to install the `ncurses` library and C header files first. Do this
however is appropriate for your system.

Then the project can be compiled with

```sh
mix deps.get
mix compile
```

### Run the examples

To run any of the examples, start them by invoking the `run_example.sh` script:

    ./run_example.sh demo

## FAQ

### Why isn't anything being drawn to the screen

Try calling `ExNcurses.refresh/0` or `ExNcurses.wrefresh/1` if you're working in
a window.

### Hold up, x and y are swapped everywhere

That's the ncurses way. Everything is row and then column, so y comes first. The
upper left is (0, 0).

### What happened to the Elixir console

It's still there. It won't receive any input between calls to
`ExNcurses.initscr/0` and `ExNcurses.endwin/0`. I'm still on the fence with what
to do with the console. It could be redirected, output could be thrown away,
etc. At the moment, you'll likely want to turn on the Elixir console logger so
that it doesn't interfere with the display.
