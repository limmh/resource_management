CC=g++
CFLAGS=-Wall -I../include/ -std=c++11
LFLAGS=-Wall -lstdc++

all: open_file

open_file: open_file.o
	$(CC) $(LFLAGS) -o open_file open_file.o

open_file.o: open_file.cpp ../include/res_mgr_resource.hpp
	$(CC) $(CFLAGS) -c open_file.cpp

clean:
	rm -f open_file.o
	rm -f open_file
