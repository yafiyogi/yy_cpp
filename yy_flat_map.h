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

#include <cstddef>

#include <algorithm>
#include <initializer_list>
#include <tuple>
#include <type_traits>

#include "yy_assert.h"
#include "yy_clear_action.h"
#include "yy_compare_util.h"
#include "yy_find_raw_util.hpp"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_types.hpp"
#include "yy_vector.h"

namespace yafiyogi::yy_data {
namespace flat_map_detail {

template<typename Key,
         typename Value,
         ClearAction KeyClearAction,
         ClearAction ValueClearAction>
struct traits_type final
{
    using key_type = yy_traits::remove_cvr_t<Key>;
    using key_l_value_ref = typename yy_traits::ref_traits<key_type>::l_value_ref;
    using key_r_value_ref = typename yy_traits::ref_traits<key_type>::r_value_ref;
    using key_vector = yy_quad::simple_vector<key_type, KeyClearAction>;
    using key_ptr =  std::add_pointer_t<typename key_vector::value_type>;
    using const_key_ptr =  std::add_pointer_t<std::add_const_t<typename key_vector::value_type>>;
    using value_type = yy_traits::remove_cvr_t<Value>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using value_vector = yy_quad::simple_vector<value_type, ValueClearAction>;
    using value_ptr = std::add_pointer_t<typename value_vector::value_type>;
    using const_value_ptr = std::add_pointer_t<std::add_const_t<typename value_vector::value_type>>;
};

} // namespace flat_map_detail

template<typename Key,
         typename Value,
         ClearAction KeyClearAction = default_clear_action_v<Key>,
         ClearAction ValueClearAction = default_clear_action_v<Value>,
         size_type SearchSizeThreshold = ((yy_data::find_util_detail::size_threshold_cache_line_size * 2) / sizeof(Key)) + 1>
class flat_map final
{
  public:
    using traits = flat_map_detail::traits_type<Key, Value, KeyClearAction, ValueClearAction>;
    using key_type = typename traits::key_type;
    using key_ptr = typename traits::key_ptr;
    using const_key_ptr = typename traits::const_key_ptr;
    using key_l_value_ref = typename traits::key_l_value_ref;
    using key_r_value_ref = typename traits::key_r_value_ref;
    using value_type = typename traits::value_type;
    using value_ptr = typename traits::value_ptr;
    using const_value_ptr = typename traits::const_value_ptr;
    using value_l_value_ref = typename traits::value_l_value_ref;
    using value_r_value_ref = typename traits::value_r_value_ref;
    using key_vector = typename traits::key_vector;
    using key_iterator = key_vector::iterator;
    using value_vector = typename traits::value_vector;
    using value_iterator = value_vector::iterator;
    using difference_type = std::ptrdiff_t;
    template<typename KeyParamType>
    using key_param_traits = yy_traits::ref_traits<KeyParamType>;
    template<typename KeyParamType>
    using key_param_l_value_ref = typename key_param_traits<KeyParamType>::l_value_ref;

    constexpr flat_map(size_type capacity) noexcept:
      m_keys(capacity),
      m_values(capacity)
    {
    }

    template<typename key_type,
             typename value_type>
    constexpr flat_map(std::initializer_list<std::tuple<key_type, value_type>> init)
    {
      reserve(init.size());
      for(auto & [key, value] : init)
      {
        emplace(std::move(key), std::move(value));
      }
    }

    constexpr flat_map() noexcept = default;
    constexpr flat_map(const flat_map &) noexcept = default;
    constexpr flat_map(flat_map &&) noexcept = default;
    constexpr ~flat_map() noexcept = default;

    constexpr flat_map & operator=(const flat_map &) noexcept = default;
    constexpr flat_map & operator=(flat_map &&) noexcept = default;

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_end_type lower_bound_pos(const KeyParamType & p_key) const noexcept
    {
      return lower_bound_raw_pos(m_keys, p_key);
    }

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr bool lower_bound(Visitor && visitor,
                               const KeyParamType & p_key) noexcept
    {
      auto [pos, is_end] = lower_bound_raw_pos(m_keys, p_key);
      if(!is_end)
      {
        visitor(key(pos), value(pos), pos);
      }

      return is_end;
    }

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr bool lower_bound(Visitor && visitor,
                               const KeyParamType & p_key) const noexcept
    {
      auto [pos, is_end] = lower_bound_raw_pos(m_keys, p_key);

      if(!is_end)
      {
        visitor(key(pos), value(pos), pos);
      }

      return is_end;
    }

    struct value_pos_type final
    {
        value_ptr value = nullptr;
        size_type pos = 0;
        bool found = false;
    };

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr value_pos_type find(const KeyParamType & p_key) noexcept
    {
      auto [pos, found] = find_raw_pos<SearchSizeThreshold>(m_keys, p_key);

      return value_pos_type{value(pos), pos, found};
    }

    struct const_value_pos_type final
    {
        const_value_ptr value = nullptr;
        size_type pos = 0;
        bool found = false;
    };

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr const_value_pos_type find(const KeyParamType & p_key) const noexcept
    {
      auto [pos, found] = find_raw_pos<SearchSizeThreshold>(m_keys, p_key);

      return value_pos_type{value(pos), pos, found};
    }

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const KeyParamType & p_key) noexcept
    {
      pos_found_type pos_found{find_raw_pos<SearchSizeThreshold>(m_keys, p_key)};

      if(pos_found.found)
      {
        visitor(value(pos_found.pos), pos_found.pos);
      }

      return pos_found;
    }

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const KeyParamType & p_key) const noexcept
    {
      pos_found_type pos_found{find_raw_pos<SearchSizeThreshold>(m_keys, p_key)};

      if(pos_found.found)
      {
        visitor(value(pos_found.pos), pos_found.pos);
      }

      return pos_found;
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_found_type find_pos(const KeyParamType & p_key) const noexcept
    {
      return find_raw_pos<SearchSizeThreshold>(m_keys, p_key);
    }

    struct ref_type final
    {
        key_type & key;
        value_type & value;
    };

    [[nodiscard]]
    constexpr ref_type operator[](size_type pos) noexcept
    {
      return ref_type{*key(pos), *value(pos)};
    }

    struct const_ref_type final
    {
        const key_type & key;
        const value_type & value;
    };

    [[nodiscard]]
    constexpr const_ref_type operator[](size_type pos) const noexcept
    {
      return const_ref_type{*key(pos), *value(pos)};
    }

    struct pos_inserted_type final
    {
        size_type pos = 0;
        bool inserted = false;
    };

    template<typename InputKeyType,
             typename InputValueType>
    constexpr pos_inserted_type emplace(InputKeyType && p_key,
                                        InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputKeyType>, key_type>
                    || (std::is_pointer_v<InputKeyType> && std::is_base_of_v<key_type, yy_traits::remove_cvr_t<std::remove_pointer<InputKeyType>>>),
                    "p_key is of an incompatible type.");
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");

      auto keys_begin{m_keys.data()};
      auto [key_iter, found] = find_raw(m_keys, p_key);
      auto pos = static_cast<size_type>(key_iter - keys_begin);

      if(!found)
      {
        auto [key, value] = add_empty(key_iter);

        pos = static_cast<size_type>(key - m_keys.data());
        *key = std::forward<InputKeyType>(p_key);
        *value = std::forward<InputValueType>(p_value);
      }

      return pos_inserted_type{pos, !found};
    }

    template<typename InputKeyType,
             typename InputValueType>
    constexpr size_type emplace(size_type p_pos,
                                InputKeyType && p_key,
                                InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputKeyType>, key_type>
                    || (std::is_pointer_v<InputKeyType> && std::is_base_of_v<key_type, yy_traits::remove_cvr_t<std::remove_pointer<InputKeyType>>>),
                    "p_key is of an incompatible type.");
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");

      auto [key, value] = add_empty(m_keys.data() + static_cast<ssize_type>(p_pos));

      *key = std::forward<InputKeyType>(p_key);
      *value = std::forward<InputValueType>(p_value);

      return static_cast<size_type>(key - m_keys.data());
    }

    template<typename InputKeyType,
             typename InputValueType>
    constexpr pos_inserted_type emplace_or_assign(InputKeyType && p_key,
                                                  InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");

      auto [key_iter, found] = find_raw(m_keys, p_key);
      auto pos = (key_iter - m_keys.data());
      auto value_iter = m_values.data() + pos;

      if(!found)
      {
        std::tie(key_iter, value_iter) = add_empty(key_iter);

        *key_iter = std::forward<InputKeyType>(p_key);
      }

      *value_iter = std::forward<InputValueType>(p_value);

      return pos_inserted_type{static_cast<size_type>(pos), !found};
    }

    template<typename InputKeyType,
             typename InputValueType>
    constexpr pos_inserted_type swap_in(InputKeyType && p_key,
                                        InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");

      auto [key_iter, found] = find_raw(m_keys, p_key);
      auto pos = (key_iter - m_keys.data());
      auto value_iter = m_values.data() + pos;

      if(!found)
      {
        std::tie(key_iter, value_iter) = add_empty(key_iter);

        key_iter->swap(std::forward<InputKeyType>(p_key));
      }

      value_iter->swap(std::forward<InputValueType>(p_value));

      return pos_inserted_type{static_cast<size_type>(pos), !found};
    }

    template<typename InputKeyType>
    constexpr void erase(InputKeyType && p_key)
    {
      if(auto [key_iter, found] = find_raw(m_keys, p_key);
         found)
      {
        m_keys.erase(key_iter);

        ssize_type pos = key_iter - m_keys.data();

        m_values.erase(m_values.data() + pos);
      }
    }

    constexpr void swap(flat_map & other) noexcept
    {
      if(this != &other)
      {
        std::swap(m_keys, other.m_keys);
        std::swap(m_values, other.m_values);
      }
    }

    constexpr void swap(flat_map && other) noexcept
    {
      if(this != &other)
      {
        m_keys = std::move(other.m_keys);
        m_values = std::move(other.m_values);
      }
    }

    friend constexpr void swap(flat_map & lhs, flat_map & rhs) noexcept
    {
      lhs.swap(rhs);
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      YY_ASSERT(m_keys.size() == m_values.size());

      return m_keys.size();
    }

    struct capacity_type
    {
        size_type keys = 0;
        size_type values = 0;
    };

    [[nodiscard]]
    constexpr capacity_type capacity() const noexcept
    {
      YY_ASSERT(m_keys.size() == m_values.size());

      return capacity_type{m_keys.capacity(), m_values.capacity()};
    }

    constexpr void reserve(size_type size)
    {
      m_keys.reserve(size);
      m_values.reserve(size);
    }

    void clear() noexcept
    {
      m_keys.clear();
      m_values.clear();
    }

    void clear(ClearAction p_clear_action) noexcept
    {
      m_keys.clear(p_clear_action);
      m_values.clear(p_clear_action);
    }

    void clear(ClearAction p_key_clear_action,
               ClearAction p_value_clear_action) noexcept
    {
      m_keys.clear(p_key_clear_action);
      m_values.clear(p_value_clear_action);
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_keys.empty();
    }

    [[nodiscard]]
    constexpr bool operator<(const flat_map & p_other) const noexcept
    {
      return compare(p_other) < 0;
    }

    [[nodiscard]]
    constexpr bool operator==(const flat_map & p_other) const noexcept
    {
      return compare(p_other) == 0;
    }

    [[nodiscard]]
    constexpr int compare(const flat_map & p_other) const noexcept
    {
      if(empty())
      {
        if(p_other.empty())
        {
          return 0;
        }

        return -1;
      }
      else if(p_other.empty())
      {
        return 1;
      }

      const auto max = std::min(size(), p_other.size());
      for(size_type idx = 0; idx < max; ++idx)
      {
        const auto & key_lhs = m_keys[idx];
        const auto & key_rhs = p_other.m_keys[idx];

        if(!yy_util::equal(key_lhs, key_rhs))
        {
          if(yy_util::less_than(key_lhs, key_rhs))
          {
            return -1;
          }
          return 1;
        }

        const auto & value_lhs = m_values[idx];
        const auto & value_rhs = p_other.m_values[idx];

        if(!yy_util::equal(value_lhs, value_rhs))
        {
          if(yy_util::less_than(value_lhs, value_rhs))
          {
            return -1;
          }
          return 1;
        }
      }

      if(size() != p_other.size())
      {
        if(size() < p_other.size())
        {
          return -1;
        }

        return 1;
      }

      return 0;
    }

    template<typename Visitor>
    void visit(Visitor && visitor)
    {
      auto value_iter{m_values.data()};

      for(auto & key : m_keys)
      {
        visitor(key, *value_iter);
        ++value_iter;
      }
    }

    template<typename Visitor>
    void visit(Visitor && visitor) const
    {
      auto value_iter{m_values.data()};

      for(const auto & key : m_keys)
      {
        visitor(key, *value_iter);
        ++value_iter;
      }
    }

    template<typename Visitor>
    void visit(Visitor && visitor, size_type pos)
    {
      if(pos < size())
      {
        visitor(*key(pos), *value(pos));
      }
    }

    template<typename Visitor>
    void visit(Visitor && visitor, size_type pos) const
    {
      if(pos < size())
      {
        visitor(*key(pos), *value(pos));
      }
    }

    [[nodiscard]]
    constexpr key_ptr key(size_type idx) noexcept
    {
      return key_ptr{m_keys.data() + std::min(idx, size())};
    }

    [[nodiscard]]
    constexpr const_key_ptr key(size_type idx) const noexcept
    {
      return const_key_ptr{m_keys.data() + std::min(idx, size())};
    }

    [[nodiscard]]
    constexpr value_ptr value(size_type idx) noexcept
    {
      return value_ptr{m_values.data() + std::min(idx, size())};
    }

    [[nodiscard]]
    constexpr const_value_ptr value(size_type idx) const noexcept
    {
      return const_value_ptr{m_values.data() + std::min(idx, size())};
    }

  private:
    [[nodiscard]]
    constexpr std::tuple<key_ptr, value_ptr> add_empty(key_iterator p_pos)
    {
      auto [key_iter, key_inserted] = m_keys.add_empty(p_pos);
      if(!key_inserted)
      {
        throw std::runtime_error("flat_map::add_empty() key add_empty() failed!");
      }

      auto insert_pos = key_iter - m_keys.begin();
      auto [_, value_inserted] = m_values.add_empty(m_values.begin() + insert_pos);
      if(!value_inserted)
      {
        throw std::runtime_error("flat_map::add_empty() value add_empty() failed!");
      }

      return std::make_tuple(m_keys.data() + insert_pos, m_values.data() + insert_pos);
    }

    key_vector m_keys;
    value_vector m_values;
};

} // namespace yafiyogi::yy_data
