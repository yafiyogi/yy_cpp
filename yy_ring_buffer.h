/*

  MIT License

  Copyright (c) 2019-2024 Yafiyogi

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
#include <type_traits>

#include "yy_type_traits.h"

namespace yafiyogi::yy_data {

template<typename ValueType,
         std::size_t Capacity>
class ring_buffer final
{
  public:
    using value_type = yy_traits::remove_cvr_t<ValueType>;
    using array_type = std::array<value_type, Capacity>;
    using size_type = array_type::size_type;

    template<typename InputValueType>
    bool push(InputValueType && value) noexcept
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      const auto old_pos = m_write_pos.load();
      const auto new_pos = calc_new_pos(old_pos);

      if(const auto read_pos = m_read_pos.load();
         new_pos == read_pos)
      {
        return false;
      }

      m_buffer[old_pos] = std::forward<value_type>(value);
      m_write_pos.store(new_pos);

      return true;
    }

    bool pop(value_type & value) noexcept
    {
      const auto write_pos = m_write_pos.load();
      const auto old_pos = m_read_pos.load();

      if(write_pos == old_pos)
      {
        return false;
      }

      value = std::move(m_buffer[old_pos]);
      m_read_pos.store(calc_new_pos(old_pos));

      return true;
    }

  private:
    static constexpr size_type calc_new_pos(size_type pos) noexcept
    {
      ++pos;
      return (pos == m_size) ? 0 : pos;
    }

    static constexpr size_type m_size = Capacity;
    array_type m_buffer;
    std::atomic<size_type> m_read_pos = 0;
    std::atomic<size_type> m_write_pos = 0;
};

} // namespace yafiyogi::yy_data
