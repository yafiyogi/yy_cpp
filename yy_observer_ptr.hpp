/*

  MIT License

  Copyright (c) 2024-2025 Yafiyogi

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

#include <type_traits>

#include "yy_types.hpp"

namespace yafiyogi::yy_data {

// Based on https://github.com/CppCon/CppCon2019/blob/master/Presentations/the_smart_pointers_i_wish_i_had/the_smart_pointers_i_wish_i_had__matthew_fleming__cppcon_2019.pdf
// page 17

template <typename T>
class observer_ptr final
{
  public:
    using value_type = std::remove_volatile_t<std::remove_reference_t<T>>;
    using pointer = std::add_pointer_t<T>;
    using const_pointer = std::add_pointer_t<std::add_const_t<T>>;

    constexpr explicit observer_ptr(pointer p) noexcept:
      m_raw_ptr(p)
    {
    }

    constexpr explicit observer_ptr() noexcept = default;
    constexpr observer_ptr(const observer_ptr &) noexcept = default;
    constexpr observer_ptr(observer_ptr &&) noexcept = default;

    constexpr ~observer_ptr() noexcept
    {
      reset();
    }

    constexpr observer_ptr & operator=(const observer_ptr &) noexcept = default;
    constexpr observer_ptr & operator=(observer_ptr &&) noexcept = default;
    constexpr observer_ptr & operator=(pointer other)
    {
      m_raw_ptr = other;

      return *this;
    }

    constexpr bool operator==(const observer_ptr & other) const noexcept
    {
      return other.m_raw_ptr == m_raw_ptr;
    }

    constexpr bool operator==(const pointer other) const noexcept
    {
      return other == m_raw_ptr;
    }

    constexpr bool operator<(const observer_ptr & other) const noexcept
    {
      return m_raw_ptr < other.m_raw_ptr;
    }

    constexpr bool operator<(const pointer other) const noexcept
    {
      return m_raw_ptr < other;
    }

    constexpr pointer release() noexcept
    {
      auto ptr = m_raw_ptr;

      reset();

      return ptr;
    }

    constexpr void reset() noexcept
    {
      m_raw_ptr = nullptr;
    }

    constexpr explicit operator bool() const noexcept
    {
      return !empty();
    }

    constexpr bool empty() const noexcept
    {
      return nullptr == m_raw_ptr;
    }

    constexpr pointer get() noexcept
    {
      return m_raw_ptr;
    }

    constexpr const_pointer get() const noexcept
    {
      return m_raw_ptr;
    }

    constexpr pointer operator->() noexcept
    {
      return m_raw_ptr;
    }

    constexpr const_pointer operator->() const noexcept
    {
      return m_raw_ptr;
    }

    constexpr T & operator*() noexcept
    {
      return *m_raw_ptr;
    }

    constexpr const T & operator*() const noexcept
    {
      return *m_raw_ptr;
    }

    constexpr observer_ptr & operator++() noexcept
    {
      ++m_raw_ptr;

      return *this;
    }

    constexpr observer_ptr operator++(int) noexcept
    {
      return observer_ptr{m_raw_ptr++};
    }

    constexpr observer_ptr & operator+=(ssize_type p_offset) noexcept
    {
      m_raw_ptr += p_offset;

      return *this;
    }

    constexpr observer_ptr & operator+=(int p_offset) noexcept
    {
      m_raw_ptr += p_offset;

      return *this;
    }

    constexpr observer_ptr & operator+=(size_type p_offset) noexcept
    {
      m_raw_ptr += p_offset;

      return *this;
    }

    friend constexpr observer_ptr operator+(observer_ptr p_iter,
                                            ssize_type p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    friend constexpr observer_ptr operator+(observer_ptr p_iter,
                                            int p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    friend constexpr observer_ptr operator+(observer_ptr p_iter,
                                            size_type p_offset) noexcept
    {
      p_iter += p_offset;

      return p_iter;
    }

    constexpr observer_ptr & operator--() noexcept
    {
      --m_raw_ptr;

      return *this;
    }

    constexpr observer_ptr operator--(int) noexcept
    {
      return observer_ptr{m_raw_ptr--};
    }

    constexpr observer_ptr & operator-=(ssize_type p_offset) noexcept
    {
      m_raw_ptr -= p_offset;

      return *this;
    }

    constexpr observer_ptr & operator-=(int p_offset) noexcept
    {
      m_raw_ptr -= p_offset;

      return *this;
    }

    constexpr observer_ptr & operator-=(size_type p_offset) noexcept
    {
      m_raw_ptr -= p_offset;

      return *this;
    }

    constexpr ssize_type operator-(const observer_ptr & other) const noexcept
    {
      return m_raw_ptr - other.m_raw_ptr;
    }

    friend constexpr observer_ptr operator-(observer_ptr p_iter,
                                            ssize_type p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    friend constexpr observer_ptr operator-(observer_ptr p_iter,
                                            int p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    friend constexpr observer_ptr operator-(observer_ptr p_iter,
                                            size_type p_offset) noexcept
    {
      p_iter -= p_offset;

      return p_iter;
    }

    constexpr void swap(observer_ptr & other) noexcept
    {
      std::swap(m_raw_ptr, other.m_raw_ptr);
    }

    friend constexpr void swap(observer_ptr & lhs, observer_ptr & rhs) noexcept
    {
      lhs.swap(rhs);
    }

  private:
    pointer m_raw_ptr = nullptr;
};

} // namespace yafiyogi::yy_data
