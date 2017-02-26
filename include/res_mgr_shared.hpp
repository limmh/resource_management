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

#ifndef RESOURCE_MANAGER_SHARED_HPP_
#define RESOURCE_MANAGER_SHARED_HPP_

#include "res_mgr_atomic.hpp"
#include <exception>

namespace res_mgr {

// The resource is shared among different instances by using reference counting.
template<typename T, typename S, S invalid_value, class ReleaseFunction, typename RefCountType = long>
class SharedResource
{
public:
	SharedResource(T res = T(invalid_value)) : m_res(res), m_pRefCount(NULL)
	{
		init();
	}

	SharedResource(const SharedResource& src) : m_res(src.m_res), m_pRefCount(NULL)
	{
		if (T(invalid_value) != m_res)
		{
			m_pRefCount = src.m_pRefCount;
			atomic_increment<RefCountType>(m_pRefCount);
		}
	}

	~SharedResource()
	{
		close();
	}

	SharedResource& operator=(const SharedResource& src)
	{
		close();
		m_res = src.m_res;
		if (T(invalid_value) != m_res)
		{
			m_pRefCount = src.m_pRefCount;
			atomic_increment<RefCountType>(m_pRefCount);
		}
		return *this;
	}

	SharedResource& operator=(T res)
	{
		close();
		m_res = res;
		init();
		return *this;
	}

	void close()
	{
		if (T(invalid_value) == m_res)
		{
			m_pRefCount = NULL;
			return;
		}

		RefCountType count = atomic_decrement<RefCountType>(m_pRefCount);
		if (0 == count)
		{
			ReleaseFunction release;
			release(m_res);
			delete m_pRefCount;
		}

		m_res = invalid_value;
		m_pRefCount = NULL;
	}

	T get() const
	{
		return m_res;
	}

	bool is_valid() const
	{
		return (invalid_value != m_res);
	}

	void swap(SharedResource& src)
	{
		T temp = m_res;
		RefCountType* p = m_pRefCount;
		m_res = src.m_res;
		m_pRefCount = src.m_pRefCount;
		src.m_res = temp;
		src.m_pRefCount = p;
	}

private:
	void init()
	{
		if (T(invalid_value) == m_res)
			return;

		try
		{
			m_pRefCount = new RefCountType(1);
		}
		catch (std::exception& e)
		{
			ReleaseFunction release;
			release(m_res);
			m_res = invalid_value;
			m_pRefCount = NULL;
			throw std::exception(e);
		}
	}

	T m_res;
	RefCountType* m_pRefCount;
};

} // namespace

#endif
