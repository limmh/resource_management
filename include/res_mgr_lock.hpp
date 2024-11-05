/*

The MIT License (MIT)

Copyright (c) 2016 - 2024 MH Lim

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

#ifndef RESOURCE_MANAGER_LOCK_HPP
#define RESOURCE_MANAGER_LOCK_HPP

namespace res_mgr {

template<typename LockType, class InitFunctor, class DeinitFunctor, class LockFunctor, class UnlockFunctor>
class ResourceLock
{
public:
	ResourceLock()
	{
		InitFunctor init;
		init(m_lock);
	}

	~ResourceLock()
	{
		DeinitFunctor deinit;
		deinit(m_lock);
	}

	void lock()
	{
		LockFunctor lock;
		lock(m_lock);
	}

	void unlock()
	{
		UnlockFunctor unlock;
		unlock(m_lock);
	}

	LockType& get()
	{
		return m_lock;
	}

private:
	LockType m_lock;
};

template<typename ResourceLock>
class ResourceLockMechanism
{
public:
	explicit ResourceLockMechanism(ResourceLock& lock) : m_lock(lock)
	{
		m_lock.lock();
	}

	~ResourceLockMechanism()
	{
		m_lock.unlock();
	}

private:
	ResourceLock& m_lock;
};

} // namespace

#endif
