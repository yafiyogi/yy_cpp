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

#include <atomic>
#include "yy_atomic_traits.hpp"
#include "yy_type_traits.h"

namespace yafiyogi::yy_quad {

template<typename T>
class AtomicWrapper
{
  public:
    using value_type = yy_traits::remove_cvr_t<T>;

    constexpr AtomicWrapper() noexcept = default;
    constexpr AtomicWrapper(const AtomicWrapper & other) noexcept
    {
      copy(other);
    }

    constexpr AtomicWrapper(AtomicWrapper && other) noexcept
    {
      move(std::move(other));
    }

    constexpr AtomicWrapper & operator=(const AtomicWrapper & other) noexcept
    {
      copy(other);
      return *this;
    }

    constexpr AtomicWrapper & operator=(AtomicWrapper && other) noexcept
    {
      move(std::move(other));
      return *this;
    }

    value_type load(std::memory_order order) const noexcept
    {
      return m_value.load(order);
    }

    void store(value_type value, std::memory_order order) noexcept
    {
      m_value.store(value, order);
    }

    value_type exchange(value_type value, std::memory_order order) noexcept
    {
      return m_value.exchange(value, order);
    }

  private:
    void copy(const AtomicWrapper & other)
    {
      m_value.store(other.m_value.load(std::memory_order_seq_cst), std::memory_order_seq_cst);
    }

    void move(AtomicWrapper && other)
    {
      m_value.store(other.m_value.exchange(value_type{}, std::memory_order_seq_cst), std::memory_order_seq_cst);
    }

    std::atomic<value_type> m_value;
};

} // namespace yafiyogi::yy_quad

namespace yafiyogi::yy_traits::atomic_traits_detail {

template<typename T>
struct is_atomic<yy_quad::AtomicWrapper<T>>:
      public std::true_type
{
};

} // namespace yafiyogi::yy_traits::atomic_traits_detail
