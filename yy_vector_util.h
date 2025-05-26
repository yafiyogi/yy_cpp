/*

  MIT License

  Copyright (c) 2021-2025 Yafiyogi

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

#include <cstddef>

#include <algorithm>
#include <tuple>
#include <type_traits>

#include "yy_type_traits.h"
#include "yy_traits_array.hpp"
#include "yy_traits_string.hpp"
#include "yy_traits_vector.hpp"

namespace yafiyogi::yy_util {

template<typename T,
         std::enable_if_t<yafiyogi::yy_traits::is_vector_v<T>
                          || yafiyogi::yy_traits::is_array_v<T>, bool> = true>
constexpr T sort(T && container)
{
  std::sort(container.data(), container.data() + container.size());

  return container;
}

template<typename T,
         std::enable_if_t<yafiyogi::yy_traits::is_vector_v<T>, bool> = true>
constexpr void unique(T & container)
{
  container.erase(std::unique(container.begin(),
                              container.end()),
                  container.end());
}

template<typename T>
constexpr void shrink(T & container)
{
  T{container}.swap(container);
}

template<typename T>
constexpr void shrink(const T & src, T & dst)
{
  T{src}.swap(dst);
}

} // namespace yafiyogi::yy_util
