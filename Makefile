CXX = g++
#CXXFLAGS = -Wall -Wextra -pedantic -O3 -std=gnu++0x

# use this for debuging
CXXFLAGS = -Wall -Wextra -pedantic -O0 -std=gnu++0x -g3

VERSION = 1.0.0
BIN = genchess
DIR = $(shell pwd | sed -e 's/\//\n/g' | tail -1)

objs = main.o Game.o Board.o MoveLookup.o ComputerPlayer.o HumanPlayer.o

all : $(objs)
	$(CXX) $(objs) -o $(BIN) $(INCLUDES)
#	strip -s $(BIN)

clean :
	rm -f *~ *.o $(BIN)

archive : clean
	tar -caf ../genchess-$(VERSION).tar.xz ../$(DIR)
