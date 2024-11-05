/*
The MIT License (MIT)

Copyright (c) 2017 - 2024 MH Lim

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

#include "mutex.h"
#include <stdlib.h>

#if defined _WIN32 || defined _WIN64
#include <Windows.h>
#else
#include <pthread.h>
#endif

#if defined _WIN32 || defined _WIN64

static void *windows_mutex_create()
{
#ifdef MUTEX_USE_WINDOWS_MUTEX
	HANDLE m = CreateMutex(NULL, FALSE, NULL);
#else
	CRITICAL_SECTION *m = (CRITICAL_SECTION*) calloc(1, sizeof *m);
	if (m)
		InitializeCriticalSection(m);
#endif
	return (void*) m;
}

static void windows_mutex_destroy(void *mutex)
{
#ifdef MUTEX_USE_WINDOWS_MUTEX
	HANDLE m = (HANDLE) mutex;
	if (m)
		CloseHandle(m);
#else
	CRITICAL_SECTION *m = (CRITICAL_SECTION*) mutex;
	if (m) {
		DeleteCriticalSection(m);
		free(m);
	}
#endif
}

static int windows_mutex_lock(void *mutex)
{
#ifdef MUTEX_USE_WINDOWS_MUTEX
	HANDLE m = (HANDLE) mutex;
	return (WaitForSingleObject(m, INFINITE) == WAIT_OBJECT_0) ? 0 : -1;
#else
	CRITICAL_SECTION *m = (CRITICAL_SECTION*) mutex;
	EnterCriticalSection(m);
	return 0;
#endif
}

static int windows_mutex_unlock(void *mutex)
{
#ifdef MUTEX_USE_WINDOWS_MUTEX
	HANDLE m = (HANDLE) mutex;
	return (ReleaseMutex(m) != FALSE) ? 0 : -1;
#else
	CRITICAL_SECTION *m = (CRITICAL_SECTION*) mutex;
	LeaveCriticalSection(m);
	return 0;
#endif
}

#endif /* #if defined _WIN32 || defined _WIN64 */

void *mutex_create()
{
#if defined _WIN32 || defined _WIN64
	return windows_mutex_create();
#else
	pthread_mutex_t *m = (pthread_mutex_t*) calloc(1, sizeof(*m));
	if (m)
		pthread_mutex_init(m, NULL);
	return (void*) m;
#endif
}

void mutex_destroy(void *mutex)
{
#if defined _WIN32 || defined _WIN64
	windows_mutex_destroy(mutex);	
#else
	pthread_mutex_t *m = (pthread_mutex_t*) mutex;
	if (m) {
		pthread_mutex_destroy(m);
		free(m);
	}
#endif
}

/* returns 0 on success, non-zero otherwise */
int mutex_lock(void *mutex)
{
#if defined _WIN32 || defined _WIN64
	return windows_mutex_lock(mutex);
#else
	pthread_mutex_t *m = (pthread_mutex_t*) mutex;
	return pthread_mutex_lock(m);
#endif
}

int mutex_unlock(void *mutex)
{
#if defined _WIN32 || defined _WIN64
	return windows_mutex_unlock(mutex);
#else
	pthread_mutex_t *m = (pthread_mutex_t*) mutex;
	return pthread_mutex_unlock(m);
#endif
}
