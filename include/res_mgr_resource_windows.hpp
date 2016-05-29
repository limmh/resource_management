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

#ifndef RESOURCE_MANAGER_RESOURCE_WINDOWS_HPP_
#define RESOURCE_MANAGER_RESOURCE_WINDOWS_HPP_

#include "res_mgr_resource.hpp"
#include "res_mgr_atomic_windows.hpp"
#include <assert.h>

namespace res_mgr {

class CCloseHandle
{
public:
	void operator()(HANDLE h)
	{
		BOOL status;
		status = ::CloseHandle(h);
		assert(TRUE == status);
	}
};

typedef Resource<HANDLE, NULL, CCloseHandle> HandleManager;
typedef SharedResource<HANDLE, NULL, CCloseHandle> SharedHandleManager;

typedef Resource<HANDLE, INVALID_HANDLE_VALUE, CCloseHandle> FileHandleManager;
typedef SharedResource<HANDLE, INVALID_HANDLE_VALUE, CCloseHandle> SharedFileHandleManager;

} // namespace

#endif
