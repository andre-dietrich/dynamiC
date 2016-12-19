CC	   = gcc
CXX    = g++
RM	   = rm
ECHO	 = echo

CFLAGS = -Wall -g #-Os
OBJLIB = libdynC.so

SRC = $(wildcard dynamic*.c) ss_string.c
OBJ = $(patsubst %.c,%.o,$(SRC))


.PHONY: all test clean

all: lib #$(OBJLIB)

#$(OBJLIB): $(OBJ)
#		ar r $@ $^
#		ranlib $@

lib: $(OBJ)
		$(CC) $(CFLAGS) -shared $(OBJ) -o $(OBJLIB)

%.o: %.c
		$(CC) $(CFLAGS) -c -fpic -o $@ $<

clean:
		cd test; make clean
		$(RM) -f *.out *.o *.so *.a
		rm -rf docs

docs:
		doxygen

test: lib
		cd test; make
