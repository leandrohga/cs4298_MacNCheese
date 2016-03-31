CXX = g++
CC = g++
FLAGS = -std=c++11
DIRS = sam tests

all: micro

build: micro sam
	-for d in $(DIRS); do (cd $$d; $(MAKE)); done
	cp sam/sam tests/
	cp sam/macc tests/
	cp micro tests/

micro: micro.cpp mparse.o mscan.o mcode.o
	$(CXX) $(FLAGS) mparse.o mscan.o mcode.o micro.cpp -o micro

mparse.o: mparse.cpp
	$(CXX) $(FLAGS) -c mparse.cpp

mscan.o: mscan.cpp
	$(CXX) $(FLAGS) -c mscan.cpp

mcode.o: mcode.cpp
	$(CXX) $(FLAGS) -c mcode.cpp

scan_test: mscan.cpp scantester.cpp
	$(CXX) $(FLAGS) mscan.cpp scantester.cpp -o scan_test

test_all: mscan.cpp testall.cpp
	$(CXX) $(FLAGS) mscan.cpp testall.cpp -o test_all

clobber:
	rm -f *.o *.exe *.lis *.obj core micro

clean: clobber
	rm -f *.asm *.lst scan_test
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean); done
