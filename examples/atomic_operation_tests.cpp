/*
The MIT License (MIT)

Copyright (c) 2024 MH Lim

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// requires C++11

#include "res_mgr_atomic.hpp"
#include <atomic>
#include <assert.h>
#include <stdio.h>

#if defined _WIN32 || defined _WIN64
#include <process.h>
#include <Windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

typedef std::atomic<unsigned int> atomic_uint_type;
typedef std::atomic<int> atomic_int_type;
typedef std::atomic<bool> atomic_bool;

struct thread_data_type {
	atomic_int_type increasing_number;
	atomic_int_type decreasing_number;
	atomic_int_type increasing_number2;
	atomic_int_type decreasing_number2;
	atomic_uint_type uint1;
	atomic_uint_type uint2;
	atomic_uint_type uint3;
	atomic_uint_type thread_count;
	atomic_bool exit;

	thread_data_type() :
		increasing_number(0),
		decreasing_number(0),
		increasing_number2(0),
		decreasing_number2(0),
		uint1(0U),
		uint2(0U),
		uint3(0U),
		thread_count(0U),
		exit(false)
	{
	}
};

#if defined _WIN32 || defined _WIN64
unsigned int __stdcall thread_procedure(void *param)
#else
void *thread_procedure(void *param)
#endif
{
	assert(param != nullptr);
	thread_data_type* data = static_cast<thread_data_type*>(param);
	const unsigned int thread_id = res_mgr::atomic_increment<unsigned int, atomic_uint_type>(&(data->thread_count));

	for (;;) {
		const bool exit = res_mgr::atomic_load<bool, atomic_bool>(&(data->exit));
		if (!exit) {
			const int increasing_number = res_mgr::atomic_increment<int, atomic_int_type>(&(data->increasing_number));
			const int decreasing_number = res_mgr::atomic_decrement<int, atomic_int_type>(&(data->decreasing_number));
			const int increasing_number2 = res_mgr::atomic_add<int, atomic_int_type>(&(data->increasing_number2), 5);
			const int decreasing_number2 = res_mgr::atomic_sub<int, atomic_int_type>(&(data->decreasing_number2), 5);
			const unsigned int pattern1 = res_mgr::atomic_and<unsigned int, atomic_uint_type>(&(data->uint1), 0xFFU);
			const unsigned int pattern2 = res_mgr::atomic_or<unsigned int, atomic_uint_type>(&(data->uint2), 0xFFFFU);
			const unsigned int pattern3 = res_mgr::atomic_xor<unsigned int, atomic_uint_type>(&(data->uint3), 0xFFFFFFFFU);

			printf("Thread %u: n1=%d, n2=%d, n3=%d, n4=%d, n5=%X, n6=%X, n7=%X\n", thread_id, increasing_number, decreasing_number,
					increasing_number2, decreasing_number2, pattern1, pattern2, pattern3);
		} else {
			break;
		}

#if defined _WIN32 || defined _WIN64
		Sleep(1000);
#else
		sleep(1);
#endif
	}

#if defined _WIN32 || defined _WIN64
	return 0;
#else
	return (void*) 0;
#endif
}

#define MAX_THREAD_COUNT 3

#if defined _WIN32 || defined _WIN64
typedef HANDLE thread_handle_type;
#else
typedef pthread_t thread_handle_type;
#endif

int main(void)
{
	const int seconds = 30;
	thread_handle_type threads[MAX_THREAD_COUNT] = {};
	thread_data_type data;
	data.uint1 = 0xFFFFU;
#if defined _WIN32 || defined _WIN64
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		threads[i] = (thread_handle_type) _beginthreadex(NULL, 0, thread_procedure, &data, 0, NULL);

	Sleep(seconds * 1000);
#else
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		pthread_create(&threads[i], NULL, thread_procedure, &data);

	sleep(seconds);
#endif

	res_mgr::atomic_store(&(data.exit), true);

#if defined _WIN32 || defined _WIN64
	WaitForMultipleObjects(MAX_THREAD_COUNT, threads, TRUE, INFINITE);
#else
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		pthread_join(threads[i], NULL);
#endif

	return 0;
}
