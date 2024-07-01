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

#ifndef yy_vector_util_h
#define yy_vector_util_h

#include <algorithm>
#include <tuple>

#include "yy_array_traits.h"
#include "yy_string_traits.h"
#include "yy_vector_traits.h"

namespace yafiyogi::yy_util {
namespace vector_detail {

template<typename T,
         typename V,
         typename C>
class less_than_comp final
{
  public:
    using container_value_type = yy_traits::remove_rcv_t<T>;
    using value_type = yy_traits::remove_rcv_t<V>;

    constexpr explicit less_than_comp(C comp) noexcept:
      m_comp(std::move(comp))
    {
    }

    less_than_comp() = delete;
    constexpr less_than_comp(const less_than_comp&) noexcept = default;
    constexpr less_than_comp(less_than_comp &&) noexcept = default;
    constexpr ~less_than_comp() noexcept = default;

    less_than_comp & operator=(const less_than_comp&) = delete;
    less_than_comp & operator=(less_than_comp &&) = delete;

    constexpr bool operator()(const container_value_type & cval,
                              const value_type & val) const noexcept
    {
      return m_comp(cval, val) < 0;
    }

  private:
    C m_comp;
};

template<typename T,
         typename V,
         typename C>
class equal_to_comp final
{
  public:
    using container_value_type = yy_traits::remove_rcv_t<T>;
    using value_type = yy_traits::remove_rcv_t<V>;

    constexpr explicit equal_to_comp(C comp) noexcept:
      m_comp(std::move(comp))
    {
    }

    equal_to_comp() = delete;
    constexpr equal_to_comp(const equal_to_comp &) noexcept = default;
    constexpr equal_to_comp(equal_to_comp &&) noexcept = default;
    constexpr ~equal_to_comp() noexcept = default;

    equal_to_comp & operator=(const equal_to_comp&) = delete;
    equal_to_comp & operator=(equal_to_comp &&) noexcept = delete;

    constexpr bool operator()(const container_value_type & cval,
                              const value_type & val) const noexcept
    {
      return 0 == m_comp(cval, val);
    }

  private:
    C m_comp;
};

template<typename T,
         typename V,
         typename Enable = void>
struct default_comp final
{
    using container_value_type = yy_traits::remove_rcv_t<T>;
    using value_type = yy_traits::remove_rcv_t<V>;

    constexpr int operator()(const container_value_type & item,
                             const value_type & val) const noexcept
    {
      if(val < item)
      {
        return 1;
      }

      if(val == item)
      {
        return 0;
      }

      return -1;
    };
};

template<typename T,
         typename V>
struct default_comp<T,
                    V,
                    std::enable_if_t<(yy_traits::is_std_string_v<T>
                                      || yy_traits::is_std_string_view_v<T>)
                                     &&(yy_traits::is_std_string_v<V>
                                        || yy_traits::is_std_string_view_v<V>)>> final
{
    constexpr int operator()(const T & item, const V & val) const noexcept
    {
      return item.compare(val);
    }
};

} // namespace vector_detail

struct pos_end_type final
{
    std::size_t pos{};
    bool is_end = false;
};

template<typename T,
         typename V,
         typename C = vector_detail::default_comp<yy_traits::container_type_t<T>,
                                                  yy_traits::remove_rcv_t<V>>,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_array_v<T>,
                          bool> = true>
constexpr pos_end_type lower_bound_pos(T && container,
                                       V && value,
                                       C && comp = C{}) noexcept
{
  using container_value_type = yy_traits::container_type_t<T>;
  using value_type = yy_traits::remove_rcv_t<V>;
  using less_than = vector_detail::less_than_comp<container_value_type, value_type, C>;

  const auto begin = container.data();
  const auto end = begin + container.size();

  auto iter = std::lower_bound(begin,
                               end,
                               value,
                               less_than{comp});
  bool is_end = end == iter;

  auto pos = std::distance(begin, iter);

  return pos_end_type{static_cast<std::size_t>(pos), is_end};
}

struct pos_found_type final
{
    std::size_t pos{};
    bool found = false;
};

template<typename T,
         typename V,
         typename C = vector_detail::default_comp<yy_traits::container_type_t<T>,
                                                  yy_traits::remove_rcv_t<V>>,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_array_v<T>,
                          bool> = true>
constexpr pos_found_type find_pos(T && container,
                                  V && value,
                                  C && comp = C{}) noexcept
{
  using container_type = yy_traits::remove_rcv_t<T>;
  using container_value_type = yy_traits::container_type_t<container_type>;
  using value_type = yy_traits::remove_rcv_t<V>;
  using equal_to = vector_detail::equal_to_comp<container_value_type, value_type, C>;

  auto [pos, is_end] = lower_bound_pos(container, value, std::forward<C>(comp));

  const auto begin = container.data();
  const auto iter = begin + pos;

  bool found = !is_end && equal_to{comp}(*iter, value);

  return pos_found_type{static_cast<std::size_t>(pos), found};
}

template<typename T,
         typename V,
         typename C = vector_detail::default_comp<yy_traits::container_type_t<T>,
                                                  yy_traits::remove_rcv_t<V>>,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_array_v<T>,
                          bool> = true>
constexpr auto lower_bound(T && container,
                           V && value,
                           C && comp = C{}) noexcept
{
  auto [pos, is_end] = lower_bound_pos(container, value, std::forward<C>(comp));

  return std::make_tuple(container.begin() + static_cast<std::ptrdiff_t>(pos), is_end);
}

template<typename T,
         typename V,
         typename C = vector_detail::default_comp<yy_traits::container_type_t<T>,
                                                  yy_traits::remove_rcv_t<V>>,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_array_v<T>,
                          bool> = true>
constexpr auto find(T && container,
                    V && value,
                    C && comp = C{}) noexcept
{
  auto [pos, found] = find_pos(container, value, std::forward<C>(comp));

  return std::make_tuple(container.begin() + static_cast<std::ptrdiff_t>(pos), found);
}

template<typename T,
         typename C = vector_detail::default_comp<yy_traits::container_type_t<T>,
                                                  yy_traits::container_type_t<T>>,
         std::enable_if_t<yafiyogi::yy_traits::is_vector_v<T>
                          || yafiyogi::yy_traits::is_array_v<T>, bool> = true>
constexpr T sort(T && container,
                    C && comp = C{})
{
  using container_value_type = yy_traits::container_type_t<T>;
  using less_than =
    vector_detail::less_than_comp<container_value_type, container_value_type, C>;

  std::sort(container.data(), container.data() + container.size(), less_than{comp});

  return container;
}

template<typename T,
         typename C = vector_detail::default_comp<yy_traits::container_type_t<T>,
                                                  yy_traits::container_type_t<T>>,
         std::enable_if_t<yafiyogi::yy_traits::is_vector_v<T>, bool> = true>
constexpr void unique(T & container,
                      C && comp = C{})
{
  using container_value_type = yy_traits::container_type_t<T>;
  using equal_to =
    vector_detail::equal_to_comp<container_value_type, container_value_type, C>;

  container.erase(std::unique(container.begin(),
                              container.end(),
                              equal_to{comp}),
                  container.end());
}

template<typename T>
constexpr void shrink(T & container)
{
  T{container}.swap(container);
}

template<typename T>
constexpr void shrink(const T & src, T & dst)
{
  T{src}.swap(dst);
}

} // namespace yafiyogi::yy_util

#endif // yy_vector_util_h
