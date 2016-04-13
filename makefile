CXX = g++
CC = g++
FLAGS = -std=c++11 \
	-Wall
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

scan_all: mscan.cpp testall.cpp
	$(CXX) $(FLAGS) mscan.cpp testall.cpp -o scan_all

scan_all_file: mscan.cpp testtofile.cpp
	$(CXX) $(FLAGS) mscan.cpp testtofile.cpp -o scan_all_file

clobber:
	rm -f *.o *.exe *.lis *.lst *.obj core micro scan_test test_all

clean: clobber
	rm -f *.asm *.lst scan_test
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean); done
