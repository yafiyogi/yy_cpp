/*

  MIT License

  Copyright (c) 2022-2024 Yafiyogi

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

#include <type_traits>
#include <vector>

#include "yy_clear_action.h"
#include "yy_type_traits.h"
#include "yy_vector_traits.h"

namespace yafiyogi::yy_quad {

template<typename T,
         std::size_t N,
         yy_data::ClearAction ct>
class static_vector;

template<typename T,
         std::size_t N,
         yy_data::ClearAction ct>
class static_simple_vector;

} // namespace yafiyogi::yy_quad

namespace yafiyogi::yy_traits::traits_detail {

template<typename T,
         std::size_t N,
         yy_data::ClearAction ct>
struct vector_traits<yy_quad::static_vector<T, N, ct>>:
      std::true_type
{
};

template<typename T,
         std::size_t N,
         yy_data::ClearAction ct>
struct container_traits<yy_quad::static_vector<T, N, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::static_vector<T, N, ct>::value_type;
};

template<typename T,
         std::size_t N,
         yy_data::ClearAction ct>
struct vector_traits<yy_quad::static_simple_vector<T, N, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::static_simple_vector<T, N, ct>::value_type;
};

template<typename T,
         std::size_t N,
         yy_data::ClearAction ct>
struct container_traits<yy_quad::static_simple_vector<T, N, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::static_simple_vector<T, N, ct>::value_type;
};

} // namespace yafiyogi::yy_vector_traits::traits_detail
