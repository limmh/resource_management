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

#ifndef RESOURCE_MANAGER_ATOMIC_HPP
#define RESOURCE_MANAGER_ATOMIC_HPP

#include <cassert>
#include <cstddef>

namespace res_mgr {

/*
Template parameters:
1) IntegerType: an integer type, e.g. int, unsigned int, long, unsigned long, etc.
2) AtomicType: a type (class or struct) based on IntegerType, the type should provide the following operators for atomic operations
   IntegerType operator++(): pre-increment
   IntegerType operator--(): pre-decrement
   operator IntegerType() const: returns the underlying integer value
   IntegerType operator=(IntegerType): assignment
   IntegerType operator+=(IntegerType): atomic addition
   IntegerType operator-=(IntegerType): atomic subtraction
   IntegerType operator&=(IntegerType): atomic bitwise AND
   IntegerType operator!=(IntegerType): atomic bitwise OR
   IntegerType operator^=(IntegerType): atomic bitwise XOR

AtomicType can be OS specific or std::atomic<IntegerType>.
*/
template<typename IntegerType, class AtomicType>
inline IntegerType atomic_increment(AtomicType *p_atomic)
{
	assert(p_atomic != NULL);
	return ++(*p_atomic);
}

template<typename IntegerType, class AtomicType>
inline IntegerType atomic_decrement(AtomicType *p_atomic)
{
	assert(p_atomic != NULL);
	return --(*p_atomic);
}

template<typename IntegerType, class AtomicType>
inline IntegerType atomic_load(AtomicType *p_atomic)
{
	assert(p_atomic != NULL);
	return static_cast<IntegerType>(*p_atomic);
}

// returns the previous value
template<typename IntegerType, class AtomicType>
inline IntegerType atomic_store(AtomicType *p_atomic, IntegerType new_value)
{
	assert(p_atomic != NULL);
	const IntegerType old_value = static_cast<IntegerType>(*p_atomic);
	*p_atomic = new_value;
	return old_value;
}

template<typename IntegerType, class AtomicType>
inline IntegerType atomic_add(AtomicType *p_atomic, IntegerType value)
{
	assert(p_atomic != NULL);
	return (*p_atomic += value);
}

template<typename IntegerType, class AtomicType>
inline IntegerType atomic_sub(AtomicType *p_atomic, IntegerType value)
{
	assert(p_atomic != NULL);
	return (*p_atomic -= value);
}

template<typename IntegerType, class AtomicType>
inline IntegerType atomic_and(AtomicType *p_atomic, IntegerType value)
{
	assert(p_atomic != NULL);
	return (*p_atomic &= value);
}

template<typename IntegerType, class AtomicType>
inline IntegerType atomic_or(AtomicType *p_atomic, IntegerType value)
{
	assert(p_atomic != NULL);
	return (*p_atomic |= value);
}

template<typename IntegerType, class AtomicType>
inline IntegerType atomic_xor(AtomicType *p_atomic, IntegerType value)
{
	assert(p_atomic != NULL);
	return (*p_atomic ^= value);
}

} // namespace

#endif
