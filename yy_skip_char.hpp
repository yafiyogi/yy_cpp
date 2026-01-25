/*

  MIT License

  Copyright (c) 2026 Yafiyogi

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

#include "yy_span.h"
#include "yy_traits_string.hpp"
#include "yy_types.hpp"

namespace yafiyogi::yy_util {

template<typename SpanType,
         typename span_type = yy_traits::remove_cvr_t<SpanType>,
         std::enable_if_t<yy_traits::is_span_v<span_type> || yy_traits::is_std_string_v<span_type>
                            || yy_traits::is_std_string_view_v<span_type>, bool> = true>
inline span_type skip_not_char(const span_type src,
                               const typename span_type::value_type c)
{
  const char * src_data = src.data();
  size_type src_size = src.size();

  while(src_size != 0)
  {
    if(*src_data != c)
    {
      break;
    }
    --src_size;
    ++src_data;
  }

  return span_type{src_data, src_size};
}

template<typename T>
inline const T * skip_not_char(const T * src,
                               size_type size, T c)
{
  return skip_not_char(yy_quad::const_span<T>(src, size), c).data();
}

template<typename T>
inline const T * skip_not_char(const T * begin,
                               const T * end, T c)
{
  return skip_not_char(yy_quad::const_span<T>(begin, end), c).data();
}

template<typename SpanType,
         typename span_type = yy_traits::remove_cvr_t<SpanType>,
         std::enable_if_t<yy_traits::is_span_v<span_type> || yy_traits::is_std_string_v<span_type>
                            || yy_traits::is_std_string_view_v<span_type>, bool> = true>
inline span_type skip_char(const span_type src,
                           typename span_type::value_type c)
{
  const char * src_data = src.data();
  size_type src_size = src.size();

  while(src_size != 0)
  {
    if(*src_data != c)
    {
      break;
    }
    --src_size;
    ++src_data;
  }

  return span_type{src_data, src_size};
}

template<typename T>
inline const T * skip_char(const T * src,
                           size_type size,
                           T c)
{
  return skip_char(yy_quad::const_span<T>(src, size), c).data();
}

template<typename T>
inline const T * skip_char(const T * begin,
                           const T * end,
                           T c)
{
  return skip_char(yy_quad::const_span<T>(begin, end), c).data();
}

} // namespace yafiyogi::yy_util
