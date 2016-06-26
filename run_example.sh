#!/bin/sh

SAMPLE=$1

[ -n "$SAMPLE" ] || SAMPLE=demo
[ -n "$MIX_ENV" ] || MIX_ENV=dev

elixir -pa _build/$MIX_ENV/lib/ex_ncurses/ebin examples/$SAMPLE.exs

