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

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stop_token>
#include <type_traits>

#include "yy_observer_ptr.hpp"
#include "yy_types.hpp"
#include "yy_type_traits.h"

namespace yafiyogi::yy_data {

// Based on code here https://github.com/boostcon/cppnow_presentations_2023/blob/main/cppnow_slides/What_Is_Low_Latency_Cpp_Part2.pdf page 61
// See https://www.youtube.com/watch?v=5uIsadq-nyk&t=1978s

template<typename ValueType,
         size_type Capacity>
class ring_buffer final
{
  public:
    using value_type = yy_traits::remove_cvr_t<ValueType>;
    using array_type = std::array<value_type, Capacity>;

    static constexpr size_type g_size = Capacity;

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
      return m_write_pos.load(std::memory_order_acquire)
        == m_read_pos.load(std::memory_order_acquire);
    }

    size_type size() const noexcept
    {
      return g_size;
    }

    template<typename Pred>
    bool wait(std::stop_token & p_stop_token,
              Pred && pred)
    {
      std::unique_lock lck{m_mtx};

      return m_cv.wait(lck, p_stop_token, std::forward<Pred>(pred));
    }

  private:
    void wake()
    {
      m_cv.notify_one();
    }

    static constexpr size_type calc_new_pos(size_type pos) noexcept
    {
      ++pos;

      if(pos == g_size)
      {
        pos = 0;
      }

      return pos;
    }

    array_type m_buffer{};
    std::atomic<size_type> m_read_pos = 0;
    std::atomic<size_type> m_write_pos = 0;

    std::mutex m_mtx{};
    std::condition_variable_any m_cv{};
};

template<typename RingBufferType>
using ring_buffer_ptr = std::shared_ptr<RingBufferType>;

template<typename RingBufferType>
class ring_buffer_reader final
{
  public:
    using queue_type = RingBufferType;
    using queue_ptr = ring_buffer_ptr<queue_type>;
    using value_type = queue_type::value_type;

    constexpr ring_buffer_reader(queue_ptr p_queue) noexcept:
      m_queue_anchor(std::move(p_queue)),
      m_queue(m_queue_anchor.get())
    {
    }

    constexpr ring_buffer_reader() noexcept = default;
    constexpr ring_buffer_reader(const ring_buffer_reader &) noexcept = delete;
    constexpr ring_buffer_reader(ring_buffer_reader &&) noexcept = default;
    constexpr ~ring_buffer_reader() noexcept = default;

    constexpr ring_buffer_reader & operator=(const ring_buffer_reader &) noexcept = delete;
    constexpr ring_buffer_reader & operator=(ring_buffer_reader &&) noexcept = default;

    bool QSwapOut(value_type & p_data)
    {
      return m_queue && m_queue->swap_out(p_data);
    }

    template<typename Pred>
    bool QWait(std::stop_token & p_stop_token,
              Pred && pred)
    {
      return m_queue && m_queue->wait(p_stop_token, std::forward<Pred>(pred));
    }

    bool QEmpty() const
    {
      return !m_queue || m_queue->empty();
    }

  private:
    using queue_obs_ptr = yy_data::observer_ptr<queue_type>;

    queue_ptr m_queue_anchor{};
    queue_obs_ptr m_queue{};
};

template<typename RingBufferType>
class ring_buffer_writer final
{
  public:
    using queue_type = RingBufferType;
    using queue_ptr = ring_buffer_ptr<queue_type>;
    using value_type = queue_type::value_type;

    constexpr ring_buffer_writer(queue_ptr p_queue) noexcept:
      m_queue_anchor(std::move(p_queue)),
      m_queue(m_queue_anchor.get())
    {
    }

    constexpr ring_buffer_writer() noexcept = default;
    constexpr ring_buffer_writer(const ring_buffer_writer &) noexcept = delete;
    constexpr ring_buffer_writer(ring_buffer_writer &&) noexcept = default;
    constexpr ~ring_buffer_writer() noexcept = default;

    constexpr ring_buffer_writer & operator=(const ring_buffer_writer &) noexcept = delete;
    constexpr ring_buffer_writer & operator=(ring_buffer_writer &&) noexcept = default;

    bool QSwapIn(value_type & p_data)
    {
      return m_queue && m_queue->swap_in(p_data);
    }

  private:
    using queue_obs_ptr = yy_data::observer_ptr<queue_type>;

    queue_ptr m_queue_anchor{};
    queue_obs_ptr m_queue{};
};

} // namespace yafiyogi::yy_data
