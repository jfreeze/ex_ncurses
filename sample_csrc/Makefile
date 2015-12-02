SRCS := $(wildcard *.c)
PRGS := $(basename $(SRCS))

all : $(PRGS)

$(PRGS) : $(SRCS)
	$(CC) $@.c -lncurses -o $@

clean :
	$(RM) $(PRGS)

# run make print-VARNAME to inspect a var
print-%  : ; @echo $* = $($*)
