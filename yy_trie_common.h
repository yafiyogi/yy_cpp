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

#include "yy_ref_traits.h"
#include "yy_span.h"
#include "yy_tokenizer.h"

namespace yafiyogi::yy_trie {

template<typename LabelType>
struct label_traits final
{
    using label_type = LabelType;
    using ref_traits = yy_traits::ref_traits<label_type>;
    using label_l_value_ref = typename ref_traits::l_value_ref;
    using label_const_l_value_ref = typename ref_traits::const_l_value_ref;
    using label_r_value_ref = typename ref_traits::r_value_ref;
};

template<typename ValueType>
struct value_traits final
{
    using value_type = yy_traits::remove_cvr_t<ValueType>;
    using value_ptr = std::add_pointer_t<value_type>;
    using const_value_ptr = std::add_pointer_t<std::add_const_t<value_type>>;
};

template<typename LabelType>
class default_label_tokenizer final
{
  public:
    using traits = label_traits<LabelType>;
    using label_type = typename traits::label_type;
    using label_span_type = yy_quad::const_span<label_type>;
    using source_type = label_span_type;
    using token_type = label_type;

    constexpr explicit default_label_tokenizer(source_type p_source) noexcept:
      m_source(yy_quad::make_const_span(p_source))
    {
    }

    [[nodiscard]]
    constexpr token_type scan() noexcept
    {
      if(!m_source.empty())
      {
        m_token = *m_source.begin();
        m_source.inc_begin();
      }
      m_has_more = !m_source.empty();

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
      return m_has_more;
    }

    [[nodiscard]]
    constexpr source_type source() const noexcept
    {
      return m_source;
    }

    [[nodiscard]]
    constexpr bool has_source() const noexcept
    {
      return !m_source.empty();
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return !has_source() && !has_more();
    }

    static constexpr label_type create(token_type & token) noexcept
    {
      return token;
    }

  private:
    source_type m_source{};
    token_type m_token{};
    bool m_has_more = true;
};

template<typename LabelType,
         typename LabelType::value_type t_delim,
         template<typename L> class Tokenizer = yy_util::tokenizer>
class label_word_tokenizer final
{
  public:
    using traits = label_traits<LabelType>;
    using label_type = typename traits::label_type;
    using tokenizer_type = Tokenizer<decltype(t_delim)>;
    using label_span_type = typename tokenizer_type::token_type;
    using source_type = label_span_type;
    using token_type = label_span_type;

    constexpr explicit label_word_tokenizer(source_type p_source) noexcept:
      m_tokenizer(p_source, t_delim)
    {
    }

    [[nodiscard]]
    constexpr token_type scan() noexcept
    {
      return m_tokenizer.scan();
    }

    [[nodiscard]]
    constexpr token_type token() const noexcept
    {
      return m_tokenizer.token();
    }

    [[nodiscard]]
    constexpr bool has_more() const noexcept
    {
      return m_tokenizer.has_more();
    }

    [[nodiscard]]
    constexpr source_type source() const noexcept
    {
      return m_tokenizer.source();
    }

    [[nodiscard]]
    constexpr bool has_source() const noexcept
    {
      return m_tokenizer.has_source();
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_tokenizer.empty();
    }

    static constexpr label_type create(token_type & token) noexcept
    {
      return label_type{token.begin(), token.end()};
    }

  private:
    tokenizer_type m_tokenizer{};
};

} // namespace yafiyogi::yy_trie
