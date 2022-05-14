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
# define yy_vector_util_h

#include <tuple>
#include <algorithm>

#include "yy_vector_traits.h"

namespace yafiyogi {
namespace detail {

template<typename V,
         typename O,
         typename C>
class LessThanComp
{
public:
  LessThanComp(C && comp):
    m_comp(comp)
  {
  }

  bool operator()(const O & o,
                  const V & v) const
  {
    return m_comp(o, v) < 0;
  }

private:
  C m_comp;
};

template<typename V,
         typename O,
         typename C>
class EqualToComp
{
public:
  EqualToComp(C && comp):
    m_comp(comp)
  {
  }

  bool operator()(const O & o,
                  const V & v) const
  {
    return 0 == m_comp(o, v);
  }

private:
  C m_comp;
};

template<typename T,
         typename Enable = void>
struct DefaultComp
{
  int operator()(const auto T & item,
                 const auto T & v)
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

template<typename T>
struct DefaultComp<T,
                   std::enable_if_t<yy_traits::is_std_string_v<std::decay_t<T> || yy_traits::is_string_view_v<std::decay_t<T>>, bool> = true>>
{
  int operator()(const auto T & item,
                 const auto T & v)
  {
    return item.compare(v);
  }
}

} // namespace detail

template<typename T,
         typename V,
         typename C = detail::DefaultComp,
         std::enable_if_t<yy_traits::is_vector_v<std::decay_t<T> || yy_traits::is_array_v<std::decay_t<T>>, bool> = true>>
auto Find(T && container, const V & value, C && comp = detail::Comp{})
{
  using value_type = yy_traits::container_type_t<std::decay_t<T>>;
  using less_than = detail::LessThanComp<V, value_type, C>;
  using equal_to = detail::EqualToComp<V, value_type, C>;

  auto iter = std::lower_bound( container.begin(),
                                container.end(),
                                value,
                                less_than{comp});
  bool found = (iter != container.end()) && equal_to{comp}(*iter, value);

  return std::make_tuple(std::move(iter), found);
}

template<typename T,
         typename V,
         typename C = detail::DefaultComp,
         std::enable_if_t<yy_traits::is_vector_v<std::decay_t<T> || yy_traits::is_array_v<std::decay_t<T>>, bool> = true>>
void Sort(T && container, const V & value, C && comp = detail::Comp{})
{
  using value_type = yy_traits::container_type_t<std::decay_t<T>>;
  using less_than = detail::LessThanComp<V, value_type, C>;

  auto iter = std::sort( container.begin(),
                         container.end(),
                         value,
                         less_than{comp});
}

} // namespace yafiyogi

#endif // yy_vector_util_h
