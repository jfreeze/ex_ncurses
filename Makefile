# Assumes erlang is installed with brew
# Change erl to your erlang install path if needed
erl=$(wildcard /usr/local/Cellar/erlang/**/lib/erlang/usr/include)
INC=$(addprefix "-I", $(erl))

priv/ex_ncurses.so : src/ex_ncurses.c
	$(CC) $(INC) -fPIC -shared -undefined dynamic_lookup -dynamiclib src/ex_ncurses.c -o priv/ex_ncurses.so -lncurses


clean:
	$(RM) priv/ex_ncurses.so

all: priv/ex_ncurses.so

# run make print-VARNAME to inspect a var
print-%  : ; @echo $* = $($*)
