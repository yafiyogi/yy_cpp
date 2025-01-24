/*

  MIT License

  Copyright (c) 2024 Yafiyogi

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

namespace yafiyogi::yy_quad::vector_detail {

template<typename VectorType>
class iterator
{
  public:
    using vector_type = VectorType;
    using size_type = vector_type::size_type;
    using ssize_type = vector_type::ssize_type;

    using iterator_category = std::random_access_iterator_tag;
    using difference_type = ssize_type;
    using value_type = vector_type::value_type;
    using pointer = std::add_pointer_t<value_type>;
    using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
    using reference = std::add_lvalue_reference_t<value_type>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    explicit constexpr iterator(vector_type * p_vec,
                                size_type p_offset) noexcept:
      m_vec(p_vec),
      m_offset(static_cast<ssize_type>(p_offset))
    {
    }

    constexpr iterator() noexcept = default;
    constexpr iterator(const iterator &) noexcept = default;
    constexpr iterator(iterator && p_other) noexcept:
      m_vec(p_other.m_vec),
      m_offset(p_other.m_offset)
    {
      p_other.m_vec = nullptr;
      p_other.m_offset = 0;
    }

    constexpr ~iterator() noexcept = default;

    constexpr iterator & operator=(const iterator &) noexcept = default;
    constexpr iterator & operator=(iterator && p_other) noexcept
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

    constexpr iterator & operator++() noexcept
    {
      ++m_offset;

      return *this;
    }

    constexpr iterator operator++(int) noexcept
    {
      iterator tmp{*this};

      ++m_offset;

      return tmp;
    }

    constexpr iterator & operator+=(ssize_type p_offset) noexcept
    {
      m_offset += p_offset;

      return *this;
    }

    constexpr iterator & operator+=(size_type p_offset) noexcept
    {
      m_offset += static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr ssize_type operator+(const iterator & p_other) const noexcept
    {
      return m_offset + p_other.m_offset;
    }

    friend constexpr iterator operator+(iterator p_iter,
                                        ssize_type p_offset) noexcept
    {
      p_iter.m_offset += p_offset;

      return p_iter;
    }

    friend constexpr iterator operator+(iterator p_iter,
                                        int p_offset) noexcept
    {
      p_iter.m_offset += static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr iterator operator+(iterator p_iter,
                                        size_type p_offset) noexcept
    {
      p_iter.m_offset += static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    constexpr iterator & operator--() noexcept
    {
      --m_offset;

      return *this;
    }

    constexpr iterator operator--(int) noexcept
    {
      iterator tmp{*this};

      --m_offset;

      return tmp;
    }

    constexpr iterator & operator-=(ssize_type p_offset) noexcept
    {
      m_offset -= p_offset;

      return *this;
    }

    constexpr iterator & operator-=(size_type p_offset) noexcept
    {
      m_offset -= static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr ssize_type operator-(const iterator & p_other) const noexcept
    {
      return m_offset - p_other.m_offset;
    }

    friend constexpr iterator operator-(iterator p_iter,
                                        ssize_type p_offset) noexcept
    {
      p_iter.m_offset -= p_offset;

      return p_iter;
    }

    friend constexpr iterator operator-(iterator p_iter,
                                        int p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr iterator operator-(iterator p_iter,
                                        size_type p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    constexpr bool operator==(const iterator & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset == other.m_offset);
    }

    constexpr bool operator<(const iterator & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset < other.m_offset);
    }

    constexpr bool operator>(const iterator & other) const noexcept
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
    std::add_pointer_t<vector_type> m_vec = nullptr;
    ssize_type m_offset = 0;
};

template<typename VectorType>
class const_iterator
{
  public:
    using vector_type = VectorType;
    using size_type = vector_type::size_type;
    using ssize_type = vector_type::ssize_type;

    using iterator_category = std::random_access_iterator_tag;
    using difference_type = ssize_type;
    using value_type = vector_type::value_type;
    using pointer = std::add_pointer_t<std::add_const_t<value_type>>;
    using reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    explicit constexpr const_iterator(const vector_type * p_vec,
                                      size_type p_offset) noexcept:
      m_vec(p_vec),
      m_offset(static_cast<ssize_type>(p_offset))
    {
    }

    constexpr const_iterator() noexcept = default;
    constexpr const_iterator(const const_iterator &) noexcept = default;
    constexpr const_iterator(const_iterator && p_other) noexcept:
      m_vec(p_other.m_vec),
      m_offset(p_other.m_offset)
    {
      p_other.m_vec = nullptr;
      p_other.m_offset = 0;
    }

    constexpr ~const_iterator() noexcept = default;

    constexpr const_iterator & operator=(const const_iterator &) noexcept = default;
    constexpr const_iterator & operator=(const_iterator && other) noexcept
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

    constexpr const_iterator & operator++() noexcept
    {
      ++m_offset;

      return *this;
    }

    constexpr const_iterator operator++(int) noexcept
    {
      const_iterator tmp{*this};

      ++m_offset;

      return tmp;
    }

    constexpr const_iterator & operator+=(ssize_type p_offset) noexcept
    {
      m_offset += static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr const_iterator & operator+=(size_type p_offset) noexcept
    {
      m_offset += static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr ssize_type operator+(const const_iterator & p_other) const noexcept
    {
      return m_offset + p_other.m_offset;
    }

    friend constexpr const_iterator operator+(const_iterator p_iter,
                                              ssize_type p_offset) noexcept
    {
      p_iter.m_offset += p_offset;

      return p_iter;
    }

    friend constexpr const_iterator operator+(const_iterator p_iter,
                                              int p_offset) noexcept
    {
      p_iter.m_offset += static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr const_iterator operator+(const_iterator p_iter,
                                              size_type p_offset) noexcept
    {
      p_iter.m_offset += static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    constexpr const_iterator & operator--() noexcept
    {
      --m_offset;

      return *this;
    }

    constexpr const_iterator operator--(int) noexcept
    {
      const_iterator tmp{*this};

      --m_offset;

      return tmp;
    }

    constexpr const_iterator & operator-=(ssize_type p_offset) noexcept
    {
      m_offset -= static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr const_iterator & operator-=(size_type p_offset) noexcept
    {
      m_offset -= static_cast<ssize_type>(p_offset);

      return *this;
    }

    constexpr ssize_type operator-(const const_iterator & p_other) const noexcept
    {
      return m_offset - p_other.m_offset;
    }

    friend constexpr const_iterator operator-(const_iterator p_iter,
                                              ssize_type p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr const_iterator operator-(const_iterator p_iter,
                                              int p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    friend constexpr const_iterator operator-(const_iterator p_iter,
                                              size_type p_offset) noexcept
    {
      p_iter.m_offset -= static_cast<ssize_type>(p_offset);

      return p_iter;
    }

    constexpr bool operator==(const const_iterator & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset == other.m_offset);
    }

    constexpr bool operator<(const const_iterator & other) const noexcept
    {
      return (m_vec == other.m_vec) && (m_offset < other.m_offset);
    }

    constexpr bool operator>(const const_iterator & other) const noexcept
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
    std::add_pointer_t<std::add_const_t<vector_type>> m_vec = nullptr;
    ssize_type m_offset = 0;
};

} // namespace yafiyogi::yy_quad::vector_detail
