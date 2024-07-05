/*

  MIT License

  Copyright (c) 2024 Yafiyogi

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

#include <hash>

namespace yafiyogi::yy_data {

// Adapted from https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3876.pdf

template<typename T>
inline constexpr st::size_t hash_combine(std::size_t & seed, const T & val) noexcept
{
  seed ^= std::hash<T>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

  return seed;
}

template <typename T, typename... Types>
inline constexpr st::size_t hash_combine(std::size_t& seed,
                                         const T& val,
                                         const Types&... args) noexcept
{
  hash_combine(seed, val);
  hash_combine(seed, args...);

  return seed;
}

inline constexpr st::size_t hash_combine(std::size_t & seed) noexcept
{
  return seed;
}

template <typename... Types>
inline constexpr std::size_t hash_val(const Types&... args) noexcept
{
  std::size_t seed = 0;

  return hash_combine(seed, args...);
}

} // namespace yafiyogi::yy_data
