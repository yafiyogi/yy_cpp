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

#include <array>
#include <utility>
#include <memory>
#include <type_traits>

#include "yy_type_traits.h"
#include "yy_types.hpp"

namespace yafiyogi::yy_util {

template<typename T>
struct ArraySize final
{
    static constexpr size_type size = 0;
};

template<typename T, size_type Size>
struct ArraySize<T[Size]> final
{
    static constexpr size_type size = Size;
};

template<typename T, size_type Size>
struct ArraySize<std::array<T, Size>> final
{
    static constexpr size_type size = Size;
};

template<typename T>
inline constexpr size_type array_size_v = ArraySize<T>::size;

template<typename Return,
         typename T>
constexpr std::unique_ptr<yy_traits::remove_cvr_t<Return>> static_unique_cast(std::unique_ptr<T> && ptr)
{
  using return_type = yy_traits::remove_cvr_t<Return>;
  using param_type = yy_traits::remove_cvr_t<T>;
  static_assert(std::is_base_of_v<return_type, param_type>, "static_unique_cast(): type 'Return' is not a base of type 'T'.");

  return std::unique_ptr<return_type>{static_cast<return_type *>(ptr.release())};
}

template<typename ...Types>
constexpr auto make_array(Types && ... data)
{
  using type = std::common_type<Types...>::type;
  return std::array<type, sizeof...(Types)>{std::forward<Types>(data)...};
}

} // namespace yafiyogi::yy_util
