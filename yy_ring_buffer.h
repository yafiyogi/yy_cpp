/*

  MIT License

  Copyright (c) 2019-2025 Yafiyogi

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

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <type_traits>

#include "yy_type_traits.h"

namespace yafiyogi::yy_data {

// Based on code here https://github.com/boostcon/cppnow_presentations_2023/blob/main/cppnow_slides/What_Is_Low_Latency_Cpp_Part2.pdf page 61
// See https://www.youtube.com/watch?v=5uIsadq-nyk&t=1978s

template<typename ValueType,
         std::size_t Capacity>
class ring_buffer final
{
  public:
    using value_type = yy_traits::remove_cvr_t<ValueType>;
    using array_type = std::array<value_type, Capacity>;

    template<typename InputValueType>
    bool push(InputValueType && value) noexcept
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      const auto old_pos = m_write_pos.load(std::memory_order_relaxed); // only push() thread changes m_write_pos
      const auto new_pos = calc_new_pos(old_pos);

      if(const auto read_pos = m_read_pos.load(std::memory_order_acquire);
         new_pos == read_pos)
      {
        return false;
      }

      m_buffer[old_pos] = std::move(value);
      m_write_pos.store(new_pos, std::memory_order_release);

      wake();

      return true;
    }

    bool pop(value_type & value) noexcept
    {
      const auto write_pos = m_write_pos.load(std::memory_order_acquire);
      const auto old_pos = m_read_pos.load(std::memory_order_relaxed);  // only pop() thread changes m_read_pos

      if(write_pos == old_pos)
      {
        return false;
      }

      value = std::move(m_buffer[old_pos]);
      m_read_pos.store(calc_new_pos(old_pos), std::memory_order_release);

      return true;
    }

    template<typename InputValueType>
    bool swap_in(InputValueType & value) noexcept
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      const auto old_pos = m_write_pos.load(std::memory_order_relaxed); // only swap_in() thread changes m_write_pos
      const auto new_pos = calc_new_pos(old_pos);

      if(const auto read_pos = m_read_pos.load(std::memory_order_acquire);
         new_pos == read_pos)
      {
        return false;
      }

      std::swap(m_buffer[old_pos], value);

      m_write_pos.store(new_pos, std::memory_order_release);

      wake();

      return true;
    }

    bool swap_out(value_type & value) noexcept
    {
      const auto write_pos = m_write_pos.load(std::memory_order_acquire);
      const auto old_pos = m_read_pos.load(std::memory_order_relaxed);  // only swap_out() thread changes m_read_pos

      if(write_pos == old_pos)
      {
        return false;
      }

      std::swap(m_buffer[old_pos], value);

      m_read_pos.store(calc_new_pos(old_pos), std::memory_order_release);

      return true;
    }

    bool empty() const noexcept
    {
      return m_write_pos.load(std::memory_order_relaxed)
        == m_read_pos.load(std::memory_order_relaxed);
    }

    size_type size() const noexcept
    {
      return m_size;
    }

    bool wait(const std::chrono::nanoseconds duration)
    {
      std::unique_lock lck{m_mtx};

      return std::cv_status::no_timeout == m_cv.wait_for(lck, duration);
    }

    template<typename Duration,
             typename Pred>
    bool wait(const Duration & duration,
              Pred && pred)
    {
      std::unique_lock lck{m_mtx};

      return m_cv.wait_for(lck, duration, std::forward<Pred>(pred));
    }

  private:
    void wake()
    {
      m_cv.notify_one();
    }

    static constexpr size_type calc_new_pos(size_type pos) noexcept
    {
      ++pos;

      if(pos == m_size)
      {
        pos = 0;
      }

      return pos;
    }

    static constexpr size_type m_size = Capacity;
    array_type m_buffer{};
    std::atomic<size_type> m_read_pos = 0;
    std::atomic<size_type> m_write_pos = 0;

    std::mutex m_mtx{};
    std::condition_variable m_cv{};

    value_type m_read_value_tmp;
    value_type m_write_value_tmp;
};

} // namespace yafiyogi::yy_data
