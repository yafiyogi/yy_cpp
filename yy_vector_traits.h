/*

  MIT License

  Copyright (c) 2022-2025 Yafiyogi

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

namespace yafiyogi::yy_quad {

template<typename T,
         yy_data::ClearAction ct>
class vector;

template<typename T,
         yy_data::ClearAction ct>
class simple_vector;

} // namespace yafiyogi::yy_quad

namespace yafiyogi::yy_traits {
namespace traits_detail {

template<typename T>
struct vector_traits:
      std::false_type
{
};

template<typename T,
         yy_data::ClearAction ct>
struct vector_traits<yy_quad::vector<T, ct>>:
      std::true_type
{
};

template<typename T,
         yy_data::ClearAction ct>
struct container_traits<yy_quad::vector<T, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::vector<T, ct>::value_type;
};

template<typename T,
         yy_data::ClearAction ct>
struct vector_traits<yy_quad::simple_vector<T, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::simple_vector<T, ct>::value_type;
};

template<typename T,
         yy_data::ClearAction ct>
struct container_traits<yy_quad::simple_vector<T, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::simple_vector<T, ct>::value_type;
};

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

} // namespace traits_detail

/** @brief is_vector type trait */
template<typename T>
using is_vector = traits_detail::vector_traits<remove_cvr_t<T>>;

template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

template<typename T>
using is_vector_t = typename is_vector<T>::type;

} // namespace yafiyogi::yy_vector_traits
