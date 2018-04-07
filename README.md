# ex_ncurses

[![Build Status](https://travis-ci.org/jfreeze/ex_ncurses.svg?branch=master)](https://travis-ci.org/jfreeze/ex_ncurses)
[![Hex version](https://img.shields.io/hexpm/v/ex_ncurses.svg "Hex version")](https://hex.pm/packages/ex_ncurses)

Ncurses NIF for Elixir

## What it does

ExNcurses is a NIF that let's you create text-based user interfaces and games.
It connects Elixir to the [ncurses
library](https://www.gnu.org/software/ncurses/ncurses.html).

## Differences from C ncurses

In general, this library provides a thin wrapper on most ncurses functions.
That means that the documentation and examples for the C library should apply
fairly directly. There are some differences:

* #defines are now atoms. For exaample, instead of `KEY_UP` for up arrow, you'll
  receive the atom `:up`.
* Keyboard input comes in as `{ex_ncurses, :key, code}` messages if you call
  `listen/0`. You can still call `getch()` like in C, but it doesn't have the
  semantics.
* Functions that take `printf` style arguments in C are available, but don't
  support arguments. It's expected that programs construct strings in Elixir
  like normal..

## Installation

The package is available on [hex.pm](https://hex.pm/packages/ex_ncurses) and can
be installed by adding `ex_ncurses` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:ex_ncurses, "~> 0.3"}
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

## Examples

To run any of the example scripts, start them by invoking the `run_example.sh`:

```sh
./run_example.sh demo
```

More examples can be found at:

* [Snake](https://github.com/fhunleth/snake)

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
