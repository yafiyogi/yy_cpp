/*

  MIT License

  Copyright (c) 2021-2025 Yafiyogi

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

#include <type_traits>
#include "yy_type_traits.h"
#include "yy_types.hpp"

namespace yafiyogi::yy_util {

template<typename Iterator>
class Range final
{
  public:
    using iterator = yy_traits::remove_cvr_t<Iterator>;
    using const_iterator = std::add_const_t<iterator>;

    constexpr explicit Range(const iterator & p_begin,
                             const iterator & p_end) noexcept:
      m_begin(p_begin),
      m_end(p_end)
    {
    }

    constexpr explicit Range(iterator && p_begin,
                             iterator && p_end) noexcept:
      m_begin(std::move(p_begin)),
      m_end(std::move(p_end))
    {
    }

    Range() = delete;
    constexpr Range(const Range &) noexcept = default;
    constexpr Range(Range && p_other) noexcept:
      m_begin(std::move(p_other.m_begin)),
      m_end(std::move(p_other.m_end))
    {
      p_other.m_begin = Iterator{};
      p_other.m_end = Iterator{};
    }

    constexpr ~Range() noexcept = default;

    constexpr Range & operator=(const Range &) noexcept = default;
    constexpr Range & operator=(Range && p_other) noexcept
    {
      m_begin = std::move(p_other.m_begin);
      p_other.m_begin = Iterator{};
      m_end = std::move(p_other.m_end);
      p_other.m_end = Iterator{};
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept
    {
      return m_begin;
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    {
      return m_begin;
    }

    [[nodiscard]]
    constexpr iterator end() noexcept
    {
      return m_end;
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    {
      return m_end;
    }

    constexpr bool empty() noexcept
    {
      return m_begin == m_end;
    }

  private:
    iterator m_begin{};
    iterator m_end{};
};

template<typename Iterator>
constexpr auto make_range(const Iterator & begin, const Iterator & end) noexcept
{
  return Range<Iterator>{begin, end};
}

template<typename Iterator>
constexpr auto make_range(Iterator && begin, Iterator && end) noexcept
{
  return Range<Iterator>{std::forward<Iterator>(begin), std::forward<Iterator>(end)};
}

template<typename T,
         size_type N>
constexpr auto make_range(T (&& array)[N]) noexcept
{
  return Range<T *>{&array[0], &array[N]};
}

} // namespace yafiyogi::yy_util
