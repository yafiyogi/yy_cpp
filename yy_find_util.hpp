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

#include "yy_find_types.hpp"

namespace yafiyogi::yy_data {
namespace find_util_detail {

template<typename Iterator,
         typename ValueType,
         typename Enable = void>
struct find_pos_linear
{
    constexpr static pos_found_type find(const Iterator & p_begin,
                                         const Iterator & p_end,
                                         const ValueType & p_value) noexcept
    {
      auto found = std::find(p_begin, p_end, p_value);

      if(found == p_end)
      {
        return pos_found_type{npos, false};
      }

      return pos_found_type{static_cast<size_type>(found - p_begin), true};
    }
};

template<typename Iterator,
         typename ValueType>
struct find_pos_linear<Iterator,
                       ValueType,
                       std::enable_if_t<std::is_same_v<char, yy_traits::remove_cvr_t<ValueType>>
                                        || std::is_same_v<char8_t, yy_traits::remove_cvr_t<ValueType>>>>
{
    constexpr static pos_found_type find(const Iterator & p_begin,
                                         const Iterator & p_end,
                                         const ValueType & p_value) noexcept
    {
      auto found{std::char_traits<char>::find(p_begin, p_end - p_begin, p_value)};

      if(nullptr == found)
      {
        return pos_found_type{npos, false};
      }

      return pos_found_type{static_cast<size_type>(found - p_begin), true};
    };
};

} // namespace find_util_detail

template<typename Iterator,
         typename ValueType>
constexpr inline pos_found_type find_pos_linear(const Iterator & p_begin,
                                                const Iterator & p_end,
                                                const ValueType & p_value) noexcept
{
  return find_util_detail::find_pos_linear<Iterator, ValueType>::find(p_begin,
                                                                      p_end,
                                                                      p_value);
}

} // namespace yafiyogi::yy_data
