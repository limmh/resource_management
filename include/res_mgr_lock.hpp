/*

The MIT License (MIT)

Copyright (c) 2016 - 2017 MH Lim

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

#ifndef RESOURCE_MANAGER_LOCK_HPP_
#define RESOURCE_MANAGER_LOCK_HPP_

namespace res_mgr {

template<typename T, class InitFunc, class UninitFunc>
class ResourceLock
{
public:
	ResourceLock()
	{
		InitFunc init;
		init(&m_resLock);
	}

	~ResourceLock()
	{
		UninitFunc uninit;
		uninit(&m_resLock);
	}

	operator T*()
	{
		return &m_resLock;	
	}

private:
	T m_resLock;
};

template<typename T, class LockFunc, class UnlockFunc>
class ResourceLockMechanism
{
public:
	explicit ResourceLockMechanism(T* pResLock) : m_pResLock(pResLock)
	{
		LockFunc lock;
		lock(m_pResLock);
	}

	~ResourceLockMechanism()
	{
		UnlockFunc unlock;
		unlock(m_pResLock);
	}

private:
	T* m_pResLock;
};

template<typename T, class LockFunc, class UnlockFunc>
class ResourceLockMechanism2
{
public:
	explicit ResourceLockMechanism2(T resLockHandle) : m_resLockHandle(resLockHandle)
	{
		LockFunc lock;
		lock(resLockHandle);
	}

	~ResourceLockMechanism2()
	{
		UnlockFunc unlock;
		unlock(m_resLockHandle);
	}

private:
	T m_resLockHandle;
};

} // namespace

#endif
