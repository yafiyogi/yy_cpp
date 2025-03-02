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

#include <tuple>

#include "yy_types.hpp"

namespace yafiyogi::yy_traits {

template<typename... Args>
struct arg_traits
{
    using tuple_type = std::tuple<Args...>;
    static constexpr size_type value = std::tuple_size<tuple_type>::value;

    template<size_type N>
    using arg_type = typename std::tuple_element<N, tuple_type>::type;
};

template<typename... Args>
inline constexpr bool arg_traits_v = arg_traits<Args...>::value;

template<size_type N, typename... Args>
using arg_traits_t = typename arg_traits<Args...>::template arg_type<N>;

} // namespace yafiyogi::yy_traits
