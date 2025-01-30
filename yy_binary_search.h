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

#include <cstddef>

#include <limits>

#include "yy_lower_bound.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"

namespace yafiyogi::yy_data {
namespace bs_detail {

template<typename T, typename V>
struct equal_val final
{
    using type = yy_traits::remove_cvr_t<T>;
    using value_type = yy_traits::remove_cvr_t<V>;
    using val_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    static constexpr size_t mask = std::numeric_limits<size_t>::max();

    constexpr equal_val() noexcept = default;
    constexpr equal_val(const equal_val &) noexcept = default;
    constexpr equal_val(equal_val &&) noexcept = default;
    constexpr ~equal_val() noexcept = default;

    constexpr equal_val & operator=(const equal_val &) noexcept = default;
    constexpr equal_val & operator=(equal_val &&) noexcept = default;

    [[nodiscard]]
    constexpr bool operator()(const type item,
                              const value_type & val) const noexcept
    {
      return *item == val;
    }
};

template<typename T>
struct result final
{
    T item{};
    bool found = false;
};

} // namespace bs_detail

template<typename T,
         typename V,
         typename CompareLT = lb_detail::less_val<T, V>,
         typename CompareEQ = bs_detail::equal_val<T, V>>
[[nodiscard]]
constexpr bs_detail::result<T> binary_search(T begin,
                                             T end,
                                             const V & val,
                                             CompareLT comp_lt = CompareLT{},
                                             CompareEQ comp_eq = CompareEQ{}) noexcept
{
  T found = yy_data::lower_bound(begin, end, val, comp_lt);

  return bs_detail::result<T>{found, (found != end) && comp_eq(found, val)};
}

} // namespace yafiyogi::yy_data
