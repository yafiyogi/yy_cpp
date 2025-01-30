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

template<typename ConatinerType>
class iterator_ptr final
{
  public:
    using container_type = yy_traits::remove_cvr_t<ConatinerType>;

    using iterator_category = std::contiguous_iterator_tag;
    using difference_type = ssize_type;
    using value_type = typename container_type::value_type;
    using pointer = std::add_pointer_t<value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;

    using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    constexpr iterator_ptr(container_type * p_container,
                           size_type p_offset) noexcept:
      m_ptr(p_container->data() + p_offset)
    {
    }

    constexpr iterator_ptr(pointer p_data) noexcept:
      m_ptr(p_data)
    {
    }

    constexpr iterator_ptr() noexcept = default;
    constexpr iterator_ptr(const iterator_ptr &) noexcept = default;
    constexpr iterator_ptr(iterator_ptr && other) noexcept:
      m_ptr(other.m_ptr)
    {
      other.m_ptr = nullptr;
    }

    constexpr ~iterator_ptr() noexcept = default;

    constexpr iterator_ptr & operator=(const iterator_ptr &) noexcept = default;
    constexpr iterator_ptr & operator=(iterator_ptr && other) noexcept
    {
      if(this != &other)
      {
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
      }
      return *this;
    }

    constexpr iterator_ptr & operator++() noexcept
    {
      ++m_ptr;

      return *this;
    }

    constexpr iterator_ptr operator++(int) noexcept
    {
      iterator_ptr tmp{*this};

      ++m_ptr;

      return tmp;
    }

    constexpr iterator_ptr & operator+=(ssize_type p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    constexpr iterator_ptr & operator+=(int p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    constexpr iterator_ptr & operator+=(size_type p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    friend constexpr iterator_ptr operator+(iterator_ptr p_iter,
                                            ssize_type p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    friend constexpr iterator_ptr operator+(iterator_ptr p_iter,
                                            int p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    friend constexpr iterator_ptr operator+(iterator_ptr p_iter,
                                            size_type p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    constexpr iterator_ptr & operator--() noexcept
    {
      --m_ptr;

      return *this;
    }

    constexpr iterator_ptr operator--(int) noexcept
    {
      iterator_ptr tmp{*this};

      --m_ptr;

      return tmp;
    }

    constexpr iterator_ptr & operator-=(ssize_type p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr iterator_ptr & operator-=(int p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr iterator_ptr & operator-=(size_type p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr ssize_type operator-(const iterator_ptr & other) const noexcept
    {
      return m_ptr - other.m_ptr;
    }

    friend constexpr iterator_ptr operator-(iterator_ptr p_iter,
                                            ssize_type p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    friend constexpr iterator_ptr operator-(iterator_ptr p_iter,
                                            int p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    friend constexpr iterator_ptr operator-(iterator_ptr p_iter,
                                            size_type p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    constexpr bool operator==(const iterator_ptr & other) const noexcept
    {
      return m_ptr == other.m_ptr;
    }

    constexpr bool operator<(const iterator_ptr & other) const noexcept
    {
      return m_ptr < other.m_ptr;
    }

    constexpr bool operator>(const iterator_ptr & other) const noexcept
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

template<typename ContainerType>
class const_iterator_ptr final
{
  public:
    using container_type = yy_traits::remove_cvr_t<ContainerType>;

    using iterator_category = std::contiguous_iterator_tag;
    using difference_type = ssize_type;
    using value_type = typename container_type::value_type;
    using pointer = std::add_pointer_t<std::add_const_t<value_type>>;
    using reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    constexpr const_iterator_ptr(const container_type * p_container,
                                 size_type p_offset) noexcept:
      m_ptr(p_container->data() + p_offset)
    {
    }

    constexpr const_iterator_ptr(const iterator_ptr<container_type> & p_iterator) noexcept:
      m_ptr(p_iterator.ptr())
    {
    }

    constexpr const_iterator_ptr(pointer p_data) noexcept:
      m_ptr(p_data)
    {
    }

    constexpr const_iterator_ptr() noexcept = default;
    constexpr const_iterator_ptr(const const_iterator_ptr &) noexcept = default;
    constexpr const_iterator_ptr(const_iterator_ptr && other) noexcept:
      m_ptr(other.m_ptr)
    {
      other.m_ptr = nullptr;
    }

    constexpr ~const_iterator_ptr() noexcept = default;

    constexpr const_iterator_ptr & operator=(const const_iterator_ptr &) noexcept = default;
    constexpr const_iterator_ptr & operator=(const_iterator_ptr && other) noexcept
    {
      if(this != &other)
      {
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
      }
      return *this;
    }

    constexpr const_iterator_ptr & operator++() noexcept
    {
      ++m_ptr;

      return *this;
    }

    constexpr const_iterator_ptr operator++(int) noexcept
    {
      const_iterator_ptr tmp{*this};

      ++m_ptr;

      return tmp;
    }

    constexpr const_iterator_ptr & operator+=(ssize_type p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    constexpr const_iterator_ptr & operator+=(int p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    constexpr const_iterator_ptr & operator+=(size_type p_offset) noexcept
    {
      m_ptr += p_offset;

      return *this;
    }

    friend constexpr const_iterator_ptr operator+(const_iterator_ptr p_iter,
                                                  ssize_type p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    friend constexpr const_iterator_ptr operator+(const_iterator_ptr p_iter,
                                                  int p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    friend constexpr const_iterator_ptr operator+(const_iterator_ptr p_iter,
                                                  size_type p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    constexpr const_iterator_ptr & operator--() noexcept
    {
      --m_ptr;

      return *this;
    }

    constexpr const_iterator_ptr operator--(int) noexcept
    {
      const_iterator_ptr tmp{*this};

      --m_ptr;

      return tmp;
    }

    constexpr const_iterator_ptr & operator-=(ssize_type p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr const_iterator_ptr & operator-=(int p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr const_iterator_ptr & operator-=(size_type p_offset) noexcept
    {
      m_ptr -= p_offset;

      return *this;
    }

    constexpr ssize_type operator-(const const_iterator_ptr & other) const noexcept
    {
      return m_ptr - other.m_ptr;
    }

    friend constexpr const_iterator_ptr operator-(const_iterator_ptr p_iter,
                                                  ssize_type p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    friend constexpr const_iterator_ptr operator-(const_iterator_ptr p_iter,
                                                  int p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    friend constexpr const_iterator_ptr operator-(const_iterator_ptr p_iter,
                                                  size_type p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    constexpr bool operator==(const const_iterator_ptr & other) const noexcept
    {
      return m_ptr == other.m_ptr;
    }

    constexpr bool operator<(const const_iterator_ptr & other) const noexcept
    {
      return m_ptr < other.m_ptr;
    }

    constexpr bool operator>(const const_iterator_ptr & other) const noexcept
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

} // namespace yafiyogi::yy_quad::iterator_detail
