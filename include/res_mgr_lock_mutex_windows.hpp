/*

The MIT License (MIT)

Copyright (c) 2016 MH Lim

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

#ifndef RESOURCE_MANAGER_LOCK_MUTEX_WINDOWS_HPP_
#define RESOURCE_MANAGER_LOCK_MUTEX_WINDOWS_HPP_

#include "res_mgr_lock.hpp"
#include <Windows.h>

namespace res_mgr {

class MutexLock
{
public:
	void operator()(HANDLE hMutex)
	{
		DWORD wait_status;
		wait_status = ::WaitForSingleObject(hMutex, INFINITE);
		assert(WAIT_OBJECT_0 == wait_status);
	}
};

class MutexUnlock
{
public:
	void operator()(HANDLE hMutex)
	{
		BOOL release_status;
		release_status = ::ReleaseMutex(hMutex);
		assert(TRUE == release_status);
	}
};

typedef ResourceLockMechanism2<HANDLE, MutexLock, MutexUnlock> CMutexLock;

} // namespace

#endif
