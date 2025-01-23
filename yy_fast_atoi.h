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

#include <cstdint>

#include <string_view>
#include <type_traits>

#include "yy_int_util.h"
#include "yy_span.h"
#include "yy_type_traits.h"

namespace yafiyogi::yy_util {

enum class FastFloat:uint8_t {Ok, NoValue};

namespace fast_atoi_detail {

template<typename I>
struct val_valid_type final
{
    using value_type = yy_traits::remove_cvr_t<I>;

    value_type value{};
    FastFloat state = FastFloat::NoValue;

    friend
    constexpr bool operator==(const val_valid_type a, const val_valid_type b) noexcept
    {
      return (a.state == b.state) && (a.value == b.value);
    }
};

} // namespace fast_atoi_detail

template<typename I>
struct fast_atoi final
{
    using val_valid_type = fast_atoi_detail::val_valid_type<I>;
    using value_type = val_valid_type::value_type;

    static constexpr val_valid_type convert(yy_quad::const_span<char> p_str) noexcept
    {
      using digits_type = Digits<value_type>;

      static_assert(std::is_integral_v<value_type>, "fast_atoi() only works with integer types!");
      static_assert(digits_type::digits <= 20, "fast_atoi(): type contains too many digits!");

      if(p_str.size() > digits_type::digits)
      {
        return val_valid_type{};
      }

      value_type val = value_type{0};
      FastFloat state = FastFloat::NoValue;

      auto add = [&val, p_str]() mutable {
        // Multiply current value by 10.
        val = (val << value_type{1}) + (val << value_type{3});

        // Add next digit.
        val += static_cast<value_type>(*p_str.begin() - '0');
        p_str.inc_begin();
      };

      switch(p_str.size())
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
          state = FastFloat::Ok;
      }

      return val_valid_type{val, state};
    }

    static constexpr val_valid_type convert(std::string_view p_str) noexcept
    {
      return convert(yy_quad::make_const_span(p_str));
    }
};

} // namespace yafiyogi::yy_util
