/*

  MIT License

  Copyright (c) 2021-2024 Yafiyogi

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

#include <cstddef>

#include <array>
#include <utility>
#include <memory>
#include <type_traits>

#include "yy_assert.h"
#include "yy_type_traits.h"

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
    constexpr Range(Range &&) noexcept = default;
    constexpr ~Range() noexcept = default;

    constexpr Range & operator=(const Range &) noexcept = default;
    constexpr Range & operator=(Range &&) noexcept = default;

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
         std::size_t N>
constexpr auto make_range(T (&& array)[N]) noexcept
{
  return Range<T *>{&array[0], &array[N]};
}

template<typename T>
struct ArraySize
{
    static constexpr std::size_t size = 0;
};

template<typename T, std::size_t Size>
struct ArraySize<T[Size]>
{
    static constexpr std::size_t size = Size;
};

template<typename T, std::size_t Size>
struct ArraySize<std::array<T, Size>>
{
    static constexpr std::size_t size = Size;
};

template<typename T>
inline constexpr std::size_t array_size_v = ArraySize<T>::size;

template<typename Return,
         typename T>
constexpr std::unique_ptr<yy_traits::remove_cvr_t<Return>> static_unique_cast(std::unique_ptr<T> && ptr)
{
  using return_type = yy_traits::remove_cvr_t<Return>;
  using param_type = yy_traits::remove_cvr_t<T>;
  static_assert(std::is_base_of_v<return_type, param_type>, "static_unique_cast(): type 'Return' is not a base of type 'T'.");

  return std::unique_ptr<return_type>{static_cast<return_type *>(ptr.release())};
}

template<typename ...Types>
constexpr auto make_array(Types && ... data)
{
  using type = std::common_type<Types...>::type;
  return std::array<type, sizeof...(Types)>{std::forward<Types>(data)...};
}

} // namespace yafiyogi::yy_util
