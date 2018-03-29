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

