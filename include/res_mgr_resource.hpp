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

#ifndef RESOURCE_MANAGER_RESOURCE_HPP
#define RESOURCE_MANAGER_RESOURCE_HPP

namespace res_mgr {
/*
 Only one copy of resource is allowed.
 Copying the resource from a non-const resource object to another will transfer the ownership.
 Template parameters:
 1) ResourceType: the type of the resource being managed, e.g. a socket descriptor or a file handle.
 2) invalid_value: a value that represents an invalid resource or no resource.
 3) ResourceFunctor: a functor or function class which contains two overloads for operator().
    - void operator() (ResourceType resource): a function to release the resource
    - bool operator() (ResourceType resource, ResourceType invalid_value): a function to compare the resource to an invalid value

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

template <typename ResourceType, ResourceType invalid_value, class ResourceFunctor>
class Resource
{
public:
	Resource(ResourceType resource = invalid_value) : m_resource(resource)
	{
	}

	~Resource()
	{
		release();
	}

	Resource(Resource& src) : m_resource(src.m_resource)
	{
		src.m_resource = invalid_value;
	}

	Resource& operator=(Resource& src)
	{
		if (this != &src) {
			release();
			m_resource = src.m_resource;
			src.m_resource = invalid_value;
		}
		return *this;
	}

	Resource& operator=(ResourceType resource)
	{
		if (m_resource != resource) {
			release();
			m_resource = resource;
		}
		return *this;
	}

	void release()
	{
		if (is_valid())
		{
			ResourceFunctor release_;
			release_(m_resource);
			m_resource = invalid_value;
		}
	}

	ResourceType get() const
	{
		return m_resource;
	}

	bool is_valid() const
	{
		ResourceFunctor compare;
		return compare(m_resource, invalid_value);
	}

	void swap(Resource& src)
	{
		if (this != &src) {
			ResourceType resource = m_resource;
			m_resource = src.m_resource;
			src.m_resource = resource;
		}
	}

private:
	Resource(const Resource&);            // disallows constructing from const resource objects
	Resource& operator=(const Resource&); // disallows copying from const resource objects
	ResourceType m_resource;
};

} // namespace

#endif
