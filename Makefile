CXX = g++
#CXXFLAGS = -Wall -Wextra -pedantic -O3 -std=gnu++0x
CXXFLAGS = -Wall -Wextra -pedantic -O3 -ggdb -std=gnu++0x
TARGET = genchess

VERSION = $(shell grep "define VERSION" main.cpp | cut -d'"' -f2)
DIR = $(shell pwd | sed -e 's/\//\n/g' | tail -1)

objs = main.o Terminal.o Game.o ComputerPlayer.o \
	TransTable.o Board.o Move.o MoveLookup.o

all : $(objs)
	$(CXX) $(objs) -o $(TARGET)
#	strip -s $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	rm -f *~ *.o $(TARGET)

archive : clean
	tar -caf ../genchess-$(VERSION).tar.xz ../$(DIR)
