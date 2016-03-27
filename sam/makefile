CXX = g++
CC = g++
FLAGS = 

all:    macc sam

sam: sam2016.cpp
	$(CXX) $(FLAGS) sam2016.cpp -o sam

macc: macc2016.cpp
	$(CXX) $(FLAGS) macc2016.cpp -o macc

clobber:
	rm -f *.o *.exe *.lis *.obj core sam macc trace.txt

