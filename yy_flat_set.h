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

#include <cstddef>

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>

#include "yy_clear_action.h"
#include "yy_lower_bound.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_vector.h"

namespace yafiyogi::yy_data {
namespace flat_set_detail {

template<typename Value,
         ClearAction ValueClearAction>
struct traits_type final
{
    using value_type = yy_traits::remove_cvr_t<Value>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using value_vector = yy_quad::simple_vector<value_type, ValueClearAction>;
    using value_ptr = value_vector::value_ptr;
    using const_value_ptr = value_vector::const_value_ptr;
    using size_type = std::size_t;
};

} // namespace flat_set_detail


template<typename Value,
         ClearAction ValueClearAction = default_clear_action_v<Value>>
class flat_set final
{
  public:
    using traits = flat_set_detail::traits_type<Value, ValueClearAction>;
    using size_type = typename traits::size_type;
    using value_type = typename traits::value_type;
    using value_l_value_ref = typename traits::value_l_value_ref;
    using value_r_value_ref = typename traits::value_r_value_ref;
    using value_vector = typename traits::value_vector;
    using value_ptr = typename traits::value_ptr;
    using const_value_ptr = typename traits::const_value_ptr;
    using difference_type = std::ptrdiff_t;

    template<typename InputValueType>
    constexpr flat_set(std::initializer_list<InputValueType> init)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "The values are of an incompatible type.");

      reserve(init.size());
      for(auto & value : init)
      {
        emplace(std::move(value));
      }
    }

    constexpr flat_set() noexcept = default;
    constexpr flat_set(const flat_set &) noexcept = default;
    constexpr flat_set(flat_set &&) noexcept = default;
    constexpr ~flat_set() noexcept = default;

    constexpr flat_set & operator=(const flat_set &) noexcept = default;
    constexpr flat_set & operator=(flat_set &&) noexcept = default;

    struct pos_end_type final
    {
        std::size_t pos{};
        bool is_end = false;
    };

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr pos_end_type lower_bound_pos(const ValueParamType & p_value) const noexcept
    {
      return do_lower_bound(p_value);
    }

    template<typename ValueParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr bool lower_bound(Visitor && visitor,
                               const ValueParamType & p_value) noexcept
    {
      auto [pos, is_end] = do_lower_bound(p_value);

      if(!is_end)
      {
        visitor(value(pos), pos);
      }

      return is_end;
    }

    template<typename ValueParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr bool lower_bound(Visitor && visitor,
                               const ValueParamType & p_value) const noexcept
    {
      auto [pos, is_end] = do_lower_bound(p_value);

      if(!is_end)
      {
        visitor(value(pos), pos);
      }

      return is_end;
    }

    struct value_pos_type final
    {
        value_ptr value = nullptr;
        size_type pos{};
    };

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr value_pos_type find(const ValueParamType & p_value) noexcept
    {
      auto [pos, found] = do_find(p_value);

      if(found)
      {
        return value_pos_type{value(pos), pos};
      }

      return value_pos_type{nullptr, pos};
    }

    struct const_value_pos_type final
    {
        value_ptr value = nullptr;
        size_type pos{};
    };

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr const_value_pos_type find(const ValueParamType & p_value) const noexcept
    {
      auto [pos, found] = do_find(p_value);

      if(found)
      {
        return const_value_pos_type{value(pos), pos};
      }

      return const_value_pos_type{nullptr, pos};
    }

    struct pos_found_type final
    {
        std::size_t pos{};
        bool found = false;
    };

    template<typename ValueParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const ValueParamType & p_value) noexcept
    {
      auto [pos, found] = do_find(p_value);

      if(found)
      {
        visitor(value(pos), pos);
      }

      return pos_found_type{pos, found};
    }

    template<typename ValueParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const ValueParamType & p_value) const noexcept
    {
      auto [pos, found] = do_find(p_value);

      if(found)
      {
        visitor(value(pos), pos);
      }

      return pos_found_type{pos, found};
    }

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr pos_found_type find_pos(const ValueParamType & p_value) const noexcept
    {
      return do_find(p_value);
    }

    [[nodiscard]]
    constexpr value_type & operator[](size_type pos) noexcept
    {
      return *value(pos);
    }

    [[nodiscard]]
    constexpr const value_type & operator[](size_type pos) const noexcept
    {
      return *value(pos);
    }

    [[nodiscard]]
    constexpr value_type & add_empty(size_type p_pos)
    {
      auto [value_pos, value_added] = m_values.add_empty(m_values.begin() + p_pos);
      if(!value_added)
      {
        throw std::runtime_error("flat_set::add_empty() value add_empty() failed!");
      }

      return *value_pos;
    }

    template<typename InputValueType>
    constexpr size_type emplace(size_type p_pos,
                                InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");

      auto value = add_empty(p_pos);

      value = std::forward<InputValueType>(p_value);

      return static_cast<size_type>(&value - m_values.begin());
    }

    struct pos_inserted_type final
    {
        std::size_t pos{};
        bool inserted = false;
    };

    template<typename InputValueType>
    constexpr pos_inserted_type emplace(InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");

      auto [iter, found] = do_find_raw(p_value);

      if(!found)
      {
        iter = m_values.emplace(iter,
                                std::forward<InputValueType>(p_value)).iter;
      }

      auto pos = static_cast<size_type>(iter - m_values.begin());

      return pos_inserted_type{pos, !found};
    }

    constexpr void swap(flat_set & other)
    {
      if(this != &other)
      {
        std::swap(m_values, other.m_values);
      }
    }

    constexpr void swap(flat_set && other)
    {
      if(this != &other)
      {
        m_values = std::move(other.m_values);
      }
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      return m_values.size();
    }

    constexpr void reserve(size_type size)
    {
      m_values.reserve(size);
    }

    void clear() noexcept
    {
      m_values.clear();
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_values.empty();
    }

    [[nodiscard]]
    constexpr bool operator<(const flat_set & other) const noexcept
    {
      if(empty())
      {
        return !other.empty();
      }

      if(!empty() && other.empty())
      {
        return false;
      }

      const auto max = std::min(size(), other.size());
      for(size_type idx = 0; idx < max; ++idx)
      {
        if(m_values[idx] < other.m_values[idx])
        {
          return true;
        }

        if(!(m_values[idx] == other.m_values[idx]))
        {
          return false;
        }
      }

      return size() < other.size();
    }

    [[nodiscard]]
    constexpr bool operator==(const flat_set & other) const noexcept
    {
      if(empty())
      {
        return other.empty();
      }

      if(!empty() && other.empty())
      {
        return false;
      }

      const auto max = std::min(size(), other.size());
      for(size_type idx = 0; idx < max; ++idx)
      {
        if(!(m_values[idx] == other.m_values[idx]))
        {
          return false;
        }
      }

      return size() == other.size();
    }

  private:
    [[nodiscard]]
    constexpr value_ptr value(size_type idx) noexcept
    {
      return m_values.data() + idx;
    }

    [[nodiscard]]
    constexpr const_value_ptr value(size_type idx) const noexcept
    {
      return m_values.data() + idx;
    }

    struct iter_end_type final
    {
        value_ptr iter = nullptr;
        bool is_end = false;
    };

    struct const_iter_end_type final
    {
        const_value_ptr iter = nullptr;
        bool is_end = false;
    };

    struct iter_found_type final
    {
        value_ptr iter = nullptr;
        bool found = false;
    };

    struct const_iter_found_type final
    {
        const_value_ptr iter = nullptr;
        bool found = false;
    };

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr iter_end_type do_lower_bound_raw(const ValueParamType & p_value) noexcept
    {
      value_ptr begin = m_values.begin();
      value_ptr end = m_values.end();
      value_ptr iter = yy_data::lower_bound(begin, end, p_value);

      return iter_end_type{iter, iter == end};
    }

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr const_iter_end_type do_lower_bound_raw(const ValueParamType & p_value) const noexcept
    {
      const_value_ptr begin = m_values.begin();
      const_value_ptr end = m_values.end();
      const_value_ptr iter = yy_data::lower_bound(begin, end, p_value);

      return const_iter_end_type{iter, iter == end};
    }

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr pos_end_type do_lower_bound(const ValueParamType & p_value) const noexcept
    {
      auto [iter, is_end] = do_lower_bound_raw(p_value);

      return pos_end_type{static_cast<size_type>(iter - m_values.data()), is_end};
    }

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr iter_found_type do_find_raw(const ValueParamType & p_value) noexcept
    {
      auto [iter, is_end] = do_lower_bound_raw(p_value);

      bool found = !is_end && (*iter == p_value);

      return iter_found_type{iter, found};
    }

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr const_iter_found_type do_find_raw(const ValueParamType & p_value) const noexcept
    {
      auto [iter, is_end] = do_lower_bound_raw(p_value);

      bool found = !is_end && (*iter == p_value);

      return const_iter_found_type{iter, found};
    }

    template<typename ValueParamType>
    [[nodiscard]]
    constexpr pos_found_type do_find(const ValueParamType & p_value) const noexcept
    {
      auto [iter, found] = do_find_raw(p_value);

      return pos_found_type{static_cast<size_type>(iter - m_values.data()), found};
    }

    value_vector m_values;
};

} // namespace yafiyogi::yy_data
