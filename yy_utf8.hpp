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

#include <bitset>
#include <iostream>

#include <bit>
#include <string_view>

#include "yy_types.hpp"
#include "yy_traits_span.hpp"
#include "yy_traits_string.hpp"
#include "yy_type_traits.h"

#include "yy_span.h"

namespace yafiyogi::yy_util {

enum class UTF8State: uint8_t { Ok, Start, InvalidDelim, InvalidChar, NotFound };

namespace utf8_detail {

template<typename CharType>
struct utf8_constants
{
    using char_type = yy_traits::remove_cvr_t<CharType>;

    static_assert(yy_traits::is_narrow_char_type_v<char_type>, "CharType must be char, char8_t, or uint8_t for utf8_constants!");

    static constexpr char_type utf8_flag = static_cast<char_type>(0x80);

    static constexpr char_type utf8_start = static_cast<char_type>(0xC0);
    static constexpr char_type utf8_start_mask = static_cast<char_type>(0xC0);

    static constexpr char_type utf8_2 = static_cast<char_type>(0xC0);
    static constexpr char_type utf8_2_mask = static_cast<char_type>(0xE0);

    static constexpr char_type utf8_3 = static_cast<char_type>(0xE0);
    static constexpr char_type utf8_3_mask = static_cast<char_type>(0xF0);

    static constexpr char_type utf8_4 = static_cast<char_type>(0xF0);
    static constexpr char_type utf8_4_mask = static_cast<char_type>(0xF8);

    static constexpr char_type utf8_cont_mask = static_cast<char_type>(0xC0);
    static constexpr char_type utf8_cont_bits = static_cast<char_type>(0x80);
};

template<typename SpanType,
         typename span_type_raw = yy_traits::remove_cvr_t<SpanType>,
         std::enable_if_t<yy_traits::is_narrow_char_type_v<typename span_type_raw::value_type>
                          && (yy_traits::is_span_v<span_type_raw>
                              || yy_traits::is_std_string_v<span_type_raw>
                              || yy_traits::is_std_string_view_v<span_type_raw>), bool> = true>
struct utf8_traits
{
    using value_type = yy_traits::remove_cvr_t<typename span_type_raw::value_type>;
    using span_type = span_type_raw;
    using char_ptr = std::add_pointer<value_type>;
    using const_char_ptr = std::add_pointer_t<std::add_const_t<value_type>>;

    using constants = utf8_constants<value_type>;
};

} // namespace utf8_detail


template<typename CharType>
inline size_type utf8_cp_len(CharType ch) noexcept
{
  using utf8_constants = utf8_detail::utf8_constants<CharType>;

  size_type size = (utf8_constants::utf8_flag & ch) == 0;

  if((utf8_constants::utf8_start_mask & ch) == utf8_constants::utf8_start)
  {
    size = static_cast<size_t>(std::countl_zero(static_cast<uint8_t>(~ch)));
  }

  return size;
}

template<typename CharType>
inline bool utf8_cp_valid(CharType ch) noexcept
{
  using utf8_constants = utf8_detail::utf8_constants<CharType>;

  return (0 == (utf8_constants::utf8_bits & ch))
    || (utf8_constants::utf8_2 == (ch & utf8_constants::utf8_2_mask))
    || (utf8_constants::utf8_3 == (ch & utf8_constants::utf8_3_mask))
    || (utf8_constants::utf8_4 == (ch & utf8_constants::utf8_4_mask))
    || (utf8_constants::utf8_cont_bits == (ch & utf8_constants::utf8_cont_mask));
}

struct utf8_result final
{
    size_type pos = 0;
    size_type size = 0;
    UTF8State state = UTF8State::Ok;

    bool operator==(const utf8_result & other) const noexcept
    {
      return std::tie(pos, size) == std::tie(other.pos, other.size);
    }
};

namespace utf8_detail {

template<typename SpanType,
         typename traits_type = utf8_traits<SpanType>,
         typename const_char_ptr = typename traits_type::const_char_ptr>
inline bool cp_match_ch(const_char_ptr ch,
                        const SpanType delim) noexcept
{
  bool found = true;

  switch(delim.size())
  {
    case 4:
      found = found && (ch[3] == delim[3]);
      [[fallthrough]];
    case 3:
      found = found && (ch[2] == delim[2]);
      [[fallthrough]];
    case 2:
      found = found && (ch[1] == delim[1]);
      break;
  }

  return found;
}

template<typename SpanType,
         typename traits_type = utf8_detail::utf8_traits<SpanType>,
         typename span_type = typename traits_type::span_type>
utf8_result cp_find_delim(const SpanType p_sv,
                          const SpanType p_delim) noexcept
{
  span_type data{p_sv};
  size_type pos;

  while(!data.empty())
  {
    pos = data.find_pos(p_delim[0]);

    if(pos == data.size())
    {
      // delim not found.
      break;
    }

    data.inc_begin(pos);
    if(p_delim.size() > data.size())
    {
      // delim not found.
      break;
    }

    if(utf8_detail::cp_match_ch(data.data(), p_delim))
    {
      return utf8_result{p_sv.size() - data.size(), utf8_cp_len(data[0]), UTF8State::Ok};
    }

    data.inc_begin(p_delim.size());
  }

  return utf8_result{p_sv.size(), 0, UTF8State::NotFound};
}

} // namespace utf8_detail

template<typename SpanType,
         typename traits_type = utf8_detail::utf8_traits<SpanType>,
         typename span_type = SpanType>
utf8_result utf8_cp_find(const SpanType sv,
                         const SpanType p_delim) noexcept
{
  SpanType l_delim{p_delim.data(), utf8_cp_len(p_delim[0])};

  if(l_delim.empty() || (l_delim.size() > p_delim.size()))
  {
    return utf8_result{0, 0, UTF8State::InvalidDelim};
  }

  return utf8_detail::cp_find_delim(sv, l_delim);
}

template<typename SpanType,
         typename traits_type = utf8_detail::utf8_traits<SpanType>>
utf8_result utf8_cp_find_first_not(SpanType p_sv,
                                   const SpanType p_delim) noexcept
{
  SpanType l_delim{p_delim.data(), utf8_cp_len(p_delim[0])};

  if(l_delim.empty() || (l_delim.size() > p_delim.size()))
  {
    return utf8_result{0, 0, UTF8State::InvalidDelim};
  }

  SpanType data{p_sv};
  SpanType ch{};
  UTF8State state = UTF8State::NotFound;

  while(!data.empty())
  {
    ch = SpanType{data.data(), utf8_cp_len(data[0])};

    if(ch.empty())
    {
      state = UTF8State::InvalidChar;
      break;
    }

    if(ch.size() > data.size())
    {
      break;
    }

    if((data[0] != l_delim[0]) || !utf8_detail::cp_match_ch(data.data(), l_delim))
    {
      state = UTF8State::Ok;
      break;
    }

    data.inc_begin(ch.size());
  }

  return utf8_result{p_sv.size() - data.size(), ch.size(), state};
}

template<typename SpanType,
         typename traits_type = utf8_detail::utf8_traits<SpanType>>
utf8_result utf8_cp_find_first_of(const SpanType sv,
                                  const SpanType p_delim) noexcept
{
  SpanType data{sv};
  SpanType ch{};
  UTF8State state = UTF8State::NotFound;

  while(!data.empty())
  {
    ch = SpanType{data.data(), utf8_cp_len(data[0])};

    if(ch.empty())
    {
      state = UTF8State::InvalidChar;
      break;
    }

    if(ch.size() > data.size())
    {
      state = UTF8State::NotFound;
      break;
    }

    state = utf8_detail::cp_find_delim(p_delim, ch).state;
    if(UTF8State::NotFound != state)
    {
      break;
    }

    data.inc_begin(ch.size());
  }

  return utf8_result{sv.size() - data.size(), ch.size(), state};
}

// Find start of last utf8 multi byte char
template<typename SpanType,
         typename traits_type = utf8_detail::utf8_traits<SpanType>>
utf8_result utf8_cp_find_last_ch(SpanType sv) noexcept
{
  using utf8_constants = typename traits_type::constants;
  using value_type = typename traits_type::value_type;

  const size_type size = sv.size();
  value_type ch;

  while(!sv.empty())
  {
    sv.dec_end();

    ch = *sv.end();
    if(((ch & utf8_constants::utf8_cont_mask) != utf8_constants::utf8_cont_bits)
       || ((ch & utf8_constants::utf8_start_mask) == utf8_constants::utf8_start))
    {
      UTF8State state = UTF8State::InvalidChar;
      size_type ch_len = utf8_cp_len(ch);

      if(ch_len == (size - sv.size()))
      {
        state = UTF8State::Start;
      }

      return utf8_result{sv.size(), ch_len, state};
    }
  }

  return utf8_result{0, 0, UTF8State::NotFound};
}

} // namespace yafiyogi::yy_util
