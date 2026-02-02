/*

  MIT License

  Copyright (c) 2025 Yafiyogi

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

#include <stddef.h>
#include <string_view>

#include "yy_types.hpp"
#include "yy_traits_span.hpp"
#include "yy_traits_string.hpp"
#include "yy_type_traits.h"

namespace yafiyogi::yy_util {
namespace utf8_detail {

template<typename CharType>
struct utf8_constants
{
    using char_type = yy_traits::remove_cvr_t<CharType>;

    static_assert(yy_traits::is_narrow_char_type_v<char_type>, "CharType must be char, char8_t, or uint8_t for utf8_constants!");

    static constexpr char_type utf8_bits_mask = static_cast<char_type>(0xC0);
    static constexpr char_type utf8_bits = static_cast<char_type>(0x80);

    static constexpr char_type utf8_start = static_cast<char_type>(0xC0);
    static constexpr char_type utf8_start_mask = static_cast<char_type>(0xC0);

    static constexpr char_type utf8_2 = static_cast<char_type>(0xC0);
    static constexpr char_type utf8_2_mask = static_cast<char_type>(0xE0);

    static constexpr char_type utf8_3 = static_cast<char_type>(0xE0);
    static constexpr char_type utf8_3_mask = static_cast<char_type>(0xF0);

    static constexpr char_type utf8_4 = static_cast<char_type>(0xF0);
    static constexpr char_type utf8_4_mask = static_cast<char_type>(0xF8);
};

template<typename SpanType,
         typename span_type_raw = yy_traits::remove_cvr_t<SpanType>,
         std::enable_if_t<yy_traits::is_narrow_char_type_v<typename span_type_raw::value_type>
                          && (yy_traits::is_span_v<span_type_raw>
                              || yy_traits::is_std_string_v<span_type_raw>
                              || yy_traits::is_std_string_view_v<span_type_raw>), bool> = true>
struct utf8_traits
{
    using span_type = span_type_raw;
    using value_type = yy_traits::remove_cvr_t<typename span_type::value_type>;
    using char_ptr = std::add_pointer<value_type>;
    using const_char_ptr = std::add_pointer_t<std::add_const_t<value_type>>;

    using constants = utf8_constants<value_type>;
};

} // namespace utf8_detail


template<typename CharType>
inline size_type utf8_len(CharType ch) noexcept
{
  using utf8_constants = utf8_detail::utf8_constants<CharType>;

  size_type size = (utf8_constants::utf8_bits & ch) == 0;

  if((utf8_constants::utf8_start_mask & ch) == utf8_constants::utf8_start)
  {
    size = 1;

    size += (utf8_constants::utf8_2 & ch) == utf8_constants::utf8_2;
    size += (utf8_constants::utf8_3 & ch) == utf8_constants::utf8_3;
    size += (utf8_constants::utf8_4 & ch) == utf8_constants::utf8_4;
  }

  return size;
}

namespace utf8_detail {

template<typename SpanType>
inline bool match_ch(SpanType ch,
                     SpanType delim) noexcept
{
  using traits_type = utf8_detail::utf8_traits<SpanType>;
  using const_char_ptr = typename traits_type::const_char_ptr;

  const_char_ptr delim_data = delim.data();
  const size_type delim_size = delim.size();
  const_char_ptr ch_data = ch.data();
  bool found = ch.size() == delim_size;

  if(found)
  {
    switch(delim_size)
    {
    case 4:
      found = found && (*ch_data++ == *delim_data++);
      [[fallthrough]];
    case 3:
      found = found && (*ch_data++ == *delim_data++);
      [[fallthrough]];
    case 2:
      found = found && (*ch_data++ == *delim_data++);
      [[fallthrough]];
    case 1:
      found = found && (*ch_data++ == *delim_data++);
      break;
    }
  }

  return found;
}

} // namespace utf8_detail

struct utf8_result final
{
    size_type pos{};
    size_type size{};

    bool operator==(const utf8_result & other) const noexcept
    {
      return std::tie(pos, size) == std::tie(other.pos, other.size);
    }
};

template<typename SpanType>
utf8_result utf8_find(SpanType sv,
                      SpanType delim) noexcept
{
  using traits_type = utf8_detail::utf8_traits<SpanType>;
  using span_type = typename traits_type::span_type;
  using value_type = typename traits_type::value_type;
  using const_char_ptr = typename traits_type::const_char_ptr;

  const_char_ptr data_begin = sv.data();
  size_type data_size = sv.size();
  const_char_ptr data_end = data_begin + data_size;

  const_char_ptr data = data_begin;
  const_char_ptr data_new;
  const_char_ptr delim_data = delim.data();
  size_type ch_size{};

  while(data_size != 0)
  {
    data_new = std::char_traits<value_type>::find(data, data_size, *delim_data);

    if(nullptr == data_new)
    {
      break;
    }

    ch_size = utf8_len(*data_new);

    if((0 == ch_size) || (ch_size > static_cast<size_type>(data_end - data_new)))
    {
      break;
    }

    if(utf8_detail::match_ch(span_type{data_new, ch_size}, delim))
    {
      return utf8_result{static_cast<size_type>(data_new - data_begin), ch_size};
    }

    data = data_new + ch_size;
    data_size -= static_cast<size_type>(data_end - data);
  }

  return utf8_result{data_size - data_size, 0};
}

template<typename SpanType>
utf8_result utf8_find_first_of(SpanType sv,
                               SpanType delim) noexcept
{
  using traits_type = utf8_detail::utf8_traits<SpanType>;
  using const_char_ptr = typename traits_type::const_char_ptr;
  using span_type = typename traits_type::span_type;

  const_char_ptr data = sv.data();
  size_type data_size = sv.size();
  span_type ch;
  size_type ch_size{};

  while(data_size != 0)
  {
    ch_size = utf8_len(*data);

    if((ch_size == 0) || (ch_size > data_size))
    {
      break;
    }

    ch = span_type{data, ch_size};

    if(find(delim, ch).size != 0)
    {
      break;
    }

    data += ch_size;
    data_size -= ch_size;
  }

  return utf8_result{sv.size() - data_size, ch_size};
}

// Find start of last utf8 multi byte char
template<typename SpanType>
utf8_result utf8_find_last_ch(SpanType sv) noexcept
{
  using traits_type = utf8_detail::utf8_traits<SpanType>;
  using utf8_constants = typename traits_type::constants;
  using const_char_ptr = typename traits_type::const_char_ptr;
  using value_type = typename traits_type::value_type;

  if(sv.empty())
  {
    return yy_util::utf8_result{sv.size(), 0};
  }

  size_type data_size = sv.size();
  const_char_ptr data = sv.data() + data_size;
  value_type ch;

  while(data_size != 0)
  {
    --data_size;
    --data;

    ch = *data;
    if(((ch & utf8_constants::utf8_bits_mask) != utf8_constants::utf8_bits)
       || ((ch & utf8_constants::utf8_start_mask) == utf8_constants::utf8_start))
    {
      return utf8_result{data_size, utf8_len(*data)};
    }
  }

  return utf8_result{data_size, 0};
}

} // namespace yafiyogi::yy_util
