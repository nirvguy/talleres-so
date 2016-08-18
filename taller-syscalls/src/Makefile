CFLAGS=-Wall -O0 -g -ggdb
LDFLAGS=
CC=gcc

BINS=launch justiciero

OBJS=$(BINS:=.o)

all: $(BINS) $(OBJS)

clean:
	rm -rf $(OBJS) $(BINS)

.PHONY: all clean
