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

template<typename SpanType>
requires yy_traits::is_span_v<SpanType>
constexpr SpanType skip_not_char(SpanType & src,
                                 const typename SpanType::value_type c) noexcept
{
  const auto pos = src.find_first(c);

  src.remove_prefix(pos);

  return src;
}

template<typename T,
         typename span_type = yy_quad::const_span<T>>
constexpr span_type skip_not_char(const T * src,
                                  size_type size, const typename span_type::value_type c) noexcept
{
  yy_quad::const_span<T> span{src, size};

  return skip_not_char(span, c);
}

template<typename T,
         typename span_type = yy_quad::const_span<T>>
constexpr span_type skip_not_char(const T * begin,
                                  const T * end, const typename span_type::value_type c) noexcept
{
  yy_quad::const_span<T> span{begin, end};

  return skip_not_char(span, c);
}

template<typename SpanType>
requires yy_traits::is_span_v<SpanType>
constexpr SpanType skip_char(SpanType & src,
                             const typename SpanType::value_type c) noexcept
{
  const auto pos = src.find_first_not(c);

  src.remove_prefix(pos);

  return src;
}

template<typename T,
         typename span_type = yy_quad::const_span<T>>
constexpr span_type skip_char(const T * src,
                              size_type size,
                              const typename span_type::value_type c) noexcept
{
  yy_quad::const_span<T> span{src, size};

  return skip_char(span, c);
}

template<typename T,
         typename span_type = yy_quad::const_span<T>>
constexpr span_type skip_char(const T * begin,
                              const T * end,
                              const typename span_type::value_type c) noexcept
{
  yy_quad::const_span<T> span{begin, end};

  return skip_char(span, c);
}

} // namespace yafiyogi::yy_util
