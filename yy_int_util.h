/*

  MIT License

  Copyright (c) 2024-2025 Yafiyogi

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

#include <limits>

#include "yy_types.hpp"
#include "yy_type_traits.h"

namespace yafiyogi::yy_util {
namespace digits_detail {

template<size_type num>
struct DigitsHelper final
{
    static constexpr const size_type digits = 1 + DigitsHelper<num / 10>::digits;
};

template<>
struct DigitsHelper<0> final
{
    static constexpr const size_type digits = 0;
};

} // namespace digits_detail

template<typename I>
struct Digits final
{
    using int_type = yy_traits::remove_cvr_t<I>;
    static_assert(std::is_integral_v<int_type>, "Type must be integer type!");

    static constexpr const size_type digits = digits_detail::DigitsHelper<std::numeric_limits<int_type>::max()>::digits;
};

} // namespace yafiyogi::yy_util
