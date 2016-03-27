CXX = g++
CC = g++
FLAGS = -std=c++11

all:   micro

micro: micro.cpp mparse.o mscan.o mcode.o
	$(CXX) $(FLAGS) mparse.o mscan.o mcode.o micro.cpp -o micro

mparse.o: mparse.cpp
	$(CXX) $(FLAGS) -c mparse.cpp

mscan.o: mscan.cpp
	$(CXX) $(FLAGS) -c mscan.cpp

mcode.o: mcode.cpp
	$(CXX) $(FLAGS) -c mcode.cpp

clobber:
	rm -f *.o *.exe *.lis *.obj core micro

clean: clobber
	rm -f *.asm *.lst

scan_test: mscan.cpp scantester.cpp
	$(CXX) $(FLAGS) mscan.cpp scantester.cpp -o scan_test
