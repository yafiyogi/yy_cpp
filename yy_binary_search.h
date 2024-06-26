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

#include "yy_lower_bound.h"

namespace yafiyogi::yy_data {
namespace bs_detail {

template<typename T, typename V>
struct equal_val final
{
    using type = yy_traits::remove_rcv_t<T>;
    using value_type = yy_traits::remove_rcv_t<V>;
    using val_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    static constexpr size_t mask = std::numeric_limits<size_t>::max();

    constexpr equal_val() noexcept = default;
    constexpr equal_val(const equal_val &) noexcept = default;
    constexpr equal_val(equal_val &&) noexcept = default;

    constexpr bool operator()(const type item,
                              const value_type & val) const noexcept
    {
      return *item == val;
    }
};

template<typename T>
struct result final
{
    T item;
    bool found;
};

} // namespace bs_detail

template<typename T,
         typename V,
         typename CompareLT,
         typename CompareEQ>
constexpr bs_detail::result<T> binary_search(T begin,
                                             T end,
                                             const V & val,
                                             CompareLT comp_lt,
                                             CompareEQ comp_eq) noexcept
{
  T found = yy_data::lower_bound(begin, end, val, comp_lt);

  return bs_detail::result<T>{found, (found != end) && comp_eq(found, val)};
}

template<typename T,
         typename V>
constexpr bs_detail::result<T> binary_search(T begin,
                                             T end,
                                             const V & val) noexcept
{
  return yy_data::binary_search(begin, end, val, lb_detail::less_val<T, V>{}, bs_detail::equal_val<T, V>{});
}

} // namespace yafiyogi::yy_data
