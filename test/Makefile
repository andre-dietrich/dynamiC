CC	   = gcc
CXX    = g++
RM	   = rm

CFLAGS = -Wall -g
OBJLIB = ../libdynC.so

SRC  = $(wildcard *.cpp)
OBJ  = $(patsubst %.cpp,%.o,$(SRC))
OUT  = $(patsubst %.o,%.out,$(OBJ))

all: $(OUT)
		@echo "          ██████╗  █████╗ ████████╗ █████╗ ████████╗██╗   ██╗██████╗ ███████╗███████╗"
		@echo "          ██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗╚══██╔══╝╚██╗ ██╔╝██╔══██╗██╔════╝██╔════╝"
		@echo "          ██║  ██║███████║   ██║   ███████║   ██║    ╚████╔╝ ██████╔╝█████╗  ███████╗"
		@echo "          ██║  ██║██╔══██║   ██║   ██╔══██║   ██║     ╚██╔╝  ██╔═══╝ ██╔══╝  ╚════██║"
		@echo "          ██████╔╝██║  ██║   ██║   ██║  ██║   ██║      ██║   ██║     ███████╗███████║"
		@echo "          ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝      ╚═╝   ╚═╝     ╚══════╝╚══════╝"
		LD_LIBRARY_PATH=.. ./$<

%.out: %.o
		$(CXX) $< -o $@ -L. $(OBJLIB) -L/usr/lib -lgtest -lgtest_main -lpthread

%.o: %.cpp
		$(CXX) $(CFLAGS) -I./../src -c $<

clean:
		$(RM) -f *.out *.o
