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

#include <cstddef>

#include <algorithm>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include "yy_compare_util.h"
#include "yy_constants.hpp"
#include "yy_find_util.hpp"
#include "yy_ref_traits.h"
#include "yy_traits_array.hpp"
#include "yy_traits_span.hpp"
#include "yy_traits_static_vector.hpp"
#include "yy_traits_string.hpp"
#include "yy_traits_vector.hpp"

#include "yy_iterator_ptr.hpp"

namespace yafiyogi::yy_quad {
namespace span_detail {

template<typename ValueType>
struct span_traits final
{
    using value_type = yy_traits::remove_cvr_t<ValueType>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using ptr = std::add_pointer_t<value_type>;
    using const_ptr = std::add_pointer_t<std::add_const_t<value_type>>;
};

} // namespace span_detail

template<typename ValueType>
class span final
{
  public:
    using traits = typename span_detail::span_traits<ValueType>;
    using value_type = typename traits::value_type;
    using value_l_value_ref = typename traits::value_l_value_ref;
    using value_const_l_value_ref = typename traits::value_const_l_value_ref;
    using value_r_value_ref = typename traits::value_r_value_ref;
    using ptr = typename traits::ptr;
    using const_ptr = typename traits::const_ptr;
    using iterator = yy_data::iterator_detail::iterator_ptr<span>;
    using const_iterator = yy_data::iterator_detail::const_iterator_ptr<span>;

    template<typename T,
             std::enable_if_t<yy_traits::is_container_v<T>
                              && std::is_same_v<value_type, yy_traits::remove_cvr_t<typename T::value_type>>>>
    constexpr explicit span(const T & container) noexcept:
      m_begin(container.data()),
      m_size(container.size())
    {
    }

    constexpr explicit span(ptr p_begin,
                            ptr p_end) noexcept:
      m_begin(p_begin),
      m_size(staic_cast<size_type>(p_end - p_begin))
    {
    }

    constexpr explicit span(ptr p_begin,
                            size_type p_size) noexcept:
      m_begin(p_begin),
      m_size(p_size)
    {
    }

    constexpr explicit span(iterator p_begin,
                            iterator p_end) noexcept:
      m_begin(p_begin.ptr()),
      m_size(static_cast<size_type>(p_end.ptr() - p_begin.ptr()))
    {
    }

    constexpr explicit span(iterator p_begin,
                            size_type p_size) noexcept:
      m_begin(p_begin.ptr),
      m_size(p_size)
    {
    }

    constexpr span() noexcept = default;
    constexpr span(const span &) noexcept = default;

    constexpr ~span() noexcept = default;

    constexpr span & operator=(const span &) noexcept = default;

    [[nodiscard]]
    constexpr value_type & operator[](size_type idx) noexcept
    {
      idx = std::min(idx, m_size);
      return *(m_begin + idx);
    }

    [[nodiscard]]
    constexpr const value_type & operator[](size_type idx) const noexcept
    {
      idx = std::min(idx, m_size);
      return *(m_begin + idx);
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept
    {
      return iterator{this, 0};
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    {
      return const_iterator{this, 0};
    }

    [[nodiscard]]
    constexpr iterator end() noexcept
    {
      return iterator{this, m_size};
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    {
      return const_iterator{this, m_size};
    }

    constexpr span & inc_begin() noexcept
    {
      if(size_type{0} != m_size)
      {
        ++m_begin;
        --m_size;
      }
      return *this;
    }

    constexpr span & inc_begin(size_type step) noexcept
    {
      step = std::min(step, m_size);
      m_begin += step;
      m_size -= step;

      return *this;
    }

    constexpr span & inc_begin(int step) noexcept
    {
      if(step > 0)
      {
        size_type l_step = std::min(static_cast<size_t>(step), m_size);

        m_begin += l_step;
        m_size -= l_step;
      }

      return *this;
    }

    constexpr span & dec_end() noexcept
    {
      if(size_type{0} != m_size)
      {
        --m_size;
      }
      return *this;
    }

    constexpr span & dec_end(size_type step) noexcept
    {
      m_size -= std::min(step, m_size);

      return *this;
    }

    constexpr span & dec_end(int step) noexcept
    {
      if(step > 0)
      {
        m_size -= std::min(static_cast<size_type>(step), m_size);
      }

      return *this;
    }

    [[nodiscard]]
    constexpr ptr data() noexcept
    {
      return m_begin;
    }

    [[nodiscard]]
    constexpr const_ptr data() const noexcept
    {
      return m_begin;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return size_type{0} == m_size;
    }

    [[nodiscard]]
    constexpr value_type & front()
    {
      if(size_type{0} == m_size)
      {
        throw std::out_of_range("span<T>::front(): span is empty!");
      }

      return *m_begin;
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref front() const
    {
      if(size_type{0} == m_size)
      {
        throw std::out_of_range("span<T>::front(): span is empty!");
      }

      return *m_begin;
    }

    [[nodiscard]]
    constexpr value_type & back()
    {
      if(size_type{0} == m_size)
      {
        throw std::out_of_range("span<T>::back(): span is empty!");
      }

      return *(begin() + (m_size - 1));
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref back() const
    {
      if(size_type{0} == m_size)
      {
        throw std::out_of_range("span<T>::back(): span is empty!");
      }

      return *(begin() + (m_size - 1));
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      return m_size;
    }

    [[nodiscard]]
    constexpr span subspan(size_type pos,
                           size_type len = npos) const noexcept
    {
      const size_type l_size = m_size;
      if(npos == len)
      {
        len = 0;
        if(pos < l_size)
        {
          len = l_size - pos;
        }
      }
      else if(pos < l_size)
      {
        if((pos + len) > l_size)
        {
          len = l_size - pos;
        }
      }
      else
      {
        pos = 0;
        len = 0;
      }

      return span{m_begin + pos, len};
    }

    constexpr size_type find_pos(const value_type & p_value) const noexcept
    {
      return yy_data::find_pos_linear(m_begin, m_size, p_value).pos;
    }

    [[nodiscard]]
    constexpr bool operator<(const span & other) const noexcept
    {
      return yy_util::less_than(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator<(const type & a,
                                    const span b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(a, b);
    }

    [[nodiscard]]
    constexpr bool operator==(const span & other) const noexcept
    {
      return yy_util::equal(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator==(const type & a,
                                     const span b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(a, b);
    }

  private:
    ptr m_begin = nullptr;
    size_type m_size = 0;
};

template<typename ValueType>
class const_span final
{
  public:
    using traits = typename span_detail::span_traits<ValueType>;
    using value_type = typename traits::value_type;
    using value_l_value_ref = typename traits::value_l_value_ref;
    using value_const_l_value_ref = typename traits::value_const_l_value_ref;
    using value_r_value_ref = typename traits::value_r_value_ref;
    using ptr = typename traits::const_ptr;
    using iterator = yy_data::iterator_detail::const_iterator_ptr<const_span>;

    template<typename T,
             std::enable_if_t<yy_traits::is_container_v<T>
                              && std::is_same_v<value_type, yy_traits::remove_cvr_t<typename T::value_type>>>>
    constexpr explicit const_span(const T & container) noexcept:
      m_begin(container.data()),
      m_size(container.size())
    {
    }

    constexpr explicit const_span(ptr p_begin,
                                  ptr p_end) noexcept:
      m_begin(p_begin),
      m_size(static_cast<size_type>(p_end - p_begin))
    {
    }

    constexpr explicit const_span(ptr p_begin,
                                  const size_type p_size) noexcept:
      m_begin(p_begin),
      m_size(p_size)
    {
    }

    constexpr explicit const_span(iterator p_begin,
                                  iterator p_end) noexcept:
      m_begin(p_begin.ptr()),
      m_size(static_cast<size_type>(p_end.ptr() - p_begin.ptr()))
    {
    }

    constexpr explicit const_span(iterator p_begin,
                                  const size_type p_size) noexcept:
      m_begin(p_begin.ptr()),
      m_size(p_size)
    {
    }

    constexpr explicit const_span(yy_data::iterator_detail::iterator_ptr<const_span> p_begin,
                                  yy_data::iterator_detail::iterator_ptr<const_span> p_end) noexcept:
      m_begin(p_begin.ptr()),
      m_size(static_cast<size_type>(p_end.ptr() - p_begin.ptr()))
    {
    }

    constexpr explicit const_span(yy_data::iterator_detail::iterator_ptr<const_span> p_begin,
                                  const size_type p_size) noexcept:
      m_begin(p_begin.ptr()),
      m_size(p_size)
    {
    }

    constexpr explicit const_span(span<value_type> span) noexcept:
      m_begin(span.m_begin),
      m_size(span.m_size)
    {
    }

    constexpr const_span() noexcept = default;
    constexpr const_span(const const_span &) noexcept = default;

    constexpr ~const_span() noexcept = default;

    constexpr const_span & operator=(const const_span &) noexcept = default;

    [[nodiscard]]
    constexpr const value_type & operator[](size_type idx) const noexcept
    {
      idx = std::min(idx, m_size);
      return *(m_begin + idx);
    }

    [[nodiscard]]
    constexpr iterator begin() const noexcept
    {
      return iterator{this, 0};
    }

    [[nodiscard]]
    constexpr iterator end() const noexcept
    {
      return iterator{this, m_size};
    }

    constexpr const_span & inc_begin() noexcept
    {
      if(size_type{0} != m_size)
      {
        ++m_begin;
        --m_size;
      }
      return *this;
    }

    constexpr const_span & inc_begin(size_type step) noexcept
    {
      step = std::min(step, m_size);
      m_begin += step;
      m_size -= step;

      return *this;
    }

    constexpr const_span & inc_begin(int step) noexcept
    {
      if(step > 0)
      {
        size_type l_step = std::min(static_cast<size_type>(step), size());
        m_begin += l_step;
        m_size -= l_step;
      }

      return *this;
    }

    constexpr const_span & dec_end() noexcept
    {
      if(size_type{0} != m_size)
      {
        --m_size;
      }
      return *this;
    }

    constexpr const_span & dec_end(size_type step) noexcept
    {
      m_size -= std::min(step, size());

      return *this;
    }

    constexpr const_span & dec_end(int step) noexcept
    {
      if(step > 0)
      {
        m_size -= std::min(static_cast<size_type>(step), m_size);
      }

      return *this;
    }

    [[nodiscard]]
    constexpr ptr data() const noexcept
    {
      return m_begin;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return size_type{0} == m_size;
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref front() const
    {
      if(size_type{0} == m_size)
      {
        throw std::out_of_range("const_span<T>::front(): span is empty!");
      }

      return *m_begin;
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref back() const
    {
      if(size_type{0} == m_size)
      {
        throw std::out_of_range("const_span<T>::back(): span is empty!");
      }

      return *(begin() + (m_size - 1));
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      return m_size;
    }

    [[nodiscard]]
    constexpr const_span subspan(size_type pos,
                                 size_type len = npos) const noexcept
    {
      const size_type l_size = m_size;
      if(npos == len)
      {
        len = 0;
        if(pos < l_size)
        {
          len = l_size - pos;
        }
      }
      else if(pos < l_size)
      {
        if((pos + len) > l_size)
        {
          len = l_size - pos;
        }
      }
      else
      {
        pos = 0;
        len = 0;
      }

      return const_span{m_begin + pos, len};
    }

    constexpr size_type find_pos(const value_type & p_value) const noexcept
    {
      return yy_data::find_pos_linear(m_begin, m_size, p_value).pos;
    }

    [[nodiscard]]
    constexpr bool operator<(const const_span & other) const noexcept
    {
      return yy_util::less_than(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator<(const type & a,
                                    const const_span b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(a, b);
    }

    [[nodiscard]]
    constexpr bool operator==(const const_span & other) const noexcept
    {
      return yy_util::equal(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator==(const type & a,
                                     const const_span b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(a, b);
    }

    [[nodiscard]]
    constexpr bool operator<(const span<value_type> other) const noexcept
    {
      return yy_util::less_than(*this, other);
    }

    [[nodiscard]]
    friend constexpr bool operator<(const span<value_type> a,
                                    const const_span b) noexcept
    {
      return yy_util::less_than(a, b);
    }

    [[nodiscard]]
    constexpr bool operator==(const span<value_type> other) const noexcept
    {
      return yy_util::equal(*this, other);
    }

    [[nodiscard]]
    friend constexpr bool operator==(const span<value_type> a,
                                     const const_span b) noexcept
    {
      return yy_util::equal(a, b);
    }

  private:
    mutable ptr m_begin = nullptr;
    mutable size_type m_size = 0;
};


template<typename T,
         typename Enable = void>
struct span_traits_helper final
{
    using traits = typename span_detail::span_traits<T>;
    using value_type = typename traits::value_type;
    using span_type = span<value_type>;
    using const_span_type = const_span<value_type>;
};

template<typename T>
struct span_traits_helper<T,
                          std::enable_if_t<yy_traits::is_vector_v<T>
                                           || yy_traits::is_std_string_v<T>
                                           || yy_traits::is_array_v<T>>> final
{
    using traits = typename span_detail::span_traits<typename T::value_type>;
    using value_type = typename traits::value_type;
    using span_type = span<value_type>;
    using const_span_type = const_span<value_type>;
};

template<typename T>
struct span_traits_helper<T,
                          std::enable_if_t<yy_traits::is_std_string_view_v<T>>> final
{
    using traits = typename span_detail::span_traits<typename T::value_type>;
    using value_type = typename traits::value_type;
    using span_type = const_span<value_type>;
    using const_span_type = const_span<value_type>;
};

template<typename T>
struct span_traits_helper<T,
                          std::enable_if_t<std::is_same_v<char, yy_traits::remove_cvr_t<T>>>> final
{
    using traits = typename span_detail::span_traits<char>;
    using value_type = typename traits::value_type;
    using span_type = span<value_type>;
    using const_span_type = const_span<value_type>;
};

template<typename T>
struct span_traits_helper<T,
                          std::enable_if_t<yy_traits::is_c_string_v<T>>> final
{
    using traits = typename span_detail::span_traits<std::remove_pointer_t<std::decay_t<T>>>;
    using value_type = typename traits::value_type;
    using span_type = const_span<value_type>;
    using const_span_type = const_span<value_type>;
};

template<typename T,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_std_string_v<T>
                          || yy_traits::is_array_v<T>, bool> = true>
constexpr auto make_span(T & container)
{
  return typename span_traits_helper<T>::span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_std_string_view_v<T>, bool> = true>
constexpr auto make_span(T p_sv)
{
  static_assert(!yy_traits::is_std_string_view_v<T>, "Can't create a 'span<>' from std::string_view!");
  //return typename span_traits_helper<T>::const_span_type{p_sv.data(), p_sv.size()};
}

template<typename T,
         std::enable_if_t<!std::is_array_v<T>
                          && yy_traits::is_c_string_v<T>, bool> = true>
constexpr auto make_span(T /* p_str */)
{
  static_assert(std::is_array_v<T>
                || !yy_traits::is_c_string_v<T>, "Can't create a 'span<>' from 'char *' or 'char[]'!");
  // std::string_view str{p_str};
  // return make_span(str);
}

template<typename T,
         size_type N>
constexpr auto make_span(T (& p_array)[N])
{
  using helper = span_traits_helper<T>;
  using traits = typename helper::traits;
  using ptr = typename traits::ptr;

  ptr l_array = p_array;
  return typename span_traits_helper<T>::span_type{l_array, N};
}

template<typename T,
         std::enable_if_t<yy_traits::is_span_v<T>, bool> = true>
constexpr auto make_span(T p_span)
{
  return T{p_span};
}

template<typename T,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_std_string_v<T>
                          || yy_traits::is_array_v<T>, bool> = true>
constexpr auto make_const_span(T & container)
{
  return typename span_traits_helper<T>::const_span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_std_string_view_v<T>, bool> = true>
constexpr auto make_const_span(T p_sv)
{
  return typename span_traits_helper<T>::const_span_type{p_sv.data(), p_sv.size()};
}

template<typename T,
         std::enable_if_t<!std::is_array_v<T>
                          && yy_traits::is_c_string_v<T>, bool> = true>
constexpr auto make_const_span(T p_str)
{
  std::string_view str{p_str};
  return typename span_traits_helper<std::string_view>::const_span_type{str.data(), str.size()};
}

template<typename T,
         size_type N,
         std::enable_if_t<!std::is_same_v<char, yy_traits::remove_cvr_t<T>>, bool> = true>
constexpr auto make_const_span(T (& p_array)[N])
{
  using helper = span_traits_helper<T>;
  using traits = typename helper::traits;
  using const_ptr = typename traits::const_ptr;

  const_ptr l_array = p_array;
  return typename span_traits_helper<T>::const_span_type{l_array, N};
}

template<typename T,
         std::enable_if_t<yy_traits::is_span_v<T>, bool> = true>
constexpr auto make_const_span(T p_span)
{
  return T{p_span};
}

} // namespace yafiyogi::yy_quad
