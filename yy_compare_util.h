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

#include <string>
#include <algorithm>
#include <type_traits>

#include "yy_string_traits.h"

namespace yafiyogi::yy_util {

template<typename Lhs,
         typename Rhs,
         typename Enable = void>
struct Compare final
{
    using lhs_type = yy_traits::remove_cvr_t<Lhs>;
    using rhs_type = yy_traits::remove_cvr_t<Rhs>;

    static_assert(std::is_same_v<typename lhs_type::value_type, typename rhs_type::value_type>,
                  "Must be comparing the same value_type!");

    [[nodiscard]]
    static constexpr bool equal(const lhs_type & lhs,
                                const rhs_type & rhs) noexcept
    {
      return compare(lhs, rhs) == 0;
    }

    [[nodiscard]]
    static constexpr bool less_than(const lhs_type & lhs,
                                    const rhs_type & rhs) noexcept
    {
      return compare(lhs, rhs) < 0;
    }

    [[nodiscard]]
    static constexpr int compare(const lhs_type & lhs,
                                 const rhs_type & rhs) noexcept
    {
      const auto lhs_begin{lhs.data()};
      const auto lhs_end{lhs_begin + lhs.size()};
      const auto rhs_begin{rhs.data()};
      const auto rhs_end{rhs_begin + rhs.size()};

      auto [lhs_first, rhs_first] = std::mismatch(lhs_begin, lhs_end,
                                                  rhs_begin, rhs_end);

      auto lhs_rest = lhs_end - lhs_first;
      auto rhs_rest = rhs_end - rhs_first;

      if(0 == lhs_rest)
      {
        if(0 == rhs_rest)
        {
          return 0;
        }

        return -1;
      }

      if(0 == rhs_rest)
      {
        return 1;
      }

      if(*lhs_first < *rhs_first)
      {
        return -1;
      }

      return 1;
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

    [[nodiscard]]
    static constexpr bool equal(const lhs_type & lhs, const rhs_type & rhs) noexcept
    {
      return compare(lhs, rhs) == 0;
    }

    [[nodiscard]]
    static constexpr bool less_than(const lhs_type & lhs, const rhs_type & rhs) noexcept
    {
      return compare(lhs, rhs) < 0;
    }

    static constexpr lhs_type compare(const lhs_type & lhs, const rhs_type & rhs) noexcept
    {
      return lhs - rhs;
    }
};

template<typename Lhs,
         typename Rhs>
struct Compare<Lhs, Rhs,
               std::enable_if_t<std::is_same_v<typename yy_traits::remove_cvr_t<Lhs>::value_type,
                                               typename yy_traits::remove_cvr_t<Rhs>::value_type>
                                && std::is_same_v<char, typename yy_traits::remove_cvr_t<Lhs>::value_type>>> final
{
    using lhs_type = yy_traits::remove_cvr_t<Lhs>;
    using rhs_type = yy_traits::remove_cvr_t<Rhs>;
    using char_type = yy_traits::remove_cvr_t<typename lhs_type::value_type>;

    [[nodiscard]]
    static constexpr bool equal(const lhs_type & lhs, const rhs_type & rhs) noexcept
    {
      return compare(lhs, rhs) == 0;
    }

    [[nodiscard]]
    static constexpr bool less_than(const lhs_type & lhs,
                                    const rhs_type & rhs) noexcept
    {
      return compare(lhs, rhs) < 0;
    }

    [[nodiscard]]
    static constexpr int compare(const lhs_type & lhs,
                                 const rhs_type & rhs) noexcept
    {
      const auto lhs_size{lhs.size()};
      const auto rhs_size{rhs.size()};
      const auto size{std::min(lhs_size, rhs_size)};

      if(int comp = std::char_traits<char_type>::compare(lhs.data(), rhs.data(), size);
         comp != 0)
      {
        return comp;
      }

      if(lhs_size == rhs_size)
      {
        return 0;
      }

      if(lhs_size < rhs_size)
      {
        return -1;
      }

      return 1;
    }
};

template<typename Lhs,
         typename Rhs>
[[nodiscard]]
constexpr bool equal(const Lhs & lhs, const Rhs & rhs) noexcept
{
  return Compare<Lhs, Rhs>::equal(lhs, rhs);
}

template<typename Lhs,
         typename Rhs>
[[nodiscard]]
constexpr bool less_than(const Lhs & lhs, const Rhs & rhs) noexcept
{
  return Compare<Lhs, Rhs>::less_than(lhs, rhs);
}

template<typename Lhs,
         typename Rhs>
[[nodiscard]]
constexpr int compare(const Lhs & lhs, const Rhs & rhs) noexcept
{
  return Compare<Lhs, Rhs>::compare(lhs, rhs);
}


} // namespace yafiyogi::yy_util
