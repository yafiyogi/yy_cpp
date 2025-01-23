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

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include "yy_array_traits.h"
#include "yy_compare_util.h"
#include "yy_ref_traits.h"
#include "yy_span_traits.h"
#include "yy_string_traits.h"
#include "yy_vector_traits.h"

namespace yafiyogi::yy_quad {
namespace span_detail {

template<typename ValueType>
class iterator;

template<typename ValueType>
class const_iterator;

template<typename ValueType>
struct span_traits final
{
    using value_type = yy_traits::remove_cvr_t<ValueType>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using ptr = std::add_pointer_t<value_type>;
    using const_ptr = std::add_pointer_t<std::add_const_t<value_type>>;
    using iterator_type = iterator<value_type>;
    using const_iterator_type = const_iterator<value_type>;
    using size_type = std::size_t;
    using ssize_type = std::ptrdiff_t;
};

template<typename ValueType>
class iterator
{
  public:
    using traits = span_traits<ValueType>;

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = typename traits::ssize_type;
    using value_type = typename traits::value_type;
    using pointer = typename traits::ptr;
    using const_pointer = typename traits::const_ptr;
    using reference = std::add_lvalue_reference_t<value_type>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
    using size_type = typename traits::size_type;
    using ssize_type = typename traits::ssize_type;

    constexpr iterator(pointer p_ptr) noexcept:
      m_ptr(p_ptr)
    {
    }

    constexpr iterator() noexcept = default;
    constexpr iterator(const iterator &) noexcept = default;
    constexpr iterator(iterator && other) noexcept:
      m_ptr(other.m_ptr)
    {
      other.m_ptr = nullptr;
    }

    constexpr ~iterator() noexcept = default;

    constexpr iterator & operator=(const iterator &) noexcept = default;
    constexpr iterator & operator=(iterator && other) noexcept
    {
      if(this != &other)
      {
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
      }
      return *this;
    }

    constexpr iterator & operator++() noexcept
    {
      ++m_ptr;

      return *this;
    }

    constexpr iterator operator++(int) noexcept
    {
      iterator tmp{*this};

      ++m_ptr;

      return tmp;
    }

    constexpr iterator & operator+=(ssize_type p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    constexpr iterator & operator+=(size_type p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    constexpr ssize_type operator+(const iterator & other) noexcept
    {
      return m_ptr + other.m_ptr;
    }

    friend constexpr iterator operator+(const iterator & p_iter,
                                        ssize_type p_offset) noexcept
    {
      iterator rv{p_iter};

      rv += p_offset;

      return rv;
    }

    friend constexpr iterator operator+(const iterator & p_iter,
                                        int p_offset) noexcept
    {
      iterator rv{p_iter};

      rv += p_offset;

      return rv;
    }

    friend constexpr iterator operator+(const iterator & p_iter,
                                        size_type p_offset) noexcept
    {
      iterator rv{p_iter};

      rv += p_offset;

      return rv;
    }

    constexpr iterator & operator--() noexcept
    {
      --m_ptr;

      return *this;
    }

    constexpr iterator operator--(int) noexcept
    {
      iterator tmp{*this};

      --m_ptr;

      return tmp;
    }

    constexpr iterator & operator-=(ssize_type p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr iterator & operator-=(size_type p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr ssize_type operator-(const iterator & other) noexcept
    {
      return m_ptr - other.m_ptr;
    }

    friend constexpr iterator operator-(const iterator & p_iter,
                                        ssize_type p_offset) noexcept
    {
      iterator rv{p_iter};

      rv -= p_offset;

      return rv;
    }

    friend constexpr iterator operator-(const iterator & p_iter,
                                        int p_offset) noexcept
    {
      iterator rv{p_iter};

      rv -= p_offset;

      return rv;
    }

    friend constexpr iterator operator-(const iterator & p_iter,
                                        size_type p_offset) noexcept
    {
      iterator rv{p_iter};

      rv -= p_offset;

      return rv;
    }

    constexpr bool operator==(const iterator & other) const noexcept
    {
      return m_ptr == other.m_ptr;
    }

    constexpr bool operator<(const iterator & other) const noexcept
    {
      return m_ptr < other.m_ptr;
    }

    constexpr bool operator>(const iterator & other) const noexcept
    {
      return m_ptr > other.m_ptr;
    }

    constexpr pointer ptr() noexcept
    {
      return m_ptr;
    }

    constexpr const_pointer ptr() const noexcept
    {
      return m_ptr;
    }

    constexpr reference operator*() noexcept
    {
      return *ptr();
    }

    constexpr const_reference operator*() const noexcept
    {
      return *ptr();
    }

    constexpr pointer operator->() noexcept
    {
      return ptr();
    }

    constexpr const_pointer operator->() const noexcept
    {
      return ptr();
    }

  private:
    pointer m_ptr;
};

template<typename ValueType>
class const_iterator
{
  public:
    using traits = span_traits<ValueType>;

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = typename traits::ssize_type;
    using value_type = typename traits::value_type;
    using pointer = typename traits::const_ptr;
    using reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
    using size_type = typename traits::size_type;
    using ssize_type = typename traits::ssize_type;

    constexpr const_iterator(pointer p_ptr) noexcept:
      m_ptr(p_ptr)
    {
    }

    constexpr const_iterator() noexcept = default;
    constexpr const_iterator(const const_iterator &) noexcept = default;
    constexpr const_iterator(const_iterator && other) noexcept:
      m_ptr(other.m_ptr)
    {
      other.m_ptr = nullptr;
    }

    constexpr ~const_iterator() noexcept = default;

    constexpr const_iterator & operator=(const const_iterator &) noexcept = default;
    constexpr const_iterator & operator=(const_iterator && other) noexcept
    {
      if(this != &other)
      {
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
      }
      return *this;
    }

    constexpr const_iterator & operator++() noexcept
    {
      ++m_ptr;

      return *this;
    }

    constexpr const_iterator operator++(int) noexcept
    {
      const_iterator tmp{*this};

      ++m_ptr;

      return tmp;
    }

    constexpr const_iterator & operator+=(ssize_type p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    constexpr const_iterator & operator+=(size_type p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    constexpr ssize_type operator+(const const_iterator & other) noexcept
    {
      return m_ptr + other.m_ptr;
    }

    friend constexpr const_iterator operator+(const const_iterator & p_iter,
                                              ssize_type p_offset) noexcept
    {
      const_iterator rv{p_iter};

      rv += p_offset;

      return rv;
    }

    friend constexpr const_iterator operator+(const const_iterator & p_iter,
                                              int p_offset) noexcept
    {
      const_iterator rv{p_iter};

      rv += p_offset;

      return rv;
    }

    friend constexpr const_iterator operator+(const const_iterator & p_iter,
                                              size_type p_offset) noexcept
    {
      iterator rv{p_iter};

      rv += p_offset;

      return rv;
    }

    constexpr const_iterator & operator--() noexcept
    {
      --m_ptr;

      return *this;
    }

    constexpr const_iterator operator--(int) noexcept
    {
      const_iterator tmp{*this};

      --m_ptr;

      return tmp;
    }

    constexpr const_iterator & operator-=(ssize_type p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr const_iterator & operator-=(size_type p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr ssize_type operator-(const const_iterator & other) noexcept
    {
      return m_ptr - other.m_ptr;
    }

    friend constexpr const_iterator operator-(const const_iterator & p_iter,
                                              ssize_type p_offset) noexcept
    {
      const_iterator rv{p_iter};

      rv -= p_offset;

      return rv;
    }

    friend constexpr const_iterator operator-(const const_iterator & p_iter,
                                              int p_offset) noexcept
    {
      const_iterator rv{p_iter};

      rv -= p_offset;

      return rv;
    }

    friend constexpr const_iterator operator-(const const_iterator & p_iter,
                                              size_type p_offset) noexcept
    {
      const_iterator rv{p_iter};

      rv -= p_offset;

      return rv;
    }

    constexpr bool operator==(const const_iterator & other) const noexcept
    {
      return m_ptr == other.m_ptr;
    }

    constexpr bool operator<(const const_iterator & other) const noexcept
    {
      return m_ptr < other.m_ptr;
    }

    constexpr bool operator>(const const_iterator & other) const noexcept
    {
      return m_ptr > other.m_ptr;
    }

    constexpr pointer ptr() const noexcept
    {
      return m_ptr;
    }

    constexpr reference operator*() const noexcept
    {
      return *ptr();
    }

    constexpr pointer operator->() const noexcept
    {
      return ptr();
    }

  private:
    pointer m_ptr;
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
    using iterator = typename traits::iterator_type;
    using const_ptr = typename traits:: const_ptr;
    using const_iterator = typename traits::const_iterator_type;
    using size_type = typename traits::size_type;

    static constexpr size_type npos = std::numeric_limits<size_type>::max();

    template<typename T,
             std::enable_if_t<yy_traits::is_container_v<T>
                              && std::is_same_v<value_type, yy_traits::remove_cvr_t<typename T::value_type>>>>
    constexpr explicit span(const T & container) noexcept:
      m_begin(container.data()),
      m_end(container.data() + container.size())
    {
    }

    constexpr explicit span(ptr p_begin,
                            ptr p_end) noexcept:
      m_begin(p_begin),
      m_end(p_end)
    {
    }

    constexpr explicit span(ptr p_begin,
                            size_type p_size) noexcept:
      m_begin(p_begin),
      m_end(p_begin + p_size)
    {
    }

    constexpr explicit span(iterator p_begin,
                            iterator p_end) noexcept:
      m_begin(p_begin.ptr()),
      m_end(p_end.ptr())
    {
    }

    constexpr explicit span(iterator p_begin,
                            size_type p_size) noexcept:
      m_begin(p_begin.ptr),
      m_end(p_begin.ptr + p_size)
    {
    }

    constexpr span() noexcept = default;
    constexpr span(const span &) noexcept = default;
    constexpr span(span &&) noexcept = default;
    constexpr ~span() noexcept = default;

    constexpr span & operator=(const span &) noexcept = default;
    constexpr span & operator=(span &&) noexcept = default;

    [[nodiscard]]
    constexpr value_type & operator[](size_type idx) noexcept
    {
      return *(m_begin + idx);
    }

    [[nodiscard]]
    constexpr const value_type & operator[](size_type idx) const noexcept
    {
      return *(m_begin + idx);
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept
    {
      return iterator{m_begin};
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    {
      return const_iterator{m_begin};
    }

    [[nodiscard]]
    constexpr iterator end() noexcept
    {
      return iterator{m_end};
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    {
      return const_iterator{m_end};
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
      m_begin += std::min(step, size());

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
      m_end -= std::min(step, size());

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
      return static_cast<size_type>(m_end - m_begin);
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

    constexpr size_type find_pos(value_type p_value) const noexcept
    {
      auto begin_pos = begin();
      auto end_pos = end();
      auto found = std::find(begin_pos, end_pos, p_value);

      if(found == end_pos)
      {
        return npos;
      }

      return static_cast<size_type>(found - begin_pos);
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
    ptr m_end = nullptr;
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
    using iterator = typename traits::const_iterator_type;
    using size_type = typename traits::size_type;

    static constexpr size_type npos = std::numeric_limits<size_type>::max();

    template<typename T,
             std::enable_if_t<yy_traits::is_container_v<T>
                              && std::is_same_v<value_type, yy_traits::remove_cvr_t<typename T::value_type>>>>
    constexpr explicit const_span(const T & container) noexcept:
      m_begin(container.data()),
      m_end(container.data() + container.size())
    {
    }

    constexpr explicit const_span(ptr p_begin,
                                  ptr p_end) noexcept:
      m_begin(p_begin),
      m_end(p_end)
    {
    }

    constexpr explicit const_span(ptr p_begin,
                                  const size_type p_size) noexcept:
      m_begin(p_begin),
      m_end(p_begin + p_size)
    {
    }

    constexpr explicit const_span(iterator p_begin,
                                  iterator p_end) noexcept:
      m_begin(p_begin.ptr()),
      m_end(p_end.ptr())
    {
    }

    constexpr explicit const_span(iterator p_begin,
                                  const size_type p_size) noexcept:
      m_begin(p_begin.ptr()),
      m_end(p_begin.ptr() + p_size)
    {
    }

    constexpr explicit const_span(typename traits::iterator_type p_begin,
                                  typename traits::iterator_type p_end) noexcept:
      m_begin(p_begin.ptr()),
      m_end(p_end.ptr())
    {
    }

    constexpr explicit const_span(typename traits::iterator_type p_begin,
                                  const size_type p_size) noexcept:
      m_begin(p_begin.ptr()),
      m_end(p_begin.ptr() + p_size)
    {
    }

    constexpr explicit const_span(span<value_type> span) noexcept:
      m_begin(span.m_begin),
      m_end(span.m_end)
    {
    }

    constexpr const_span() noexcept = default;
    constexpr const_span(const const_span &) noexcept = default;
    constexpr const_span(const_span && other) noexcept
    {
      m_begin = other.m_begin;
      other.m_begin = nullptr;
      m_end = other.m_end;
      other.m_end = nullptr;
    }

    constexpr ~const_span() noexcept = default;

    constexpr const_span & operator=(const const_span &) noexcept = default;
    constexpr const_span & operator=(const_span && other) noexcept
    {
      if(this != &other)
      {
        m_begin = other.m_begin;
        other.m_begin = nullptr;
        m_end = other.m_end;
        other.m_end = nullptr;
      }

      return *this;
    }

    [[nodiscard]]
    constexpr const value_type & operator[](size_type idx) const noexcept
    {
      return *(m_begin + idx);
    }

    [[nodiscard]]
    constexpr iterator begin() const noexcept
    {
      return iterator{m_begin};
    }

    [[nodiscard]]
    constexpr iterator end() const noexcept
    {
      return iterator{m_end};
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
      m_begin += std::min(step, size());

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
      m_end -= std::min(step, size());

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
      return static_cast<size_type>(m_end - m_begin);
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

    constexpr size_type find_pos(value_type p_value) const noexcept
    {
      auto begin_pos = begin();
      auto end_pos = end();
      auto found = std::find(begin_pos, end_pos, p_value);

      if(found == end_pos)
      {
        return npos;
      }

      return static_cast<size_type>(found - begin_pos);
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
    mutable ptr m_begin = 0;
    mutable ptr m_end = 0;
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
         std::enable_if_t<yy_traits::is_vector_v<T>
                          || yy_traits::is_std_string_v<T>
                          || yy_traits::is_array_v<T>, bool> = true>
constexpr auto make_span(const T & container)
{
  return typename span_traits_helper<T>::const_span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_std_string_view_v<T>, bool> = true>
constexpr auto make_span(T container)
{
  return typename span_traits_helper<T>::const_span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_c_string_v<T>, bool> = true>
constexpr auto make_span(T p_str)
{
  std::string_view str{p_str};
  return make_span(str);
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
constexpr auto make_const_span(T container)
{
  return typename span_traits_helper<T>::const_span_type{container.data(), container.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_c_string_v<T>, bool> = true>
constexpr auto make_const_span(T p_str)
{
  std::string_view str{p_str};
  return typename span_traits_helper<std::string_view>::const_span_type{str.data(), str.size()};
}

template<typename T,
         std::enable_if_t<yy_traits::is_span_v<T>, bool> = true>
constexpr auto make_const_span(T p_span)
{
  return T{p_span};
}

} // namespace yafiyogi::yy_quad
