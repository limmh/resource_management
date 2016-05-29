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

#ifndef RESOURCE_MANAGER_ATOMIC_HPP_
#define RESOURCE_MANAGER_ATOMIC_HPP_

namespace res_mgr {

// The implementations are OS specific.
// Implement the atomic operations according to the platform that you want to support

template<typename T> inline T atomic_increment(T* p_shared);
template<typename T> inline T atomic_decrement(T* p_shared);
template<typename T> inline T atomic_load(T* p_shared);
template<typename T> inline T atomic_store(T* p_shared, T new_value);
template<typename T> inline T atomic_add(T* p_shared, T value);
template<typename T> inline T atomic_sub(T* p_shared, T value);
template<typename T> inline T atomic_or(T* p_shared, T value);
template<typename T> inline T atomic_and(T* p_shared, T value);
template<typename T> inline T atomic_xor(T* p_shared, T value);

// more to be added ...

} // namespace

#endif
