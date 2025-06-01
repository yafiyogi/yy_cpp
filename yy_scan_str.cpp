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

#include "yy_scan_str.hpp"

namespace yafiyogi::yy_util::scan_str_detail {

size_type find_first_of(scan_type p_source,
                        const scan_type p_chars) noexcept
{
  const char * src = p_source.data();
  size_type str_size = p_source.size();

  for(auto ch : p_chars)
  {
    if(auto end = static_cast<const char *>(std::memchr(src, ch, str_size));
       nullptr != end)
    {
      str_size = static_cast<std::size_t>(end - src);
    }
  }

  return str_size;
}

size_type find_first_not_of(scan_type p_source,
                            const scan_type p_chars) noexcept
{
  size_type size = p_source.size();

  for(scan_type::value_type src : p_source)
  {
    if(nullptr == std::memchr(p_chars.data(), src, p_chars.size()))
    {
      break;
    }
    --size;
  }

  return p_source.size() - size;
}

size_type get_width(scan_type & p_format) noexcept
{
  size_type width = default_width;
  size_type width_width = std::min(max_width_width, p_format.size());

  scan_type l_format{p_format.subspan(0, width_width)};

  if(auto last = find_first_not_of(l_format, g_digits);
     0 != last)
  {
    l_format.dec_end(l_format.size() - last);

    if(auto width_val{yy_util::fast_atoi<size_type>::convert(l_format)};
       FastFloat::Ok == width_val.state)
    {
      width = width_val.value;
      p_format.inc_begin(l_format.size());
    }
  }

  return width;
}

bool scan_leading(scan_type & p_source,
                  scan_type & p_format) noexcept
{
  for(const char src : p_source)
  {
    if(p_format.empty())
    {
      return false;
    }

    const auto fmt = p_format[0];

    if((0 == src)
       || (0 == fmt))
    {
      return false;
    }

    if('%' == fmt)
    {
      // Consume leading '%'.
      p_format.inc_begin();

      if(p_format.empty())
      {
        return false;
      }

      if('%' != p_format[0])
      {
        return true;
      }
    }

    if(src != fmt)
    {
      return false;
    }

    p_format.inc_begin();
    p_source.inc_begin();
  }

  return false;
}

void scan_span(scan_type & p_source,
               span_type p_value) noexcept
{
  size_type copy_size = std::min(p_value.size() - 1, p_source.size());

  char * dest = p_value.data();
  memcpy(dest, p_source.data(), copy_size);
  *(dest + copy_size) = 0;

  p_source = scan_type{p_source.data(), copy_size};
}

size_type scan_string(scan_type & p_source,
                      std::string & p_dest) noexcept
{
  size_type copy_size = p_source.size();

  p_dest.reserve(copy_size);
  p_dest.assign(p_source.data(), copy_size);

  p_source.inc_begin(copy_size);
  return 1;
}

} // namespace yafiyogi::yy_util::scan_str_detail
