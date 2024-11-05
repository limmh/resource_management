cmake_minimum_required(VERSION 3.10)

project(tests)

add_executable(binary_file_viewer open_file.cpp ../include/res_mgr_resource.hpp)
target_include_directories(binary_file_viewer PUBLIC ../include)

add_library(mutex mutex.c ../include/mutex.h)
target_include_directories(mutex PUBLIC ../include)
if (UNIX)
	target_link_libraries(mutex pthread)
endif (UNIX)

add_executable(shared_resource_tests shared_resource_tests.cpp ../include/mutex.h ../include/res_mgr_atomic.hpp ../include/res_mgr_lock.hpp ../include/res_mgr_shared.hpp)
target_include_directories(shared_resource_tests PUBLIC ../include)
target_link_libraries(shared_resource_tests mutex)

add_executable(atomic_operation_tests atomic_operation_tests.cpp ../include/res_mgr_atomic.hpp)
target_include_directories(atomic_operation_tests PUBLIC ../include)
