/*

  MIT License

  Copyright (c) 2024 Yafiyogi

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

#include "yy_type_traits.h"
#include "yy_ref_traits.h"
#include "yy_span.h"

namespace yafiyogi::yy_util {
namespace tokenizer_detail {

template<typename T>
class traits final
{
  public:
    using value_type = yy_traits::remove_rcv_t<T>;
    using const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using span_type = yy_quad::const_span<value_type>;
};

} // namespace tokenizer_detail

template<typename T>
class tokenizer final
{
  public:
    using traits = tokenizer_detail::traits<T>;
    using value_type = typename traits::value_type;
    using const_l_value_ref = typename traits::const_l_value_ref;
    using span_type = typename traits::span_type;
    using iterator = typename span_type::const_iterator;
    using size_type = typename span_type::size_type;

    inline static constexpr size_type none{};

    constexpr tokenizer(span_type p_span, const_l_value_ref p_delim) noexcept:
      m_span(p_span),
      m_delim(p_delim)
    {
    }

    constexpr tokenizer() noexcept = default;
    constexpr tokenizer(const tokenizer &) noexcept = default;
    constexpr tokenizer(tokenizer &&) noexcept = default;

    constexpr tokenizer & operator=(const tokenizer &) noexcept = default;
    constexpr tokenizer & operator=(tokenizer &&) noexcept = default;

    [[nodiscard]]
    constexpr span_type scan() noexcept
    {
      ++m_token_idx;
      m_pos = std::find(m_span.begin(), m_span.end(), m_delim);

      m_more = m_pos != m_span.end();

      m_token = span_type{m_span.begin(), m_pos};
      m_span = span_type{m_pos, m_span.end()};
      m_span.inc_begin();

      return token();
    }

    [[nodiscard]]
    constexpr span_type token() const noexcept
    {
      return m_token;
    }

    // 0 -> no tokens.
    // 1 -> token 1 found.
    // 2 -> token 2 found.
    // ... etc
    [[nodiscard]]
    constexpr size_type token_idx() const noexcept
    {
      return m_token_idx;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_span.empty() && !more();
    }

    [[nodiscard]]
    constexpr bool more() const noexcept
    {
      return m_more;
    }

    constexpr span_type source()
    {
      return m_span;
    }

  private:
    span_type m_token;
    size_type m_token_idx = none;
    span_type m_span;
    iterator m_pos{};
    value_type m_delim{};
    bool m_more = false;
};



} // namespace yafiyogi::yy_util
