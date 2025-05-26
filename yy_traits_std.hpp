/*

  MIT License

  Copyright (c) 2025 Yafiyogi

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

#pragma once

#include <array>
#include <type_traits>
#include <vector>

#include "yy_type_traits.h"
#include "yy_vector_traits.h"

namespace yafiyogi::yy_traits::traits_detail {

template<typename T>
struct container_traits<std::vector<T>>:
      std::true_type
{
    using value_type = typename std::vector<T>::value_type;
};

template<typename T>
struct vector_traits<std::vector<T>>:
      std::true_type
{
};

template<typename T,
         size_type N>
struct container_traits<std::vector<T, N>>:
      std::true_type
{
    using value_type = typename std::array<T, N>::value_type;
};

template<typename T,
         size_type N>
struct vector_traits<std::array<T, N>>:
      std::true_type
{
};

} // namespace yafiyogi::yy_traits::traits_detail
