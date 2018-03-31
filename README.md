# ex_ncurses
Ncurses Nif for Elixir (or Erlang)

## What It Does

ExNcurses is a Nif that you can add to your project and monitor
keyboard inputs one character at a time.

Testing ncurses inside an 'iex -S mix' console will fail to work after a few keystrokes.

## Usage

For usage, you can look at the code in the `examples` directory
or check out the QuickieSynth sample project (@elixirsips)
where we show a version using ExNcurses.

   [github.com/jfreeze/ex_ncurses](https://github.com/jfreeze/ex_ncurses)

## Installation

The package is available on [github](https://github.com/jfreeze/ex_ncurses) can be installed
by adding `ex_ncurses` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:ex_ncurses, git: "https://github.com/jfreeze/ex_ncurses.git"}
  ]
end
```

## Compiling

You will need to install the `ncurses` library and C header files first. Do this
however is appropriate for your system.

Then the project can be compiled with

    mix deps.get
    mix compile

### Run the examples

To run any of the examples, start them by invoking the `run_example.sh` script:

    ./run_example.sh demo

## FAQ

### Why isn't anything being drawn to the screen?

Try calling `ExNcurses.refresh/0` or `ExNcurses.wrefresh/1` if you're working in
a window.

### Hold up, x and y are swapped everywhere

That's the ncurses way. Everything is row and then column, so y comes first. The
upper left is (0, 0).

### I get a bad input fd in erts_poll error sometimes

When calling `initscr/0` from a `.exs` script, it's possible to get the
following error message:

```text
16:37:59.313 [error] Bad input fd in erts_poll()! fd=0, port=#Port<0.635>, driver=fd, name=0/1
```

This is due to race condition when `ex_ncurses` takes over `stdin` from the
Erlang console. There's some more info about this in `ex_ncurses.c`. If you're
reading this and know how to fix it, please share your knowledge and post an
issue. Otherwise, just run `ExNcurses.clear/0` in your script to obscure the
message.
