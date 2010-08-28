CXX = g++

#CXXFLAGS = -Wall -Wextra -pedantic -O0 -std=gnu++0x -g3
CXXFLAGS = -Wall -Wextra -pedantic -O3 -std=gnu++0x

VERSION = $(shell grep "define VERSION" main.cpp | cut -d'"' -f2)
BIN = genchess
DIR = $(shell pwd | sed -e 's/\//\n/g' | tail -1)

objs = main.o Terminal.o Game.o ComputerPlayer.o Board.o Move.o MoveLookup.o

all : $(objs)
	$(CXX) $(objs) -o $(BIN) $(INCLUDES)
#	strip -s $(BIN)

clean :
	rm -f *~ *.o $(BIN)

archive : clean
	tar -caf ../genchess-$(VERSION).tar.xz ../$(DIR)
