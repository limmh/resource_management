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
#include "res_mgr_lock.hpp"
#include "res_mgr_shared.hpp"
#include "mutex.h"

#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined _WIN32 || defined _WIN64
#include <process.h>
#include <Windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

template <typename T, size_t size>
constexpr size_t sizeof_array(T (&array)[size])
{
	return size;
}

struct MutexInitFunctor {
	void operator()(void* &mutex) {
		mutex = mutex_create();
	}
};

struct MutexDeinitFunctor {
	void operator()(void *mutex) {
		mutex_destroy(mutex);
	}
};

struct MutexLockFunctor {
	void operator()(void *mutex) {
		mutex_lock(mutex);
	}
};

struct MutexUnlockFunctor {
	void operator()(void *mutex) {
		mutex_unlock(mutex);
	}
};

struct DynamicMemoryFunctor
{
	static void* allocate(size_t number_of_bytes) {
		return calloc(number_of_bytes, sizeof(unsigned char));
	}

	void operator()(void* memory) {
		free(memory);
		// Comment the following line so that a debug message will not be printed
		printf(">> Debug message: Memory at %p released\n", memory);
	}

	bool operator()(void* memory_address, void* invalid_address) { return (memory_address != invalid_address); }
};

typedef res_mgr::SharedResource<void*, nullptr, DynamicMemoryFunctor, long, std::atomic<long>> SharedDynamicMemory;
typedef res_mgr::ResourceLock<void*, MutexInitFunctor, MutexDeinitFunctor, MutexLockFunctor, MutexUnlockFunctor> Mutex;
typedef res_mgr::ResourceLockMechanism<Mutex> MutexLock;
typedef std::atomic<unsigned int> atomic_uint_type;

struct thread_data_type {
	Mutex mutex;
	SharedDynamicMemory shared_memory;
	atomic_uint_type count;
	int thread_count;
	unsigned int n;
	char text[101];
	bool exit;

	thread_data_type() :
		count(0),
		thread_count(0),
		n(0),
		exit(false)
	{
		memset(text, 0, sizeof(text));
	}
};

#if defined _WIN32 || defined _WIN64
unsigned int __stdcall thread_procedure(void *param)
#else
void *thread_procedure(void *param)
#endif
{
	const char *text[] = {
		"Hello World",
		"The quick brown fox jumps over the lazy dog.",
		"0123456789",
		"Haha"
	};

	int thread_id = 0;
	SharedDynamicMemory shared_mem;
	thread_data_type* data = static_cast<thread_data_type*>(param);

	{
		MutexLock lock(data->mutex);
		thread_id = ++(data->thread_count);
		shared_mem = data->shared_memory;
		printf("reference count = %ld\n", shared_mem.get_refcount());
	}

	for (;;) {
		int exit = 0;
		{
			MutexLock lock(data->mutex);
			exit = data->exit;
		}

		if (!exit) {
			data->n++; // non-atomic increment
			const unsigned int count = res_mgr::atomic_increment<unsigned int, atomic_uint_type>(&(data->count));
			const size_t i = count % sizeof_array(text);
			{
				MutexLock lock(data->mutex);
				strncpy(data->text, text[i], sizeof_array(data->text));
				data->text[sizeof_array(data->text) - 1] = '\0';
				printf("Thread %d: atomic count = %u, non-atomic count = %u, text = %s, shared = %s\n",
					thread_id, count, data->n, data->text, static_cast<const char*>(shared_mem.get()));
			}
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
	constexpr size_t number_of_bytes = 11;
	thread_handle_type threads[MAX_THREAD_COUNT] = {};
	thread_data_type data;
	printf("reference count = %ld\n", data.shared_memory.get_refcount());
	data.shared_memory = DynamicMemoryFunctor::allocate(number_of_bytes);
	if (data.shared_memory.is_valid()) {
		const unsigned char digits[] = { '9','8','7','6','5','4','3','2','1','0', '\0'};
		unsigned char* mem = static_cast<unsigned char*>(data.shared_memory.get());
		static_assert(sizeof_array(digits) <= number_of_bytes, "Size mismatch.");
		for (int i = 0; i < number_of_bytes; ++i) {
			mem[i] = digits[i];
		}
	}
	printf("reference count = %ld\n", data.shared_memory.get_refcount());

#if defined _WIN32 || defined _WIN64
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		threads[i] = (thread_handle_type) _beginthreadex(NULL, 0, thread_procedure, &data, 0, NULL);

	Sleep(seconds * 1000);
#else
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		pthread_create(&threads[i], NULL, thread_procedure, &data);

	sleep(seconds);
#endif

	{
		MutexLock lock(data.mutex);
		data.exit = true;
	}

#if defined _WIN32 || defined _WIN64
	WaitForMultipleObjects(MAX_THREAD_COUNT, threads, TRUE, INFINITE);
#else
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
		pthread_join(threads[i], NULL);
#endif
	printf("reference count = %ld\n", data.shared_memory.get_refcount());
	return 0;
}
