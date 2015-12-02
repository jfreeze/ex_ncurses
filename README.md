# ex_ncurses
Ncurses Nif for Elixir (or Erlang)

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed as:

  1. Add ex_ncurses to your list of dependencies in `mix.exs`:

        def deps do
          [{:ex_ncurses, "~> 0.0.1"}]
        end

## Compiling

This project compiles a Nif to

    priv/ex_ncurses.so

The Makefile assumes ncurses is already installed, that Erlang is installed with brew, and is only tested for OS X.  You may need to edit the Makefile for other systems.

Compile the Nif with either <code>make</code> or

    mix compile.ex_ncurses

## Usage
See demo.ex for example usage.
