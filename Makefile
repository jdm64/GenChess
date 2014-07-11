CXX = g++
#CXXFLAGS = -Wall -Wextra -pedantic -O3 -std=gnu++0x
CXXFLAGS = -Wall -Wextra -pedantic -O3 -std=gnu++0x
TARGET = genchess

VERSION = $(shell git describe --tags --dirty)
DIR = $(shell pwd | sed -e 's/\//\n/g' | tail -1)

objs = Move.o BaseBoard.o MoveLookup.o GenMoveLookup.o RegMoveLookup.o \
	GenPosition.o RegPosition.o GenBoard.o RegBoard.o TransTable.o GenEngine.o \
	RegEngine.o main.o

all : genchess enginetester

genchess : $(objs)
	$(CXX) $(objs) -o $(TARGET)
#	strip -s $(TARGET)

enginetester : EngineTester.o
	$(CXX) EngineTester.o -o enginetester

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o : main.cpp
	$(CXX) -DVERSION=\"$(VERSION)\" $(CXXFLAGS) -c $< -o $@

clean :
	rm -f *~ *.o $(TARGET) enginetester

archive : clean
	tar -caf ../genchess-$(VERSION).tar.xz ../$(DIR)

cppcheck :
	cppcheck --enable=all --inconclusive ./ 1> /dev/null

analyze : clean
	scan-build make all
