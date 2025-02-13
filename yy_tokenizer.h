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

#pragma once

#include <string>

#include "yy_type_traits.h"
#include "yy_ref_traits.h"
#include "yy_span.h"

namespace yafiyogi::yy_util {
namespace tokenizer_detail {

template<typename T>
class traits final
{
  public:
    using value_type = yy_traits::remove_cvr_t<T>;
    using const_value_ptr = std::add_pointer_t<std::add_const_t<value_type>>;
    using const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using token_type = yy_quad::const_span<value_type>;
};

} // namespace tokenizer_detail

template<typename T,
         T t_delim,
         typename Enable = void>
class tokenizer
{
  public:
    using traits = tokenizer_detail::traits<T>;
    using value_type = typename traits::value_type;
    using const_value_ptr = typename traits::const_value_ptr;
    using const_l_value_ref = typename traits::const_l_value_ref;
    using token_type = typename traits::token_type;

    constexpr explicit tokenizer(token_type p_source) noexcept:
      m_source(p_source),
      m_token(p_source.begin(), p_source.begin())
    {
    }

    constexpr tokenizer() noexcept = default;
    constexpr tokenizer(const tokenizer &) noexcept = default;
    constexpr tokenizer(tokenizer &&) noexcept = default;

    constexpr tokenizer & operator=(const tokenizer &) noexcept = default;
    constexpr tokenizer & operator=(tokenizer &&) noexcept = default;

    [[nodiscard]]
    static constexpr token_type scan(const token_type p_source) noexcept
    {
      const auto source_begin{p_source.begin()};
      const auto source_end{p_source.end()};
      auto token_end{std::find(source_begin, source_end, t_delim)};

      if(source_end == token_end)
      {
        token_end = source_end;
      }

      return token_type{source_begin, token_end};
    }

    [[nodiscard]]
    constexpr token_type scan() noexcept
    {
      m_token = scan(m_source);
      m_source = token_type{m_token.end(), m_source.end()};
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

    static constexpr value_type delim = t_delim;

  private:
    token_type m_source{};
    token_type m_token{};
};

template<typename T,
         T t_delim>
class tokenizer<T,
                t_delim,
                std::enable_if_t<std::is_same_v<char, yy_traits::remove_cvr_t<T>>
                                 || std::is_same_v<char8_t, yy_traits::remove_cvr_t<T>>>>
{
  public:
    using traits = tokenizer_detail::traits<char>;
    using value_type = typename traits::value_type;
    using const_value_ptr = typename traits::const_value_ptr;
    using const_l_value_ref = typename traits::const_l_value_ref;
    using token_type = typename traits::token_type;
    using char_traits = std::char_traits<value_type>;

    constexpr explicit tokenizer(token_type p_source) noexcept:
      m_source(p_source),
      m_token(p_source.begin(), p_source.begin())
    {
    }

    constexpr tokenizer() noexcept = default;
    constexpr tokenizer(const tokenizer &) noexcept = default;
    constexpr tokenizer(tokenizer &&) noexcept = default;

    constexpr tokenizer & operator=(const tokenizer &) noexcept = default;
    constexpr tokenizer & operator=(tokenizer &&) noexcept = default;

    [[nodiscard]]
    static constexpr token_type scan(const token_type p_source) noexcept
    {
      token_type::iterator token_end{char_traits::find(p_source.data(), p_source.size(), t_delim)};

      if(not_found == token_end)
      {
        token_end = p_source.end();
      }

      return token_type{p_source.begin(), token_end};
    }

    [[nodiscard]]
    constexpr token_type scan() noexcept
    {
      m_token = scan(m_source);
      m_source = token_type{m_token.end(), m_source.end()};
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

    static constexpr value_type delim = t_delim;

  private:
    token_type m_source{};
    token_type m_token{};
    static constexpr const token_type::iterator not_found{};
};

template<typename T,
         T t_delim>
class tokenizer_first:
      public tokenizer<T, t_delim>
{
  public:
    using traits = tokenizer_detail::traits<char>;
    using value_type = typename traits::value_type;
    using const_l_value_ref = typename traits::const_l_value_ref;
    using token_type = typename traits::token_type;
    using base = tokenizer<T, t_delim>;

    constexpr explicit tokenizer_first(token_type p_source) noexcept:
      base(p_source)
    {
      if(auto src{base::source()};
         !src.empty() && (t_delim == src[0]))
      {
        // Remove delim at begining if present.
        std::ignore = base::scan();
      }
    }
};

} // namespace yafiyogi::yy_util
