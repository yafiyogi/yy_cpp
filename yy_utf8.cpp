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

inline bool match_ch(std::string_view ch, std::string_view delim) noexcept
{
  const char * delim_data = delim.data();
  const size_t delim_size = delim.size();
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

bool find_ch(std::string_view ch, std::string_view delim) noexcept
{
  const char * delim_data = delim.data();
  const char * delim_end = delim_data + delim.size();
  size_t delim_len;

  while(delim_data != delim_end)
  {
    delim_len = utf8_len(*delim_data);

    if(delim_len > static_cast<size_t>(delim_end - delim_data))
    {
      return false;
    }

    if(match_ch(ch, std::string_view{delim_data, delim_len}))
    {
      return true;
    }

    delim_data += delim_len;
  }

  return false;
}

} // anonymous namespace

std::string_view find(std::string_view sv, std::string_view delim) noexcept
{
  const char * data = sv.data();
  const char * end = data + sv.size();
  size_t size;
  std::string_view ch;
  size_t ch_size;

  while(data != end)
  {
    size = static_cast<size_t>(end - data);
    data = std::char_traits<char>::find(data, size, *data);

    if(!data)
    {
      break;
    }

    ch_size = utf8_len(*data);

    if(ch_size > size)
    {
      break;
    }

    ch = std::string_view{data, ch_size};
    if(match_ch(ch, delim))
    {
      return ch;
    }

    data += ch_size;
  }

  return std::string_view{end, 0};
}

std::string_view find_first_of(std::string_view sv, std::string_view delim) noexcept
{
  const char * data = sv.data();
  const char * end = data + sv.size();
  std::string_view ch;
  size_t ch_size;

  while(data != end)
  {
    ch_size = utf8_len(*data);

    if(ch_size > static_cast<size_t>(end - data))
    {
      break;
    }

    ch = std::string_view{data, ch_size};

    if(find_ch(ch, delim))
    {
      return ch;
    }
    data += ch_size;
  }

  return std::string_view{end, 0};
}

size_t utf8_find_last(std::string_view sv) noexcept
{
  if(sv.empty())
  {
    return std::string_view::npos;
  }

  const char * start = sv.data();
  const char * data = start + sv.size();
  char ch{};

  while(data-- != start)
  {
    ch = *data;
    if (((ch & utf8_detail::utf8_bits_mask) != utf8_detail::utf8_bits)
        || ((ch & utf8_detail::utf8_start_mask) == utf8_detail::utf8_start))
    {
      return static_cast<size_t>(data - start);
    }
  }

  return std::string_view::npos;
}

} // namespace yafiyogi::yy_util
