/*

  MIT License

  Copyright (c) 2024-2026 Yafiyogi

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

#include "yy_int_util.h"
#include "yy_span.h"
#include "yy_type_traits.h"
#include "yy_types.hpp"

namespace yafiyogi::yy_util {

enum class FastNum:uint8_t {Ok, NoValue};

namespace fast_atoi_detail {

template<typename I>
struct val_valid_type final
{
    using value_type = yy_traits::remove_cvr_t<I>;

    value_type value{};
    FastNum state = FastNum::NoValue;

    friend
    constexpr bool operator==(const val_valid_type a, const val_valid_type b) noexcept
    {
      return (a.state == b.state) && (a.value == b.value);
    }
};

template<typename ValueType,
         size_type Idx>
inline void add_digit(ValueType & val, yy_quad::const_span<char> p_str) noexcept
{
  static_assert(Idx > 0, "add<Idx>(): Idx must be > 0.");
  // Multiply current value by 10.
  val *= 10;

  // Add next digit.
  val += static_cast<ValueType>(p_str[Idx - 1] - '0');
};

} // namespace fast_atoi_detail

template<typename I>
struct fast_atoi_idx final
{
    static constexpr size_type max_digits = 20;
    using val_valid_type = fast_atoi_detail::val_valid_type<I>;
    using value_type = val_valid_type::value_type;

    static constexpr val_valid_type convert(yy_quad::const_span<char> p_str) noexcept
    {
      using digits_type = Digits<value_type>;

      static_assert(std::is_integral_v<value_type>, "fast_atoi() only works with integer types!");
      static_assert(digits_type::digits <= max_digits, "fast_atoi(): type contains too many digits!");

      if(p_str.size() > digits_type::digits)
      {
        return val_valid_type{};
      }

      value_type val = value_type{};
      FastNum state = FastNum::NoValue;

      switch(p_str.size())
      {
        case 20:
          fast_atoi_detail::add_digit<value_type, 20>(val, p_str);
          [[fallthrough]];

        case 19:
          fast_atoi_detail::add_digit<value_type, 19>(val, p_str);
          [[fallthrough]];

        case 18:
          fast_atoi_detail::add_digit<value_type, 18>(val, p_str);
          [[fallthrough]];

        case 17:
          fast_atoi_detail::add_digit<value_type, 17>(val, p_str);
          [[fallthrough]];

        case 16:
          fast_atoi_detail::add_digit<value_type, 16>(val, p_str);
          [[fallthrough]];

        case 15:
          fast_atoi_detail::add_digit<value_type, 15>(val, p_str);
          [[fallthrough]];

        case 14:
          fast_atoi_detail::add_digit<value_type, 14>(val, p_str);
          [[fallthrough]];

        case 13:
          fast_atoi_detail::add_digit<value_type, 13>(val, p_str);
          [[fallthrough]];

        case 12:
          fast_atoi_detail::add_digit<value_type, 12>(val, p_str);
          [[fallthrough]];

        case 11:
          fast_atoi_detail::add_digit<value_type, 11>(val, p_str);
          [[fallthrough]];

        case 10:
          fast_atoi_detail::add_digit<value_type, 10>(val, p_str);
          [[fallthrough]];

        case 9:
          fast_atoi_detail::add_digit<value_type, 9>(val, p_str);
          [[fallthrough]];

        case 8:
          fast_atoi_detail::add_digit<value_type, 8>(val, p_str);
          [[fallthrough]];

        case 7:
          fast_atoi_detail::add_digit<value_type, 7>(val, p_str);
          [[fallthrough]];

        case 6:
          fast_atoi_detail::add_digit<value_type, 6>(val, p_str);
          [[fallthrough]];

        case 5:
          fast_atoi_detail::add_digit<value_type, 5>(val, p_str);
          [[fallthrough]];

        case 4:
          fast_atoi_detail::add_digit<value_type, 4>(val, p_str);
          [[fallthrough]];;

        case 3:
          fast_atoi_detail::add_digit<value_type, 3>(val, p_str);
          [[fallthrough]];

        case 2:
          fast_atoi_detail::add_digit<value_type, 2>(val, p_str);
          [[fallthrough]];

        case 1:
          fast_atoi_detail::add_digit<value_type, 1>(val, p_str);
          state = FastNum::Ok;
      }

      return val_valid_type{val, state};
    }

    static constexpr val_valid_type convert(std::string_view p_str) noexcept
    {
      return convert(yy_quad::make_const_span(p_str));
    }
};

} // namespace yafiyogi::yy_util
