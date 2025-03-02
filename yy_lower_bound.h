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

#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_types.hpp"

namespace yafiyogi::yy_data {
namespace lb_detail {

template<typename T, typename V>
struct less_val final
{
    using type = yy_traits::remove_cvr_t<T>;
    using value_type = yy_traits::remove_cvr_t<V>;
    using val_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    static constexpr size_type mask = std::numeric_limits<size_type>::max();

    constexpr less_val() noexcept = default;
    constexpr less_val(const less_val &) noexcept = default;
    constexpr less_val(less_val &&) noexcept = default;
    constexpr ~less_val() noexcept = default;

    constexpr less_val & operator=(const less_val &) noexcept = default;
    constexpr less_val & operator=(less_val &&) noexcept = default;

    [[nodiscard]]
    constexpr bool operator()(const type item,
                              const value_type & val) const noexcept
    {
      return *item < val;
    }

    [[nodiscard]]
    constexpr size_type comp_bool(const type item,
                                    const value_type & val) const noexcept
    {
      return static_cast<size_type>(*item < val);
    }

    [[nodiscard]]
    constexpr size_type comp_mask(const type item,
                                    const value_type & val) const noexcept
    {
      static_assert(mask == (size_type{0} - size_type{1}), "Your platform/complier doesn't wrap unsigned integers. You are going to have to use std::lower_bound. Sorry!");

      return size_type{0} - comp_bool(item, val);
    }

    [[nodiscard]]
    constexpr size_type comp_mask(const type item,
                                    const value_type & val,
                                    const size_type val_to_mask) const noexcept
    {
      return val_to_mask & comp_mask(item, val);
    }
};

inline constexpr size_type loop_mask = std::numeric_limits<size_type>::max() - size_type{1};

}

template<typename T,
         typename V,
         typename Compare = lb_detail::less_val<T, V>>
[[nodiscard]]
constexpr T lower_bound(T begin, T end, const V & val, Compare comp = Compare{}) noexcept
{
  static_assert(std::is_pointer_v<T>, "Type 'T' must be a pointer!");

  if(begin == end)
  {
    return begin;
  }

  auto n = static_cast<size_type>(end - begin);

  while(0 != (n & lb_detail::loop_mask))
  {
    size_type half = n >> 1;

    begin = begin + comp.comp_mask(begin + half, val, half);

    n = n - half;
  }

  return begin + comp.comp_bool(begin, val);
}

} // namespace yafiyogi::yy_data
