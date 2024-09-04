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
    using value_type = T;
    using const_value_ptr = std::add_pointer_t<std::add_const_t<T>>;
    using const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using token_type = yy_quad::const_span<value_type>;
};

} // namespace tokenizer_detail

template<typename T>
class tokenizer
{
  public:
    using traits = tokenizer_detail::traits<T>;
    using value_type = typename traits::value_type;
    using const_value_ptr = typename traits::const_value_ptr;
    using const_l_value_ref = typename traits::const_l_value_ref;
    using token_type = typename traits::token_type;
    using size_type = typename token_type::size_type;

    static constexpr size_type none{};

    constexpr explicit tokenizer(token_type p_source, const_l_value_ref p_delim) noexcept:
      m_source(p_source),
      m_delim(p_delim)
    {
    }

    constexpr tokenizer() noexcept = default;
    constexpr tokenizer(const tokenizer &) noexcept = default;
    constexpr tokenizer(tokenizer &&) noexcept = default;

    constexpr tokenizer & operator=(const tokenizer &) noexcept = default;
    constexpr tokenizer & operator=(tokenizer &&) noexcept = default;

    [[nodiscard]]
    constexpr token_type scan() noexcept
    {
      if(!empty())
      {
        ++m_token_idx;
      }

      const_value_ptr source_begin = m_source.begin();
      const_value_ptr source_end = m_source.end();
      const_value_ptr token_end = std::find(source_begin, source_end, m_delim);

      m_has_more = source_end != token_end;

      m_token = token_type{source_begin, token_end};
      m_source.inc_begin(static_cast<size_type>(token_end - source_begin) + 1);

      return token();
    }

    [[nodiscard]]
    constexpr token_type token() const noexcept
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
      return !has_source() && !has_more();
    }

    [[nodiscard]]
    constexpr bool has_more() const noexcept
    {
      return m_has_more;
    }

    [[nodiscard]]
    constexpr bool has_source() const noexcept
    {
      return !m_source.empty();
    }

    [[nodiscard]]
    constexpr token_type source() const noexcept
    {
      return m_source;
    }

    [[nodiscard]]
    constexpr value_type delim() const noexcept
    {
      return m_delim;
    }

  private:
    token_type m_token{};
    size_type m_token_idx = none;
    token_type m_source{};
    value_type m_delim{};
    bool m_has_more = true;
};

template<>
class tokenizer<char>
{
  public:
    using traits = tokenizer_detail::traits<char>;
    using value_type = typename traits::value_type;
    using const_value_ptr = typename traits::const_value_ptr;
    using char_traits = std::char_traits<value_type>;
    using const_l_value_ref = typename traits::const_l_value_ref;
    using token_type = typename traits::token_type;
    using size_type = typename token_type::size_type;

    static constexpr size_type none{};

    constexpr explicit tokenizer(token_type p_source, const_l_value_ref p_delim) noexcept:
      m_source(p_source),
      m_delim(p_delim)
    {
    }

    constexpr tokenizer() noexcept = default;
    constexpr tokenizer(const tokenizer &) noexcept = default;
    constexpr tokenizer(tokenizer &&) noexcept = default;

    constexpr tokenizer & operator=(const tokenizer &) noexcept = default;
    constexpr tokenizer & operator=(tokenizer &&) noexcept = default;

    [[nodiscard]]
    constexpr token_type scan() noexcept
    {
      if(!empty())
      {
        ++m_token_idx;
      }

      const_value_ptr source_begin = m_source.begin();
      const_value_ptr pos = char_traits::find(source_begin, m_source.size(), m_delim);

      m_has_more = nullptr != pos;

      const_value_ptr token_end = m_source.end();
      if(m_has_more)
      {
        token_end = pos;
      }

      m_token = token_type{source_begin, token_end};
      m_source.inc_begin(static_cast<size_type>(token_end - source_begin) + 1);

      return token();
    }

    [[nodiscard]]
    constexpr token_type token() const noexcept
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
      return !has_source() && !has_more();
    }

    [[nodiscard]]
    constexpr bool has_more() const noexcept
    {
      return m_has_more;
    }

    [[nodiscard]]
    constexpr bool has_source() const noexcept
    {
      return !m_source.empty();
    }

    [[nodiscard]]
    constexpr token_type source() const noexcept
    {
      return m_source;
    }

    [[nodiscard]]
    constexpr value_type delim() const noexcept
    {
      return m_delim;
    }

  private:
    token_type m_token{};
    size_type m_token_idx = none;
    token_type m_source{};
    value_type m_delim{};
    bool m_has_more = true;
};

template<typename T>
class tokenizer_first:
      public tokenizer<T>
{
  public:
    using traits = typename tokenizer<T>::traits;
    using value_type = tokenizer<T>::value_type;
    using const_l_value_ref = tokenizer<T>::const_l_value_ref;
    using token_type = tokenizer<T>::token_type;
    using size_type = tokenizer<T>::size_type;

    constexpr explicit tokenizer_first(token_type p_source, const_l_value_ref p_delim) noexcept:
      tokenizer<T>(p_source, p_delim)
    {
      if(auto src = tokenizer<T>::source();
         !src.empty() && (tokenizer<T>::delim() == src[0]))
      {
        std::ignore = tokenizer<T>::scan();
      }
    }
};

} // namespace yafiyogi::yy_util
