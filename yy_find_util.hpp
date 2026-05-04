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

#include <algorithm>
#include <type_traits>

#include "yy_type_traits.h"
#include "yy_find_types.hpp"

namespace yafiyogi::yy_data {
namespace find_util_detail {

template<typename ValueType>
struct find_pos_linear
{
    template<typename Iterator>
    constexpr static pos_found_type find(const Iterator & p_begin,
                                         const Iterator & p_end,
                                         const ValueType & p_value) noexcept
    {
      const auto found = std::find(p_begin, p_end, p_value);

      return pos_found_type{static_cast<size_type>(found - p_begin), found == p_end};
    }

    template<typename Iterator>
    constexpr static pos_found_type find(const Iterator & p_begin,
                                         const size_type & p_size,
                                         const ValueType & p_value) noexcept
    {
      return find(p_begin, p_begin + p_size, p_value);
    }
};

template<typename ValueType>
requires yy_traits::is_narrow_char_type<ValueType>
  || yy_traits::is_wide_char_type<ValueType>
struct find_pos_linear<ValueType>
{
    template<typename Iterator>
    constexpr static pos_found_type find(const Iterator & p_begin,
                                         const size_type & p_size,
                                         const ValueType & p_value) noexcept
    {
      auto found{std::char_traits<ValueType>::find(p_begin, p_size, p_value)};

      const bool is_found = nullptr != found;

      if(!is_found)
      {
        found = p_begin + p_size;
      }

      return pos_found_type{static_cast<size_type>(found - p_begin), is_found};
    };

    template<typename Iterator>
    constexpr static pos_found_type find(const Iterator & p_begin,
                                         const Iterator & p_end,
                                         const ValueType & p_value) noexcept
    {
      return find(p_begin, static_cast<size_type>(p_end - p_begin), p_value);
    };
};

} // namespace find_util_detail

template<typename Iterator,
         typename ValueType>
constexpr inline pos_found_type find_pos_linear(const Iterator & p_begin,
                                                const Iterator & p_end,
                                                const ValueType & p_value) noexcept
{
  return find_util_detail::find_pos_linear<ValueType>::find(p_begin,
                                                            p_end,
                                                            p_value);
}

template<typename Iterator,
         typename ValueType>
constexpr inline pos_found_type find_pos_linear(const Iterator & p_begin,
                                                const size_type & p_size,
                                                const ValueType & p_value) noexcept
{
  return find_util_detail::find_pos_linear<ValueType>::find(p_begin,
                                                            p_size,
                                                            p_value);
}

template<typename ValueStore>
requires yy_traits::is_container_v<ValueStore>
constexpr inline pos_found_type find_pos_linear(const ValueStore & p_store,
                                                const typename ValueStore::value_type & p_value) noexcept
{
  using value_type = typename ValueStore::value_type;

  return find_util_detail::find_pos_linear<value_type>::find(p_store.begin(),
                                                             p_store.end(),
                                                             p_value);
}

} // namespace yafiyogi::yy_data
