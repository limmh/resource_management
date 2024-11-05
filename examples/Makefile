CC=g++
CFLAGS=-Wall -I../include
LFLAGS=-Wall -lstdc++

all: atomic_operation_tests binary_file_viewer shared_resource_tests

atomic_operation_tests: atomic_operation_tests.o
	$(CC) $(LFLAGS) -o atomic_operation_tests atomic_operation_tests.o

atomic_operation_tests.o: atomic_operation_tests.cpp ../include/res_mgr_atomic.hpp
	$(CC) $(CFLAGS) -c atomic_operation_tests.cpp

binary_file_viewer: open_file.o
	$(CC) $(LFLAGS) -o binary_file_viewer open_file.o

open_file.o: open_file.cpp ../include/res_mgr_resource.hpp
	$(CC) $(CFLAGS) -c open_file.cpp

shared_resource_tests: shared_resource_tests.o libmutex.a
	$(CC) $(LFLAGS) -o shared_resource_tests shared_resource_tests.o -L. -lmutex

shared_resource_tests.o: shared_resource_tests.cpp ../include/res_mgr_atomic.hpp ../include/res_mgr_lock.hpp ../include/res_mgr_shared.hpp
	$(CC) $(CFLAGS) -c shared_resource_tests.cpp

libmutex.a: mutex.o
	ar -rc libmutex.a mutex.o

mutex.o: mutex.c ../include/mutex.hpp
	$(CC) $(CFLAGS) -c mutex.c

clean:
	rm -f atomic_operation_tests
	rm -f atomic_operation_tests.o
	rm -f binary_file_viewer
	rm -f open_file.o
	rm -f shared_resource_tests
	rm -f shared_resource_tests.o
	rm -f libmutex.a
	rm -f mutex.o
