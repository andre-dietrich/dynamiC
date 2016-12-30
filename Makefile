CC	   = gcc
CXX    = g++
RM	   = rm
ECHO	 = echo

CFLAGS = -Wall -g #-Os
OBJLIB = libdynC.so

SRC = $(wildcard src/dynamic*.c)
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
		cd src; $(RM) -f *.out *.o *.so *.a

clean-docs:
		rm -rf docs

docs: clean-docs
		doxygen

test: lib
		cd test; make
