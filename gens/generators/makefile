CXX = g++
CC = g++
FLAGS = 

all:    llgen parsergen

llgen: llgen2016.cpp Intset.h
	$(CXX) $(FLAGS) llgen2016.cpp -o llgen

parsergen: parserGen2016.cpp
	$(CXX) $(FLAGS) parserGen2016.cpp -o parsergen

clobber:
	rm -f *.o *.exe *.lis *.obj core llgen parsergen trace.txt

