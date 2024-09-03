/*

  MIT License

  Copyright (c) 2021-2024 Yafiyogi

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

#include "yy_type_traits.h"

namespace yafiyogi {
namespace yy_quad {

template<typename ValueType>
class span;

template<typename ValueType>
class const_span;

} //namespace yy_quad

namespace yy_traits {
namespace traits_detail {

template<typename T>
struct container_traits<yy_quad::span<T>>:
      std::true_type
{
};

template<typename T>
struct container_traits<yy_quad::const_span<T>>:
      std::true_type
{
};

template<typename T>
struct span_traits:
      std::false_type
{
};

template<typename T>
struct span_traits<yy_quad::span<T>>:
      std::true_type
{
};

template<typename T>
struct span_traits<yy_quad::const_span<T>>:
      std::true_type
{
};

} // namespace traits_detail

template<typename T>
using is_span = traits_detail::span_traits<T>;

template<typename T>
using is_span_t = typename is_span<T>::type;

template<typename T>
inline constexpr bool is_span_v = is_span<T>::value;

} // namespace yy_traits
} // namespace yafiyogi
