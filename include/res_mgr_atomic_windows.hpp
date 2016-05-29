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

#ifndef RESOURCE_MANAGER_ATOMIC_WINDOWS_HPP_
#define RESOURCE_MANAGER_ATOMIC_WINDOWS_HPP_

#include "res_mgr_atomic.hpp"
#include <Windows.h>

namespace res_mgr {

// template specialization is used to provide OS specific implementations

template<> inline long atomic_increment<long>(long* p_shared)
{
	return InterlockedIncrement(const_cast<volatile long*>(p_shared));
}

template<> inline long long atomic_increment<long long>(long long* p_shared)
{
	return InterlockedIncrement64(const_cast<volatile long long*>(p_shared));
}

template<> inline long atomic_decrement<long>(long* p_shared)
{
	return InterlockedDecrement(const_cast<volatile long*>(p_shared));
}

template<> inline long long atomic_decrement<long long>(long long* p_shared)
{
	return InterlockedIncrement64(const_cast<volatile long long*>(p_shared));
}

template<> inline long atomic_store<long>(long* p_shared, long new_value)
{
	return InterlockedExchange(const_cast<volatile long*>(p_shared), new_value);
}

template<> inline long atomic_load<long>(long* p_shared)
{
	return InterlockedExchangeAdd(const_cast<volatile long*>(p_shared), 0);
}

template<> inline long atomic_add<long>(long* p_shared, long value)
{
	return InterlockedExchangeAdd(const_cast<volatile long*>(p_shared), value);
}

template<> inline long atomic_sub<long>(long* p_shared, long value)
{
	return InterlockedExchangeAdd(const_cast<volatile long*>(p_shared), -1 * value);
}

// implementations incomplete, more to add at a later time

} // namespace

#endif
