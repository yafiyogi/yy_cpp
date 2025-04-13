/*

  MIT License

  Copyright (c) 2021-2024-2025 Yafiyogi

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

#include <algorithm>
#include <type_traits>

#include "yy_string_traits.h"

namespace yafiyogi::yy_util {
namespace compare_util_detail {

template<typename A,
         typename B,
         typename Enable = void>
struct is_string:
      std::false_type
{
};

template<typename A,
         typename B>
struct is_string<A, B,
                 typename std::enable_if_t<(yy_traits::is_std_string_v<A>
                                            || yy_traits::is_std_string_view_v<A>)
                                           && (yy_traits::is_std_string_v<B>
                                               || yy_traits::is_std_string_view_v<B>)>>:
      std::true_type
{
};

template<typename A,
         typename B>
inline constexpr bool is_string_v = is_string<A, B>::value;

template<typename A,
         typename B>
using is_string_t = typename is_string<A, B>::type;

} // namespace compare_util_detail

template<typename Lhs,
         typename Rhs,
         typename Enable = void>
struct Compare final
{
    using lhs_type = yy_traits::remove_cvr_t<Lhs>;
    using rhs_type = yy_traits::remove_cvr_t<Rhs>;

    static_assert(std::is_same_v<typename lhs_type::value_type, typename rhs_type::value_type>,
                  "Must be comparing the same value_type!");

    static constexpr bool equal(const lhs_type & lhs,
                                const rhs_type & rhs) noexcept
    {
      const auto lhs_size{lhs.size()};
      const auto lhs_begin{lhs.data()};

      return (lhs_size == rhs.size())
        && std::equal(lhs_begin, lhs_begin + lhs_size, rhs.data());
    }

    static constexpr bool less_than(const lhs_type & lhs,
                                    const rhs_type & rhs) noexcept
    {
      const auto lhs_size{lhs.size()};
      const auto lhs_begin{lhs.data()};
      const auto rhs_size{rhs.size()};
      const auto rhs_begin{rhs.data()};

      return std::lexicographical_compare(lhs_begin, lhs_begin + lhs_size,
                                          rhs_begin, rhs_begin + rhs_size);
    }
};

template<typename Lhs,
         typename Rhs>
struct Compare<Lhs, Rhs,
               std::enable_if_t<std::is_arithmetic_v<Lhs>
                                && std::is_arithmetic_v<Rhs>>> final
{
    using lhs_type = yy_traits::remove_cvr_t<Lhs>;
    using rhs_type = yy_traits::remove_cvr_t<Rhs>;

    static_assert(std::is_same_v<lhs_type, rhs_type>,
                  "Must be comparing the same value_type!");

    static constexpr bool equal(const lhs_type & a, const rhs_type & b) noexcept
    {
      return a == b;
    }

    static constexpr bool less_than(const lhs_type & a, const rhs_type & b) noexcept
    {
      return a < b;
    }
};

template<typename A,
         typename B>
struct Compare<A, B,
               std::enable_if_t<compare_util_detail::is_string_v<A, B>>> final
{
    using a_type = yy_traits::remove_cvr_t<A>;
    using b_type = yy_traits::remove_cvr_t<B>;

    static constexpr bool equal(const a_type & a, const b_type & b) noexcept
    {
      return 0 == a.compare(b);
    }

    static constexpr bool less_than(const a_type & a, const b_type & b) noexcept
    {
      return a.compare(b) < 0;
    }
};

template<typename A,
         typename B>
struct Compare<A, B,
               std::enable_if_t<!compare_util_detail::is_string_v<A, B>
                                && std::is_same_v<typename yy_traits::remove_cvr_t<A>::value_type,
                                                  typename yy_traits::remove_cvr_t<B>::value_type>
                                && (std::is_same_v<char, typename yy_traits::remove_cvr_t<A>::value_type>
                                    || std::is_same_v<char8_t, typename yy_traits::remove_cvr_t<A>::value_type>)>> final
{
    using a_type = yy_traits::remove_cvr_t<A>;
    using b_type = yy_traits::remove_cvr_t<B>;
    using char_type = yy_traits::remove_cvr_t<typename a_type::value_type>;

    static constexpr bool equal(const a_type & a, const b_type & b) noexcept
    {
      const auto a_size{a.size()};

      return (a_size == b.size())
        && (0 == std::char_traits<char_type>::compare(a.data(), b.data(), a_size));
    }

    static constexpr bool less_than(const a_type & a, const b_type & b) noexcept
    {
      const auto a_size{a.size()};
      const auto b_size{b.size()};
      const auto size{std::min(a_size, b_size)};

      auto comp{std::char_traits<char_type>::compare(a.data(), b.data(), size)};

      return (comp < 0) || ((0 == comp) && (a_size < b_size));
    }
};

template<typename A,
         typename B>
constexpr bool equal(const A & a, const B & b) noexcept
{
  return Compare<A, B>::equal(a, b);
}

template<typename A,
         typename B>
constexpr bool less_than(const A & a, const B & b) noexcept
{
  return Compare<A, B>::less_than(a, b);
}


} // namespace yafiyogi::yy_util
