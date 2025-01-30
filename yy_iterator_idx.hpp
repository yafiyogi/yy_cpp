/*

  MIT License

  Copyright (c) 2025 Yafiyogi

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

#include "yy_types.hpp"

namespace yafiyogi::yy_data::iterator_detail {

template<typename ContainerType>
class const_iterator_idx;

template<typename ContainerType>
class iterator_idx final
{
  public:
    using container_type = yy_traits::remove_cvr_t<ContainerType>;

    using iterator_category = std::contiguous_iterator_tag;
    using difference_type = ssize_type;
    using value_type = typename container_type::value_type;
    using pointer = std::add_pointer_t<value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;

    using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    friend const_iterator_idx<ContainerType>;

    explicit constexpr iterator_idx(container_type * p_vec,
                                    size_type p_offset) noexcept:
      m_vec(p_vec),
      m_offset(static_cast<ssize_type>(p_offset))
    {
    }

    constexpr iterator_idx() noexcept = default;
    constexpr iterator_idx(const iterator_idx &) noexcept = default;
    constexpr iterator_idx(iterator_idx && p_other) noexcept:
      m_vec(p_other.m_vec),
      m_offset(p_other.m_offset)
    {
      p_other.m_vec = nullptr;
      p_other.m_offset = 0;
    }

    constexpr ~iterator_idx() noexcept = default;

    constexpr iterator_idx & operator=(const iterator_idx &) noexcept = default;
    constexpr iterator_idx & operator=(iterator_idx && p_other) noexcept
    {
      if(&p_other != this)
      {
        m_vec = p_other.m_vec;
        p_other.m_vec = nullptr;
        m_offset = p_other.m_offset;
        p_other.m_offset = 0;
      }

      return *this;
    }

    constexpr iterator_idx & operator++() noexcept
    {
      ++m_offset;

      return *this;
    }

    constexpr iterator_idx operator++(int) noexcept
    {
      iterator_idx tmp{*this};

      ++m_offset;

      return tmp;
    }

    constexpr iterator_idx & operator+=(ssize_type p_offset) noexcept
    {
      m_offset += p_offset;

      return *this;
    }

    constexpr iterator_idx & operator+=(int p_offset) noexcept
    {
      m_offset += p_offset;

      return *this;
    }

    constexpr iterator_idx & operator+=(size_type p_offset) noexcept
    {
      m_offset += static_cast<ssize_type>(p_offset);

      return *this;
    }

    friend constexpr iterator_idx operator+(iterator_idx p_iter,
                                            ssize_type p_offset) noexcept
    {
      p_iter.m_offset += p_offset;

      return p_iter;
    }

    friend constexpr iterator_idx operator+(iterator_idx p_iter,
                                            int p_offset) noexcept
    {
      p_iter.m_offset += static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr iterator_idx operator+(iterator_idx p_iter,
                                            size_type p_offset) noexcept
    {
      p_iter.m_offset += static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    constexpr iterator_idx & operator--() noexcept
    {
      --m_offset;

      return *this;
    }

    constexpr iterator_idx operator--(int) noexcept
    {
      iterator_idx tmp{*this};

      --m_offset;

      return tmp;
    }

    constexpr iterator_idx & operator-=(ssize_type p_offset) noexcept
    {
      m_offset -= p_offset;

      return *this;
    }

    constexpr iterator_idx & operator-=(int p_offset) noexcept
    {
      m_offset -= p_offset;

      return *this;
    }

    constexpr iterator_idx & operator-=(size_type p_offset) noexcept
    {
      m_offset -= static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr ssize_type operator-(const iterator_idx & p_other) const noexcept
    {
      return m_offset - p_other.m_offset;
    }

    friend constexpr iterator_idx operator-(iterator_idx p_iter,
                                            ssize_type p_offset) noexcept
    {
      p_iter.m_offset -= p_offset;

      return p_iter;
    }

    friend constexpr iterator_idx operator-(iterator_idx p_iter,
                                            int p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr iterator_idx operator-(iterator_idx p_iter,
                                            size_type p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    constexpr bool operator==(const iterator_idx & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset == other.m_offset);
    }

    constexpr bool operator<(const iterator_idx & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset < other.m_offset);
    }

    constexpr bool operator>(const iterator_idx & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset > other.m_offset);
    }

    constexpr pointer ptr() noexcept
    {
      return m_vec->data() + m_offset;
    }

    constexpr const_pointer ptr() const noexcept
    {
      return m_vec->data() + m_offset;
    }

    constexpr ssize_type offset() noexcept
    {
      return m_offset;
    }

    constexpr ssize_type offset() const noexcept
    {
      return m_offset;
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
    std::add_pointer_t<container_type> m_vec = nullptr;
    ssize_type m_offset = 0;
};

template<typename ContainerType>
class const_iterator_idx final
{
  public:
    using container_type = yy_traits::remove_cvr_t<ContainerType>;

    using iterator_category = std::contiguous_iterator_tag;
    using difference_type = ssize_type;
    using value_type = container_type::value_type;
    using pointer = std::add_pointer_t<std::add_const_t<value_type>>;
    using reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    explicit constexpr const_iterator_idx(const container_type * p_vec,
                                          size_type p_offset) noexcept:
      m_vec(p_vec),
      m_offset(static_cast<ssize_type>(p_offset))
    {
    }

    explicit constexpr const_iterator_idx(const iterator_idx<container_type> & p_iterator) noexcept:
      m_vec(p_iterator.m_vec),
      m_offset(p_iterator.m_offset)
    {
    }

    constexpr const_iterator_idx() noexcept = default;
    constexpr const_iterator_idx(const const_iterator_idx &) noexcept = default;
    constexpr const_iterator_idx(const_iterator_idx && p_other) noexcept:
      m_vec(p_other.m_vec),
      m_offset(p_other.m_offset)
    {
      p_other.m_vec = nullptr;
      p_other.m_offset = 0;
    }

    constexpr ~const_iterator_idx() noexcept = default;

    constexpr const_iterator_idx & operator=(const const_iterator_idx &) noexcept = default;
    constexpr const_iterator_idx & operator=(const_iterator_idx && other) noexcept
    {
      if(&other != this)
      {
        m_vec = other.m_vec;
        other.m_vec = nullptr;
        m_offset = other.m_offset;
        other.m_offset = 0;
      }

      return *this;
    }

    constexpr const_iterator_idx & operator++() noexcept
    {
      ++m_offset;

      return *this;
    }

    constexpr const_iterator_idx operator++(int) noexcept
    {
      const_iterator_idx tmp{*this};

      ++m_offset;

      return tmp;
    }

    constexpr const_iterator_idx & operator+=(ssize_type p_offset) noexcept
    {
      m_offset += static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr const_iterator_idx & operator+=(int p_offset) noexcept
    {
      m_offset += static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr const_iterator_idx & operator+=(size_type p_offset) noexcept
    {
      m_offset += static_cast<ssize_type>(p_offset);

      return *this;
    }

    friend constexpr const_iterator_idx operator+(const_iterator_idx p_iter,
                                                  ssize_type p_offset) noexcept
    {
      p_iter.m_offset += p_offset;

      return p_iter;
    }

    friend constexpr const_iterator_idx operator+(const_iterator_idx p_iter,
                                                  int p_offset) noexcept
    {
      p_iter.m_offset += static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr const_iterator_idx operator+(const_iterator_idx p_iter,
                                                  size_type p_offset) noexcept
    {
      p_iter.m_offset += static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    constexpr const_iterator_idx & operator--() noexcept
    {
      --m_offset;

      return *this;
    }

    constexpr const_iterator_idx operator--(int) noexcept
    {
      const_iterator_idx tmp{*this};

      --m_offset;

      return tmp;
    }

    constexpr const_iterator_idx & operator-=(ssize_type p_offset) noexcept
    {
      m_offset -= static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr const_iterator_idx & operator-=(int p_offset) noexcept
    {
      m_offset -= static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr const_iterator_idx & operator-=(size_type p_offset) noexcept
    {
      m_offset -= static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr ssize_type operator-(const const_iterator_idx & p_other) const noexcept
    {
      return m_offset - p_other.m_offset;
    }

    friend constexpr const_iterator_idx operator-(const_iterator_idx p_iter,
                                                  ssize_type p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr const_iterator_idx operator-(const_iterator_idx p_iter,
                                                  int p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr const_iterator_idx operator-(const_iterator_idx p_iter,
                                                  size_type p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    constexpr bool operator==(const const_iterator_idx & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset == other.m_offset);
    }

    constexpr bool operator<(const const_iterator_idx & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset < other.m_offset);
    }

    constexpr bool operator>(const const_iterator_idx & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset > other.m_offset);
    }

    constexpr pointer ptr() const noexcept
    {
      return m_vec->data() + m_offset;
    }

    constexpr ssize_type offset() const noexcept
    {
      return m_offset;
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
    std::add_pointer_t<std::add_const_t<container_type>> m_vec = nullptr;
    ssize_type m_offset = 0;
};

} // namespace yafiyogi::yy_quad::vector_detail
