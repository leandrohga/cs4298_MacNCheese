CXX = g++
CC = g++
FLAGS = -std=c++11
DIRS = sam

all: micro

build: micro sam
	-for d in $(DIRS); do (cd $$d; $(MAKE)); done

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
	rm -f *.asm *.lst scan_test

scan_test: mscan.cpp scantester.cpp
	$(CXX) $(FLAGS) mscan.cpp scantester.cpp -o scan_test
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean); done
