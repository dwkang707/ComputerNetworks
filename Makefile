CC=gcc
CFLAGS= -o
TARGETS=server
all: $(TARGETS)
.PHONY: all
%:
    $(CC) $(CFLAGS) $@ $@.c

clean:
    rm $(TARGETS)