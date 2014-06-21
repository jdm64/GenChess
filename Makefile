CXX = g++
#CXXFLAGS = -Wall -Wextra -pedantic -O3 -std=gnu++0x
CXXFLAGS = -Wall -Wextra -pedantic -O3 -std=gnu++0x
TARGET = genchess

VERSION = $(shell grep "define VERSION" Defines.h | cut -d'"' -f2)
DIR = $(shell pwd | sed -e 's/\//\n/g' | tail -1)

objs = Util.o Move.o BaseBoard.o MoveLookup.o GenMoveLookup.o RegMoveLookup.o Position.o \
	GenPosition.o RegPosition.o GenBoard.o RegBoard.o TransTable.o GenEngine.o \
	RegEngine.o GenGame.o RegGame.o main.o

all : genchess enginetester

genchess : $(objs)
	$(CXX) $(objs) -o $(TARGET)
#	strip -s $(TARGET)

enginetester : EngineTester.o Util.o
	$(CXX) EngineTester.o Util.o -o enginetester

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	rm -f *~ *.o $(TARGET) enginetester

archive : clean
	tar -caf ../genchess-$(VERSION).tar.xz ../$(DIR)

cppcheck :
	cppcheck --enable=all --inconclusive ./ 1> /dev/null

analyze : clean
	scan-build make all
