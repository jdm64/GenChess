CXX = g++
#CXXFLAGS = -Wall -Wextra -pedantic -O3 -std=gnu++0x
CXXFLAGS = -Wall -Wextra -Weffc++ -pedantic -O3 -ggdb -std=gnu++0x -flto -fwhole-program
TARGET = genchess

VERSION = $(shell grep "define VERSION" Defines.h | cut -d'"' -f2)
DIR = $(shell pwd | sed -e 's/\//\n/g' | tail -1)

objs = Util.o Move.o MoveLookup.o Position.o Board.o TransTable.o \
	ComputerPlayer.o Game.o UI.o CVEP.o Terminal.o main.o

all : genchess enginetester

genchess : $(objs)
	$(CXX) $(objs) -flto -o $(TARGET)
#	strip -s $(TARGET)

enginetester : EngineTester.o
	$(CXX) EngineTester.o Util.o -o enginetester

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	rm -f *~ *.o $(TARGET) enginetester

archive : clean
	tar -caf ../genchess-$(VERSION).tar.xz ../$(DIR)
