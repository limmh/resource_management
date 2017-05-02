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

#ifndef RESOURCE_MANAGER_RESOURCE_HPP_
#define RESOURCE_MANAGER_RESOURCE_HPP_

#include <exception>

namespace res_mgr {

// Only one copy of resource is allowed.
// Copying resource from one resource manager object to another will transfer the ownership.
// T is the type of the resource being managed, e.g. a socket descriptor or a file handle.
// ReleaseFunction is a function object which overloads the operator() to provide a way to release the resource.
// e.g.
// class ReleaseFunction {
// public:
//     void operator() (int sockfd) {
//         ::close(sockfd);
//     }
// };

template<typename T, typename S, S invalid_value, class ReleaseFuncType>
class Resource
{
public:
	Resource(T res = T(invalid_value)) : m_res(res)
	{
	}

	~Resource()
	{
		close();
	}

	Resource(const Resource& src) : m_res(src.m_res)
	{
		Resource& r = const_cast<Resource&>(src);
		r.m_res = T(invalid_value);
	}

	Resource& operator=(const Resource& src)
	{
		close();
		m_res = src.m_res;
		Resource& r = const_cast<Resource&>(src);
		r.m_res = T(invalid_value);
		return *this;
	}

	Resource& operator=(T res)
	{
		close();
		m_res = res;
		return *this;
	}

	void close()
	{
		if (T(invalid_value) != m_res)
		{
			ReleaseFuncType release;
			release(m_res);
			m_res = invalid_value;
		}
	}

	T get() const
	{
		return m_res;
	}

	bool is_valid() const
	{
		return (T(invalid_value) != m_res);
	}

	void swap(Resource& src)
	{
		T temp = m_res;
		m_res = src.m_res;
		src.m_res = temp;
	}

private:
	T m_res;
};

} // namespace

#endif
