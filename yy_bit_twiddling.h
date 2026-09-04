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

#include <bitset>
#include <cstdint>
#include <iostream>
#include <limits>

#include "yy_types.hpp"

namespace yafiyogi::yy_bit_twiddling {
namespace bits_detail {

template<typename T>
struct bits
{
    using value_type = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<value_type>::digits;

    static constexpr value_type multiplier =
      static_cast<value_type>(std::numeric_limits<value_type>::max() / value_type{0xff});
    static constexpr value_type three_three = multiplier * value_type{0x33};
    static constexpr value_type five_five = multiplier * value_type{0x55};
    static constexpr value_type eight_zero = multiplier * value_type{0x80};
    static constexpr value_type zero_one = multiplier * value_type{0x01};
    static constexpr value_type zero_f = multiplier * value_type{0x0f};
};

template<typename T, typename bits = bits<T>, typename value_type = typename bits::value_type>
constexpr int pop(value_type val) noexcept
{
  val = val - ((val >> 1) & bits::five_five);
  val = (val & bits::three_three) + ((val >> 2) & bits::three_three);

  return static_cast<int>((((val + (val >> 4)) & bits::zero_f) * bits::zero_one)
                          >> (bits::digits - 8));
}

template<typename T, typename bits = bits<T>, typename value_type = typename bits::value_type>
constexpr int ntz(value_type val) noexcept
{
  return bits::digits - pop<value_type>(static_cast<value_type>(val | -val));
}

template<typename T, typename bits = bits<T>, typename value_type = typename bits::value_type>
constexpr value_type has_zero_raw(value_type v) noexcept
{
  return (v - bits::zero_one) & (v) &bits::eight_zero;
}

template<typename T, typename value_type = typename bits<T>::value_type>
constexpr bool has_zero(value_type v) noexcept
{
  return 0 != has_zero_raw(v);
}

template<typename T, typename bits = bits<T>, typename value_type = typename bits::value_type>
constexpr bool has_value(value_type v, uint8_t n) noexcept
{
  return value_type{0} != (has_zero_raw(v) ^ (bits::multiplier * n));
}

template<typename T, size_type Bits>
struct BitsShift
{
    using value_type = typename bits<T>::value_type;

    static constexpr value_type shift(value_type val)
    {
      val = BitsShift<T, Bits / 2>::shift(val);
      val |= val >> value_type{Bits >> 1};

      return val;
    }
};

template<typename T>
struct BitsShift<T, 0>
{
    using value_type = typename bits<T>::value_type;

    static constexpr value_type shift(value_type val)
    {
      return val;
    }
};

template<typename T, typename bits = bits<T>, typename value_type = typename bits::value_type>
constexpr value_type round_up_pow2(value_type val) noexcept
{
  --val;

  val = BitsShift<value_type, bits::digits>::shift(val);

  ++val;

  return val;
}

template<typename T, typename bits = bits<T>, typename value_type = typename bits::value_type>
constexpr value_type round_down_pow2(value_type val) noexcept
{
  val = BitsShift<value_type, bits::digits>::shift(val);

  return val - (val >> value_type{1});
}

template<typename T, typename bits = bits<T>, typename value_type = typename bits::value_type>
constexpr int nlz(value_type val) noexcept
{
  val = BitsShift<value_type, bits::digits>::shift(val);

  return pop<value_type>(static_cast<value_type>(~val));
}

} // namespace bits_detail

template<typename T, typename value_type = typename bits_detail::bits<T>::value_type>
constexpr int pop(T val)
{
  return bits_detail::pop<value_type>(static_cast<value_type>(val));
}

template<typename T, typename value_type = typename bits_detail::bits<T>::value_type>
constexpr int ntz(T val)
{
  return bits_detail::ntz<value_type>(static_cast<value_type>(val));
}

template<typename T, typename value_type = typename bits_detail::bits<T>::value_type>
constexpr value_type has_zero(T val)
{
  return bits_detail::has_zero<value_type>(static_cast<value_type>(val));
}

template<typename T, typename value_type = typename bits_detail::bits<T>::value_type>
constexpr value_type has_value(T val)
{
  return bits_detail::has_value<value_type>(static_cast<value_type>(val));
}

template<typename T, typename value_type = typename bits_detail::bits<T>::value_type>
constexpr value_type round_up_pow2(T val) noexcept
{
  return bits_detail::round_up_pow2<value_type>(val);
}

template<typename T, typename value_type = typename bits_detail::bits<T>::value_type>
constexpr value_type round_down_pow2(T val) noexcept
{
  return bits_detail::round_down_pow2<value_type>(static_cast<value_type>(val));
}

template<typename T, typename value_type = typename bits_detail::bits<T>::value_type>
constexpr int nlz(T val) noexcept
{
  return bits_detail::nlz<value_type>(static_cast<value_type>(val));
}

} // namespace yafiyogi::yy_bit_twiddling
