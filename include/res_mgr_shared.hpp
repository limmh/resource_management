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

#ifndef RESOURCE_MANAGER_SHARED_HPP
#define RESOURCE_MANAGER_SHARED_HPP

#include "res_mgr_atomic.hpp"
#include <exception>

namespace res_mgr {
/*
The resource is shared among different instances by using reference counting.
The reference counter is thread safe, but the resource is not thread safe.
Template parameters:
1) ResourceType: the type of the resource being managed, e.g. a socket descriptor or a file handle.
2) invalid_value: a value that represents an invalid resource or no resource.
3) ResourceFunctor: a functor or function class which contains two overloads for operator().
	- void operator() (ResourceType resource): a function to release the resource
	- bool operator() (ResourceType resource, ResourceType invalid_value): a function to compare the resource to an invalid value
4) RefCountType: internal integer type for the reference count variable, e.g. int
5) RefCountAtomicType: atomic type for the reference count variable, e.g. std::atomic<int>

e.g.
class SocketFunctor {
public:
	 void operator() (int sockfd) {
		 ::close(sockfd);
	 }
	 bool operator(int resource_value, int invalid_value) { // invalid_value refers to the second function template parameter
		  return (resource_value <= invalid_value);
	 }
	 // optional: static member functions
	 // no other non-static members
}
*/
template<typename ResourceType, ResourceType invalid_value, class ResourceFunctor, typename RefCountType, typename RefCountAtomicType>
class SharedResource
{
public:
	SharedResource(ResourceType res = invalid_value) : m_res(res), m_pRefCount(NULL)
	{
		init();
	}

	SharedResource(const SharedResource& src) : m_res(src.m_res), m_pRefCount(NULL)
	{
		if (invalid_value != m_res)
		{
			m_pRefCount = src.m_pRefCount;
			atomic_increment<RefCountType, RefCountAtomicType>(m_pRefCount);
		}
	}

	~SharedResource()
	{
		release();
	}

	SharedResource& operator=(const SharedResource& src)
	{
		if (this != &src)
		{
			release();
			m_res = src.m_res;
			if (is_valid())
			{
				m_pRefCount = src.m_pRefCount;
				atomic_increment<RefCountType>(m_pRefCount);
			}
		}
		return *this;
	}

	SharedResource& operator=(ResourceType res)
	{
		if (m_res != res)
		{
			release();
			m_res = res;
			init();
		}
		return *this;
	}

	void release()
	{
		if (is_valid())
		{
			const RefCountType count = atomic_decrement<RefCountType, RefCountAtomicType>(m_pRefCount);
			if (0 == count)
			{
				ResourceFunctor release_;
				release_(m_res);
				delete m_pRefCount;
			}
			m_res = invalid_value;
			m_pRefCount = NULL;
		}
	}

	ResourceType get() const
	{
		return m_res;
	}

	bool is_valid() const
	{
		ResourceFunctor compare;
		return compare(m_res, invalid_value);
	}

	void swap(SharedResource& src)
	{
		if (this != &src)
		{
			ResourceType temp = m_res;
			RefCountType* p = m_pRefCount;
			m_res = src.m_res;
			m_pRefCount = src.m_pRefCount;
			src.m_res = temp;
			src.m_pRefCount = p;
		}
	}

	RefCountType get_refcount() const
	{
		return (m_pRefCount != NULL) ? atomic_load<RefCountType, RefCountAtomicType>(m_pRefCount) : 0;
	}

private:
	void init()
	{
		if (is_valid())
		{
			try
			{
				m_pRefCount = new RefCountAtomicType;
				atomic_store<RefCountType, RefCountAtomicType>(m_pRefCount, 1);
			}
			catch (std::exception& e)
			{
				ResourceFunctor release_;
				release_(m_res);
				m_res = invalid_value;
				m_pRefCount = NULL;
				throw std::exception(e);
			}
		}
	}

	ResourceType m_res;
	RefCountAtomicType *m_pRefCount;
};

} // namespace

#endif
