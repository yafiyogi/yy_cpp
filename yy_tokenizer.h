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

    constexpr explicit tokenizer(token_type p_source, const_l_value_ref p_delim) noexcept:
      m_source(p_source),
      m_token(p_source.begin(), p_source.begin()),
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
      auto source_begin{m_source.begin()};
      auto source_end{m_source.end()};
      auto token_end{std::find(source_begin, source_end, m_delim)};

      m_token = token_type{source_begin, token_end};
      m_source = token_type{token_end, source_end};
      m_source.inc_begin();

      return token();
    }

    [[nodiscard]]
    constexpr token_type token() const noexcept
    {
      return m_token;
    }

    [[nodiscard]]
    constexpr bool has_more() const noexcept
    {
      return m_token.end() != m_source.end();
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_source.empty();
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
    token_type m_source{};
    token_type m_token{};
    value_type m_delim{};
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

    constexpr explicit tokenizer(token_type p_source, const_l_value_ref p_delim) noexcept:
      m_source(p_source),
      m_token(p_source.begin(), p_source.begin()),
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
      token_type::iterator source_end{m_source.end()};
      token_type::iterator token_end{char_traits::find(m_source.data(), m_source.size(), m_delim)};

      if(not_found == token_end)
      {
        token_end = source_end;
      }

      m_token = token_type{m_source.begin(), token_end};
      m_source = token_type{token_end, source_end};
      m_source.inc_begin();

      return token();
    }

    [[nodiscard]]
    constexpr token_type token() const noexcept
    {
      return m_token;
    }

    [[nodiscard]]
    constexpr bool has_more() const noexcept
    {
      return m_token.end() != m_source.end();
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_source.empty();
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
    token_type m_source{};
    token_type m_token{};
    value_type m_delim{};
    static constexpr const token_type::iterator not_found{};
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
