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

#include "yy_utf8.hpp"

namespace yafiyogi::yy_util {
namespace {

inline bool match_ch(std::string_view ch,
                     std::string_view delim) noexcept
{
  const char * delim_data = delim.data();
  const size_type delim_size = delim.size();
  const char * ch_data = ch.data();
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

bool find_ch(std::string_view ch,
             std::string_view delim) noexcept
{
  const char * data = delim.data();
  size_type size = delim.size();
  size_type delim_len;

  while(size != 0)
  {
    delim_len = utf8_len(*data);

    if((delim_len == 0) || (delim_len > size))
    {
      return false;
    }

    if(match_ch(ch, std::string_view{data, delim_len}))
    {
      return true;
    }

    data += delim_len;
    size -= delim_len;
  }

  return false;
}

} // anonymous namespace

utf8_result utf8_find(std::string_view sv,
                      std::string_view delim) noexcept
{
  const char * data = sv.data();
  size_type data_size = sv.size();
  const char * data_new;
  const char * end = data + sv.size();
  const char * delim_data = delim.data();
  size_type ch_size{};

  while(data_size != 0)
  {
    data_new = std::char_traits<char>::find(data, data_size, *delim_data);

    if(nullptr == data_new)
    {
      break;
    }

    ch_size = utf8_len(*data_new);

    if((0 == ch_size) || (ch_size > static_cast<size_type>(end - data_new)))
    {
      break;
    }

    if(match_ch(std::string_view{data_new, ch_size}, delim))
    {
      return utf8_result{static_cast<size_type>(data_new - data), ch_size};
    }

    data = data_new + ch_size;
    data_size -= ch_size;
  }

  return utf8_result{sv.size(), 0};
}

utf8_result utf8_find_first_of(std::string_view sv,
                               std::string_view delim) noexcept
{
  const char * data = sv.data();
  size_type size = sv.size();
  std::string_view ch;
  size_type ch_size{};

  while(size != 0)
  {
    ch_size = utf8_len(*data);

    if((ch_size == 0) || (ch_size > size))
    {
      break;
    }

    ch = std::string_view{data, ch_size};

    if(find_ch(ch, delim))
    {
      return utf8_result{sv.size() - size, ch_size};
    }

    data += ch_size;
    size -= ch_size;
  }

  return utf8_result{sv.size(), ch_size};
}

utf8_result utf8_find_last_ch(std::string_view sv) noexcept
{
  if(sv.empty())
  {
    return yy_util::utf8_result{sv.size(), 0};
  }

  size_type size = sv.size();
  const char * data = sv.begin() + size;
  char ch;

  while(size != 0)
  {
    --size;
    --data;

    ch = *data;
    if(((ch & utf8_detail::utf8_bits_mask) != utf8_detail::utf8_bits)
       || ((ch & utf8_detail::utf8_start_mask) == utf8_detail::utf8_start))
    {
      return utf8_result{size, utf8_len(*data)};
    }
  }

  return utf8_result{sv.size(), 0};
}

} // namespace yafiyogi::yy_util
