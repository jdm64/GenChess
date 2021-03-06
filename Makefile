CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -Wno-unused-parameter -O3 -std=c++11
TARGET = genchess
ENG_TESTER = enginetester
LIB_GENCHESS = libgenchess.a

VERSION = $(shell git describe --tags --dirty)
DIR = $(shell pwd | sed -e 's/\//\n/g' | tail -1)

base_objs = Move.o BaseBoard.o MoveLookup.o GenMoveLookup.o RegMoveLookup.o \
	GenPosition.o RegPosition.o GenBoard.o RegBoard.o TransTable.o

eng_objs = GenEngine.o RegEngine.o main.o

all : genchess enginetester

genchess : $(base_objs) $(eng_objs)
	$(CXX) $(base_objs) $(eng_objs) -o $(TARGET)
#	strip -s $(TARGET)

enginetester : EngineTester.o
	$(CXX) EngineTester.o -o $(ENG_TESTER)

libgenchess : $(base_objs)
	ar rs $(LIB_GENCHESS) $(base_objs)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o : main.cpp
	$(CXX) -DVERSION=\"$(VERSION)\" $(CXXFLAGS) -c $< -o $@

clean :
	rm -f *~ *.o $(TARGET) $(ENG_TESTER) $(LIB_GENCHESS)

archive : clean
	tar -caf ../genchess-$(VERSION).tar.xz ../$(DIR)

cppcheck :
	cppcheck --enable=all --inconclusive ./ 1> /dev/null

analyze : clean
	scan-build make all
