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

#include <algorithm>
#include <limits>
#include <type_traits>

#include "yy_array_traits.h"
#include "yy_ref_traits.h"
#include "yy_string_traits.h"
#include "yy_vector_traits.h"

namespace yafiyogi {
namespace yy_quad {

template<typename ValueType>
class span;

template<typename ValueType>
class const_span;

} //namespace yy_quad

namespace yy_traits {

template<typename T>
struct SpanTraits:
      std::false_type
{
};

template<typename T>
struct SpanTraits<yy_quad::span<T>>:
      std::true_type
{
};

template<typename T>
struct SpanTraits<yy_quad::const_span<T>>:
      std::true_type
{
};

template<typename T>
using is_span = SpanTraits<T>;

template<typename T>
using is_span_t = typename is_span<T>::type;

template<typename T>
inline constexpr bool is_span_v = is_span<T>::value;

} // namespace yy_traits

namespace yy_quad {
namespace span_detail {

template<typename ValueType>
struct span_traits
{
    using value_type = yy_traits::remove_rcv_t<ValueType>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using ptr = value_type *;
    using const_ptr = const value_type *;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
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
    using ptr =  typename traits::ptr;
    using iterator =  typename traits::iterator;
    using const_ptr =  typename traits:: const_ptr;
    using const_iterator =  typename traits::const_iterator;
    using size_type =  typename traits::size_type;
    using difference_type =  typename traits::difference_type;

    static constexpr size_type npos = std::numeric_limits<size_type>::max();

    constexpr explicit span(ptr p_begin,
                            ptr p_end):
      m_begin(p_begin),
      m_end(p_end)
    {
    }

    constexpr explicit span(iterator p_begin,
                            size_type p_size) noexcept:
      m_begin(p_begin),
      m_end(p_begin + p_size)
    {
    }

    constexpr span() noexcept = default;
    constexpr span(const span &) noexcept = default;
    constexpr span(span &&) noexcept = default;
    constexpr ~span() noexcept = default;

    constexpr span & operator=(const span &) noexcept = default;
    constexpr span & operator=(span &&) noexcept = default;

    constexpr value_type & operator[](size_type idx) noexcept
    {
      return *(m_begin + idx);
    }

    constexpr const value_type & operator[](size_type idx) const noexcept
    {
      return *(m_begin + idx);
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

    constexpr span & inc_begin() noexcept
    {
      if(m_begin != m_end)
      {
        ++m_begin;
      }
      return *this;
    }

    constexpr span & inc_begin(size_type step) noexcept
    {
      if((m_begin != m_end) && (static_cast<size_type>((m_begin + step) - m_begin) <= size()))
      {
        m_begin += step;
      }
      return *this;
    }

    constexpr span & dec_end() noexcept
    {
      if(m_begin != m_end)
      {
        --m_end;
      }
      return *this;
    }

    constexpr span & dec_end(size_type step) noexcept
    {
      if((m_begin != m_end) && ((m_end - step - m_begin) >= 0))
      {
        m_end -= step;
      }
      return *this;
    }

    constexpr const_ptr data() noexcept
    {
      return m_begin;
    }

    constexpr const_ptr data() const noexcept
    {
      return m_begin;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_begin == m_end;;
    }

    [[nodiscard]]
    constexpr value_type & front()
    {
      if(m_begin == m_end)
      {
        throw std::out_of_range("span<T>::front(): span is empty!");
      }

      return *m_begin;
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref front() const
    {
      if(m_begin == m_end)
      {
        throw std::out_of_range("span<T>::front(): span is empty!");
      }

      return *m_begin;
    }

    [[nodiscard]]
    constexpr value_type & back()
    {
      if(m_begin == m_end)
      {
        throw std::out_of_range("span<T>::back(): span is empty!");
      }

      return *(m_end - 1);
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref back() const
    {
      if(m_begin == m_end)
      {
        throw std::out_of_range("span<T>::back(): span is empty!");
      }

      return *(m_end - 1);
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      return static_cast<size_type>(std::distance(m_begin, m_end));
    }

    [[nodiscard]]
    constexpr span subspan(const size_type pos,
                           size_type len = npos) const noexcept
    {
      const size_type l_size = size();
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
        len = 0;
      }

      return span{m_begin + pos, len};
    }

    template<typename type>
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(begin(), end(),
                                          other.data(), other.data() + other.size());

      //return span_detail::less_than(*this, span{other.data(), other.size()});
    }

    template<typename type>
    friend constexpr bool operator<(const type & a,
                                    const span b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(a.data(), a.data() + a.size(),
                                          b.begin(), b.end());
    }

    template<typename type>
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (size() == other.size())
        && std::equal(begin(), end(), other.data());
    }

    template<typename type>
    friend constexpr bool operator==(const type & a,
                                     const span b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (a.size() == b.size())
        && std::equal(a.data(), a.data() + a.size(), b.begin());
    }

  private:
    ptr m_begin{};
    ptr m_end{};
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
    using const_ptr =  typename traits::const_ptr;
    using const_iterator =  typename traits::const_iterator;
    using size_type =  typename traits::size_type;
    using difference_type =  typename traits::difference_type;

    static constexpr size_type npos = std::numeric_limits<size_type>::max();

    constexpr explicit const_span(const_ptr p_begin,
                                  const_ptr p_end):
      m_begin(p_begin),
      m_end(p_end)
    {
    }

    constexpr explicit const_span(const_iterator p_begin,
                                  const size_type p_size) noexcept:
      m_begin(p_begin),
      m_end(p_begin + p_size)
    {
    }

    constexpr explicit const_span(span<value_type> span) noexcept:
      m_begin(span.begin()),
      m_end(span.end())
    {
    }

    constexpr const_span() noexcept = default;
    constexpr const_span(const const_span &) noexcept = default;
    constexpr const_span(const_span &&) noexcept = default;
    constexpr ~const_span() noexcept = default;

    constexpr const_span & operator=(const const_span &) noexcept = default;
    constexpr const_span & operator=(const_span &&) noexcept = default;

    constexpr const value_type & operator[](size_type idx) const noexcept
    {
      return *(m_begin + idx);
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    {
      return m_begin;
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    {
      return m_end;
    }

    constexpr const_span & inc_begin() noexcept
    {
      if(m_begin != m_end)
      {
        ++m_begin;
      }
      return *this;
    }

    constexpr const_span & inc_begin(size_type step) noexcept
    {
      if((m_begin != m_end) && (static_cast<size_type>((m_begin + step) - m_begin) <= size()))
      {
        m_begin += step;
      }
      return *this;
    }

    constexpr const_span & dec_end() noexcept
    {
      if(m_begin != m_end)
      {
        --m_end;
      }
      return *this;
    }

    constexpr const_span & dec_end(size_type step) noexcept
    {
      if((m_begin != m_end) && ((m_end - step - m_begin) >= 0))
      {
        m_end -= step;
      }
      return *this;
    }

    constexpr const_ptr data() const noexcept
    {
      return m_begin;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_begin == m_end;;
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref front() const
    {
      if(m_begin == m_end)
      {
        throw std::out_of_range("const_span<T>::front(): span is empty!");
      }

      return *m_begin;
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref back() const
    {
      if(m_begin == m_end)
      {
        throw std::out_of_range("const_span<T>::back(): span is empty!");
      }

      return *(m_end - 1);
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      return static_cast<size_type>(std::distance(m_begin, m_end));
    }

    [[nodiscard]]
    constexpr const_span subspan(const size_type pos,
                                 size_type len = npos) const noexcept
    {
      const size_type l_size = size();
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
        len = 0;
      }

      return const_span{m_begin + pos, len};
    }

    template<typename type>
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(begin(), end(),
                                          other.data(), other.data() + other.size());
    }

    template<typename type>
    friend constexpr bool operator<(const type & a,
                                    const const_span b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(a.data(), a.data() + a.size(),
                                          b.begin(), b.end());
    }

    template<typename type>
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (size() == other.size())
        && std::equal(begin(), end(), other.data());
    }

    template<typename type>
    friend constexpr bool operator==(const type & a,
                                     const const_span b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (a.size() == b.size())
        && std::equal(a.data(), a.data() + a.size(), b.begin());
    }

    constexpr bool operator<(const span<value_type> other) const noexcept
    {
      return std::lexicographical_compare(begin(), end(),
                                          other.begin(), other.end());
    }

    friend constexpr bool operator<(const span<value_type> a,
                                    const const_span b) noexcept
    {
      return std::lexicographical_compare(a.begin(), a.end(),
                                          b.begin(), b.end());
    }

    constexpr bool operator==(const span<value_type> other) const noexcept
    {
      return (size() == other.size())
        && std::equal(begin(), end(), other.begin());
    }

    friend constexpr bool operator==(const span<value_type> a,
                                     const const_span b) noexcept
    {
      return (a.size() == b.size())
        && std::equal(a.begin(), a.end(), b.begin());
    }

  private:
    mutable const_ptr m_begin{};
    mutable const_ptr m_end{};
};


template<typename T,
         typename Enable = void>
struct span_traits_helper final
{
    static_assert(true, "I can't use this type to create a span!");
};

template<typename T>
struct span_traits_helper<T,
                          std::enable_if_t<yy_traits::is_vector_v<T>
                                           || yy_traits::is_std_string_v<T>
                                           || yy_traits::is_array_v<T>>> final:
  span_detail::span_traits<typename T::value_type>
{
    using traits = typename span_detail::span_traits<typename T::value_type>;
    using span_type = span<typename traits::value_type>;
    using const_span_type = const_span<typename traits::value_type>;
};

template<typename T>
struct span_traits_helper<T,
                          std::enable_if_t<yy_traits::is_std_string_view_v<T>>> final:
  span_detail::span_traits<typename T::value_type>
{
    using traits = typename span_detail::span_traits<typename T::value_type>;
    using span_type = const_span<typename traits::value_type>;
    using const_span_type = const_span<typename traits::value_type>;
};

template<typename T>
struct span_traits_helper<T,
                          std::enable_if_t<yy_traits::is_c_string_v<T>>> final:
  span_detail::span_traits<std::remove_pointer_t<std::decay_t<T>>>
{
    using traits = typename span_detail::span_traits<std::remove_pointer_t<std::decay_t<T>>>;
    using span_type = const_span<typename traits::value_type>;
    using const_span_type = const_span<typename traits::value_type>;
};

template<typename T,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_std_string_v<T>
                          || yy_traits::is_array_v<T>, bool> = true>
inline constexpr auto make_span(T & container)
{
  return typename span_traits_helper<T>::span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_std_string_v<T>
                          || yy_traits::is_array_v<T>, bool> = true>
inline constexpr auto make_span(const T & container)
{
  return typename span_traits_helper<T>::const_span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_std_string_view_v<T>, bool> = true>
inline constexpr auto make_span(T container)
{
  return typename span_traits_helper<T>::const_span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_c_string_v<T>, bool> = true>
inline constexpr auto make_span(T p_str)
{
  std::string_view str{p_str};
  return make_span(str);
}

template<typename T,
         std::enable_if_t<yy_traits::is_span_v<T>, bool> = true>
inline constexpr auto make_span(T p_span)
{
  return T{p_span};
}

template<typename T,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_std_string_v<T>
                          || yy_traits::is_array_v<T>, bool> = true>
inline constexpr auto make_const_span(T & container)
{
  return typename span_traits_helper<T>::const_span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_std_string_v<T>
                          || yy_traits::is_array_v<T>, bool> = true>
inline constexpr auto make_const_span(const T & container)
{
  return make_span(container);
}

template<typename T,
         std::enable_if_t<yy_traits::is_std_string_view_v<T>, bool> = true>
inline constexpr auto make_const_span(T container)
{
  return make_span(container);
}

template<typename T,
         std::enable_if_t<yy_traits::is_c_string_v<T>, bool> = true>
inline constexpr auto make_const_span(T p_str)
{
  return make_span(p_str);
}

template<typename T,
         std::enable_if_t<yy_traits::is_span_v<T>, bool> = true>
inline constexpr auto make_const_span(T p_span)
{
  return const_span<typename T::value_type>{p_span};
}

} // namespace yy_quad
} // namespace yafiyog
