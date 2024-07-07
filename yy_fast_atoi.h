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

#include <string_view>
#include <type_traits>

#include "yy_span.h"
#include "yy_int_util.h"

namespace yafiyogi::yy_util {

template<typename I>
constexpr inline I fast_atoi(std::string_view p_str)
{
  static_assert(std::is_integral_v<I>, "fast_atoi() only works with integer types!");
  static_assert(Digits<I>::digits <= 20, "fast_atoi(): type contains too many digits!");

  I val = 0;

  auto str_span = yy_quad::make_const_span(p_str);

  auto add = [&val, str_span]() mutable {
    // multiply by 10
    val = (val << 1) + (val << 3);
    val += *str_span.begin() - '0';
    str_span.inc_begin();
  };

  switch(str_span.size())
  {
    case 20:
      add();
      [[fallthrough]];
    case 19:
      add();
      [[fallthrough]];
    case 18:
      add();
      [[fallthrough]];
    case 17:
      add();
      [[fallthrough]];
    case 16:
      add();
      [[fallthrough]];
    case 15:
      add();
      [[fallthrough]];
    case 14:
      add();
      [[fallthrough]];
    case 13:
      add();
      [[fallthrough]];
    case 12:
      add();
      [[fallthrough]];
    case 11:
      add();
      [[fallthrough]];
    case 10:
      add();
      [[fallthrough]];
    case 9:
      add();
      [[fallthrough]];
    case 8:
      add();
      [[fallthrough]];
    case 7:
      add();
      [[fallthrough]];
    case 6:
      add();
      [[fallthrough]];
    case 5:
      add();
      [[fallthrough]];
    case 4:
      add();
      [[fallthrough]];;
    case 3:
      add();
      [[fallthrough]];
    case 2:
      add();
      [[fallthrough]];
    case 1:
      add();
  }

  return val;
}

} // namespace yafiyogi::yy_util
