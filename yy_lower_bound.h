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

#include <limits>

#include "yy_ref_traits.h"

namespace yafiyogi::yy_data {
namespace lb_detail {

template<typename T, typename V>
struct less_val final
{
    using type = yy_traits::remove_rcv_t<T>;
    using value_type = yy_traits::remove_rcv_t<V>;
    using val_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    static constexpr size_t mask = std::numeric_limits<size_t>::max();

    constexpr less_val() noexcept = default;
    constexpr less_val(const less_val &) noexcept = default;
    constexpr less_val(less_val &&) noexcept = default;

    constexpr bool operator()(const type item,
                              const value_type & val) const noexcept
    {
      return *item < val;
    }

    constexpr size_t comp_bool(const type item,
                               const value_type & val) const noexcept
    {
      return static_cast<size_t>(*item < val);
    }

    constexpr size_t comp_mask(const type item,
                               const value_type & val) const noexcept
    {
      static_assert(mask == (size_t{0} - size_t{1}), "Your platform/complier doesn't wrap unsigned integers. You are going to have to use std::lower_bound. Sorry!");

      return size_t{0} - comp_bool(item, val);
    }

    constexpr size_t comp_mask(const type item,
                               const value_type & val,
                               const size_t val_to_mask) const noexcept
    {
      return val_to_mask & comp_mask(item, val);
    }
};

inline constexpr size_t loop_mask = std::numeric_limits<size_t>::max() - size_t{1};

}

template<typename T,
         typename V,
         typename Compare>
constexpr T lower_bound(T begin, T end, const V & val, Compare comp) noexcept
{
  static_assert(std::is_pointer_v<T>, "Type 'T' must be a pointer!");

  if(begin == end)
  {
    return begin;
  }

  size_t n = static_cast<size_t>(end - begin);

  while(0 != (n & lb_detail::loop_mask))
  {
    size_t half = n >> 1;

    begin = begin + comp.comp_mask(begin + half, val, half);

    n = n - half;
  }

  return begin + comp.comp_bool(begin, val);
}

template<typename T,
         typename V>
constexpr T lower_bound(T begin, T end, const V & val) noexcept
{
  static_assert(std::is_pointer_v<T>, "Type 'T' must be a pointer!");

  return yy_data::lower_bound(begin, end, val, lb_detail::less_val<T, V>{});
}

} // namespace yafiyogi::yy_data
