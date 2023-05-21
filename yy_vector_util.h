/*

  MIT License

  Copyright (c) 2021-2022 Yafiyogi

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

#include "yy_string_traits.h"
#include "yy_vector_traits.h"

namespace yafiyogi::yy_util {
namespace detail {

template<typename T, typename V, typename C>
class less_than_comp final
{
  public:
    using container_value_type = yy_traits::remove_rcv_t<T>;
    using value_type = yy_traits::remove_rcv_t<V>;

    less_than_comp(C comp) :
      m_comp(std::move(comp))
    {
    }

    bool operator()(const container_value_type & cv, const value_type & v) const
    {
      return m_comp(cv, v) < 0;
    }

  private:
    const C m_comp;
};

template<typename T, typename V, typename C>
class equal_to_comp final
{
  public:
    using container_value_type = yy_traits::remove_rcv_t<T>;
    using value_type = yy_traits::remove_rcv_t<V>;

    equal_to_comp(C comp) :
      m_comp(std::move(comp))
    {
    }

    bool operator()(const container_value_type & cv, const value_type & v) const
    {
      return 0 == m_comp(cv, v);
    }

  private:
    const C m_comp;
};

template<typename T, typename V, typename Enable = void>
struct default_comp final
{
    using container_value_type = yy_traits::remove_rcv_t<T>;
    using value_type = yy_traits::remove_rcv_t<V>;

    int operator()(const container_value_type & item,
                   const value_type & v) const
    {
      int rv = 1;

      if(v == item)
      {
        rv = 0;
      }
      else if(v < item)
      {
        rv = -1;
      }

      return rv;
    };
};

template<typename T, typename V>
struct default_comp<
  T,
  V,
  typename std::enable_if_t<
    (yy_traits::is_std_string_v<T> || yy_traits::is_std_string_view_v<T>) &&(
      yy_traits::is_std_string_v<V> || yy_traits::is_std_string_view_v<V>)>> final
{
    int operator()(const T & item, const V & v) const
    {
      return item.compare(v);
    }
};

} // namespace detail

template<typename T,
         typename V,
         typename C = detail::default_comp<yy_traits::container_type_t<T>,
                                           yy_traits::remove_rcv_t<V>>,
         std::enable_if_t<yafiyogi::yy_traits::is_vector_v<
                            T> || yafiyogi::yy_traits::is_array_v<T>,
                          bool> = true>
auto find(T && container, V && value, C && comp = C{})
{
  using container_value_type = yy_traits::container_type_t<T>;
  using value_type = yy_traits::remove_rcv_t<V>;
  using less_than = detail::less_than_comp<container_value_type, value_type, C>;
  using equal_to = detail::equal_to_comp<container_value_type, value_type, C>;

  auto iter = std::lower_bound(container.begin(),
                               container.end(),
                               value,
                               less_than{comp});
  bool found = (iter != container.end()) && equal_to{comp}(*iter, value);

  return std::make_tuple(std::move(iter), found);
}

template<typename T,
         typename C = detail::default_comp<yy_traits::container_type_t<T>,
                                           yy_traits::container_type_t<T>>,
         std::enable_if_t<yafiyogi::yy_traits::is_vector_v<
                            T> || yafiyogi::yy_traits::is_array_v<T>,
                          bool> = true>
void sort(T & container, C && comp = C{})
{
  using container_value_type = yy_traits::container_type_t<T>;
  using less_than =
    detail::less_than_comp<container_value_type, container_value_type, C>;

  std::sort(container.begin(), container.end(), less_than{comp});
}

template<typename T,
         typename C = detail::default_comp<yy_traits::container_type_t<T>,
                                           yy_traits::container_type_t<T>>,
         std::enable_if_t<yafiyogi::yy_traits::is_vector_v<T>, bool> = true>
void unique(T & container, C && comp = C{})
{
  using container_value_type = yy_traits::container_type_t<T>;
  using equal_to =
    detail::equal_to_comp<container_value_type, container_value_type, C>;

  container.erase(std::unique(container.begin(),
                              container.end(),
                              equal_to{comp}),
                  container.end());
}

template<typename T>
void shrink(T & container)
{
  T{container}.swap(container);
}

template<typename T>
void shrink(const T & src, T & dst)
{
  T{src}.swap(dst);
}

} // namespace yafiyogi::yy_util

#endif // yy_vector_util_h
