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

enum class ScanType { SkipBlank, All};

template<typename T>
struct traits final
{
  public:
    using value_type = yy_traits::remove_cvr_t<T>;
    using value_ptr = std::add_pointer_t<value_type>;
    using const_value_ptr = std::add_pointer_t<std::add_const_t<value_type>>;
    using const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using token_type = yy_quad::const_span<value_type>;
};

template<typename T,
         T t_delim,
         typename Enable = void>
struct default_scanner final
{
    using token_type = typename traits<T>::token_type;

    [[nodiscard]]
    static constexpr token_type scan(const token_type p_source) noexcept
    {
      const auto source_begin{p_source.begin()};
      auto token_end{std::find(source_begin, p_source.end(), t_delim)};

      return token_type{source_begin, token_end};
    }
};

template<typename T,
         T t_delim>
struct default_scanner<T,
                       t_delim,
                       std::enable_if_t<yy_traits::is_char_type<T>>> final
{
    using token_type = typename traits<T>::token_type;
    using const_value_ptr = typename traits<T>::const_value_ptr;
    using value_type = typename traits<T>::value_type;
    using char_traits = std::char_traits<value_type>;

    [[nodiscard]]
    static constexpr token_type scan(const token_type p_source) noexcept
    {
      const_value_ptr l_source_begin = p_source.data();
      size_type l_size = p_source.size();

      const_value_ptr token_end{char_traits::find(l_source_begin, l_size, t_delim)};
      if(nullptr == token_end)
      {
        token_end = l_source_begin + l_size;
      }

      return token_type{l_source_begin, token_end};
    }
};

template<typename T,
         T t_delim,
         ScanType t_scan_type,
         typename Scanner = default_scanner<T, t_delim>>
class tokenizer
{
  public:
    using tokenizer_traits = traits<T>;
    using value_type = typename tokenizer_traits::value_type;
    using const_value_ptr = typename tokenizer_traits::const_value_ptr;
    using const_l_value_ref = typename tokenizer_traits::const_l_value_ref;
    using token_type = typename tokenizer_traits::token_type;
    using scanner = Scanner;

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
    constexpr token_type scan() noexcept
    {
      do_scan();

      if constexpr (ScanType::SkipBlank == t_scan_type)
      {
        while(m_token.empty())
        {
          if(!has_more())
          {
            break;
          }

          do_scan();
        }
      }

      return m_token;
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
    void do_scan()
    {
      m_token = Scanner::scan(m_source);
      m_source = token_type{m_token.end(), m_source.end()};
      m_source.inc_begin();
    }

    token_type m_source{};
    token_type m_token{};
};

template<typename T,
         T t_delim,
         ScanType t_scan_type,
         typename Scanner = default_scanner<T, t_delim>,
         typename Base = tokenizer<T, t_delim, t_scan_type, Scanner>>
class tokenizer_first:
      public Base
{
  public:
    using base_traits = Base::tokenizer_traits;
    using value_type = typename base_traits::value_type;
    using const_l_value_ref = typename base_traits::const_l_value_ref;
    using token_type = typename base_traits::token_type;

    constexpr explicit tokenizer_first(token_type p_source) noexcept:
      Base(p_source)
    {
      if(auto src{Base::source()};
         !src.empty() && (t_delim == src[0]))
      {
        // Remove delim at begining if present.
        std::ignore = Base::scan();
      }
    }
};

} // namespace tokenizer_detail

template<typename T,
         T t_delim>
using tokenizer = tokenizer_detail::tokenizer<T, t_delim, tokenizer_detail::ScanType::All>;

template<typename T,
         T t_delim>
using tokenizer_first = tokenizer_detail::tokenizer_first<T, t_delim, tokenizer_detail::ScanType::All>;

} // namespace yafiyogi::yy_util
