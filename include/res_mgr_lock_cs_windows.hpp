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

#ifndef RESOURCE_MANAGER_LOCK_CS_WINDOWS_HPP_
#define RESOURCE_MANAGER_LOCK_CS_WINDOWS_HPP_

#include "res_mgr_lock.hpp"
#include <Windows.h>

namespace res_mgr {

class CriticalSectionInit
{
public:
	void operator()(CRITICAL_SECTION* p_cs)
	{
		::InitializeCriticalSection(p_cs);
	}
};

template<DWORD spin_count>
class CriticalSectionInitWithSpinCount
{
public:
	void operator()(CRITICAL_SECTION* p_cs)
	{
		::InitializeCriticalSectionAndSpinCount(p_cs, spin_count);
	}
};

class CriticalSectionUninit
{
public:
	void operator()(CRITICAL_SECTION* p_cs)
	{
		::DeleteCriticalSection(p_cs);
	}
};

class CriticalSectionLock
{
public:
	void operator()(CRITICAL_SECTION* p_cs)
	{
		::EnterCriticalSection(p_cs);
	}
};

class CriticalSectionUnlock
{
public:
	void operator()(CRITICAL_SECTION* p_cs)
	{
		::LeaveCriticalSection(p_cs);
	}
};

typedef ResourceLock<CRITICAL_SECTION, CriticalSectionInitWithSpinCount<4000>, CriticalSectionUninit> CCriticalSection;
typedef ResourceLock<CRITICAL_SECTION, CriticalSectionInit, CriticalSectionUninit> CCriticalSection2;
typedef ResourceLockMechanism<CRITICAL_SECTION, CriticalSectionLock, CriticalSectionUnlock> CCriticalSectionLock;

} // namespace

#endif
