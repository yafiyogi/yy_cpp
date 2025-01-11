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

#include <algorithm>
#include <type_traits>

#include "yy_type_traits.h"

namespace yafiyogi::yy_util {

template<typename A,
         typename B,
         std::enable_if_t<std::is_same_v<typename yy_traits::remove_cvr_t<A>::value_type,
                                         typename yy_traits::remove_cvr_t<B>::value_type>
                          && !std::is_same_v<char, typename yy_traits::remove_cvr_t<A>::value_type>, bool> = true>
constexpr bool equal(A && a, B && b) noexcept
{
  const auto a_size{a.size()};
  const auto a_begin{a.data()};

  return (a_size == b.size())
    && std::equal(a_begin, a_begin + a_size, b.data());
}

template<typename A,
         typename B,
         std::enable_if_t<std::is_same_v<char, typename yy_traits::remove_cvr_t<A>::value_type>
                          && std::is_same_v<char, typename yy_traits::remove_cvr_t<B>::value_type>, bool> = true>
constexpr bool equal(A && a, B && b) noexcept
{
  const auto a_size{a.size()};

  return (a_size == b.size())
    && (0 == std::char_traits<char>::compare(a.data(), b.data(), a_size));
}

template<typename A,
         typename B,
         std::enable_if_t<std::is_same_v<typename yy_traits::remove_cvr_t<A>::value_type,
                                         typename yy_traits::remove_cvr_t<B>::value_type>
                          && !std::is_same_v<char, typename yy_traits::remove_cvr_t<A>::value_type>, bool> = true>
constexpr bool less_than(A && a, B && b) noexcept
{
  const auto a_size{a.size()};
  const auto a_begin{a.data()};
  const auto b_size{b.size()};
  const auto b_begin{b.data()};

  return std::lexicographical_compare(a_begin, a_begin + a_size,
                                      b_begin, b_begin + b_size);
}

template<typename A,
         typename B,
         std::enable_if_t<std::is_same_v<char, typename yy_traits::remove_cvr_t<A>::value_type>
                          && std::is_same_v<char, typename yy_traits::remove_cvr_t<B>::value_type>, bool> = true>
constexpr bool less_than(A && a, B && b) noexcept
{
  const auto a_size{a.size()};
  const auto b_size{b.size()};
  const auto size{std::min(a_size, b_size)};

  auto comp{std::char_traits<char>::compare(a.data(), b.data(), size)};

  return (comp < 0) || ((0 == comp) && (a_size < b_size));
}

} // namespace yafiyogi::yy_util
