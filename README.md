# ex_ncurses
Ncurses Nif for Elixir (or Erlang)

## What It Does

ExNcurses is a Nif that you can add to your project and monitor
keyboard inputs one character at a time.

Note, however, in order for this to work, programs must be compiled
with <code>'mix escript.build'</code>.

Testing ncurses inside an 'iex -S mix' console will fail to work after a few keystrokes.

## Usage

For usage, you can look at the example code in demo.ex,
or check out the QuickieSynth sample project (@elixirsips)
where we show a version using ExNcurses.

   [github.com/jfreeze/ex_ncurses](https://github.com/jfreeze/ex_ncurses)


## Installation

The package is available on [github](https://github.com/jfreeze/ex_ncurses) can be installed as:

  1. Add ex_ncurses to your list of dependencies in your `mix.exs`:

        def deps do
        [
          {:ex_ncurses, git: "https://github.com/jfreeze/ex_ncurses.git"}
        ]
        end

## Sample Project

You can see a version of @ElixirSips QuickieSynth project using ExNcurses at [https://github.com/jfreeze/quickie_synth](https://github.com/jfreeze/quickie_synth)

## Compiling

The project can be compiled from your parent project with

    mix deps.compile

## Working with ExNcurses

This project compiles a Nif to

    mix compile.ex_ncurses

or

    mix demo

### Run the Demo

    ./priv/demo

The Makefile assumes ncurses is already installed, that Erlang is installed with brew, and is only tested for OS X.  You may need to edit the Makefile for other systems.

Compile the Nif with either <code>make</code> or

    mix compile.ex_ncurses
