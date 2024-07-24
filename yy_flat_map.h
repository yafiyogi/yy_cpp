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
#include <type_traits>
#include <tuple>

#include "yy_assert.h"
#include "yy_clear_action.h"
//#include "yy_lower_bound.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_utility.h"
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
    using key_ptr = key_vector::value_ptr;
    using const_key_ptr = key_vector::const_value_ptr;
    using value_type = yy_traits::remove_cvr_t<Value>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using value_vector = yy_quad::simple_vector<value_type, ValueClearAction>;
    using value_ptr = value_vector::value_ptr;
    using const_value_ptr = value_vector::const_value_ptr;
    using size_type = std::size_t;
};

} // namespace flat_map_detail


template<typename Key,
         typename Value,
         ClearAction KeyClearAction = default_clear_action_v<Key>,
         ClearAction ValueClearAction = default_clear_action_v<Value>>
class flat_map final
{
  public:
    using traits = flat_map_detail::traits_type<Key, Value, KeyClearAction, ValueClearAction>;
    using size_type = typename traits::size_type;
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
    using value_vector = typename traits::value_vector;
    using difference_type = std::ptrdiff_t;
    template<typename KeyParamType>
    using key_param_traits = yy_traits::ref_traits<KeyParamType>;
    template<typename KeyParamType>
    using key_param_l_value_ref = typename key_param_traits<KeyParamType>::l_value_ref;

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

    struct pos_end_type final
    {
        size_type pos{};
        bool is_end = false;
    };

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_end_type lower_bound_pos(const KeyParamType & p_key) const noexcept
    {
      return do_lower_bound(p_key);
    }

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr bool lower_bound(Visitor && visitor,
                               const KeyParamType & p_key) noexcept
    {
      auto [pos, is_end] = do_lower_bound(p_key);

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
      auto [pos, is_end] = do_lower_bound(p_key);

      if(!is_end)
      {
        visitor(key(pos), value(pos), pos);
      }

      return is_end;
    }

    struct key_value_pos_type final
    {
        key_ptr key = nullptr;
        value_ptr value = nullptr;
        size_type pos{};
    };

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr key_value_pos_type find(const KeyParamType & p_key) noexcept
    {
      auto [pos, found] = do_find(p_key);

      if(found)
      {
        return key_value_pos_type{key(pos), value(pos), pos};
      }

      return key_value_pos_type{nullptr, nullptr, pos};
    }

    struct const_key_value_pos_type final
    {
        const_key_ptr key = nullptr;
        const_value_ptr value = nullptr;
        size_type pos{};
    };

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr const_key_value_pos_type find(const KeyParamType & p_key) const noexcept
    {
      auto [pos, found] = do_find(p_key);

      if(found)
      {
        return const_key_value_pos_type{key(pos), value(pos), pos};
      }

      return const_key_value_pos_type{nullptr, nullptr, pos};
    }

    struct pos_found_type final
    {
        size_type pos{};
        bool found = false;
    };

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const KeyParamType & p_key) noexcept
    {
      auto [pos, found] = do_find(p_key);

      if(found)
      {
        visitor(value(pos), pos);
      }

      return pos_found_type{pos, found};
    }

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const KeyParamType & p_key) const noexcept
    {
      auto [pos, found] = do_find(p_key);

      if(found)
      {
        visitor(value(pos), pos);
      }

      return pos_found_type{pos, found};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_found_type find_pos(const KeyParamType & p_key) const noexcept
    {
      return do_find(p_key);
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

    struct add_empty_type final
    {
        key_ptr key = nullptr;
        value_ptr value = nullptr;
        bool inserted = false;
    };

    [[nodiscard]]
    constexpr add_empty_type add_empty(size_type p_pos)
    {
      return add_empty(m_keys.begin() + p_pos);
    }

    struct pos_inserted_type final
    {
        size_type pos{};
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

      auto [key_iter, found] = do_find_raw(p_key);
      auto pos = static_cast<size_type>(key_iter - m_keys.begin());

      if(!found)
      {
        auto [key, value] = add_empty(key_iter);

        pos = static_cast<size_type>(key - m_keys.begin());
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

      auto [key, value] = add_empty(m_keys.begin() + p_pos);

      *key = std::forward<InputKeyType>(p_key);
      *value = std::forward<InputValueType>(p_value);

      return static_cast<size_type>(key - m_keys.begin());
    }

    template<typename InputKeyType,
             typename InputValueType>
    constexpr pos_inserted_type emplace_or_assign(InputKeyType && p_key,
                                                  InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");

      auto [key_iter, found] = do_find_raw(p_key);
      auto pos = (key_iter - m_keys.begin());
      auto value_iter = m_values.begin() + pos;

      if(!found)
      {
        std::tie(key_iter, value_iter) = add_empty(key_iter);

        *key_iter = std::forward<InputKeyType>(p_key);
      }

      *value_iter = std::forward<InputValueType>(p_value);

      return pos_inserted_type{static_cast<size_type>(pos), !found};
    }

    template<typename InputKeyType>
    constexpr void erase(InputKeyType && p_key)
    {
      if(auto [key_iter, found] = do_find_raw(p_key);
         found)
      {
        m_keys.erase(key_iter);

        auto pos = static_cast<size_type>(key_iter - m_keys.begin());

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

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      YY_ASSERT(m_keys.size() == m_values.size());

      return m_keys.size();
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

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_keys.empty();
    }

    [[nodiscard]]
    constexpr bool operator<(const flat_map & other) const noexcept
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
        if(m_keys[idx] < other.m_keys[idx])
        {
          return true;
        }

        if(!(m_keys[idx] == other.m_keys[idx]))
        {
          return false;
        }

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
    constexpr bool operator==(const flat_map & other) const noexcept
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
        if(!(m_keys[idx] == other.m_keys[idx]))
        {
          return false;
        }

        if(!(m_values[idx] == other.m_values[idx]))
        {
          return false;
        }
      }

      return size() == other.size();
    }

    template<typename Visitor>
    void visit(Visitor && visitor)
    {
      value_ptr value_iter = m_values.begin();

      for(const auto & key : m_keys)
      {
        visitor(key, *value_iter);
        ++value_iter;
      }
    }

    template<typename Visitor>
    void visit(Visitor && visitor) const
    {
      const_value_ptr value_iter = m_values.begin();

      for(const auto & key : m_keys)
      {
        visitor(key, *value_iter);
        ++value_iter;
      }
    }

  private:
    [[nodiscard]]
    constexpr std::tuple<key_ptr, value_ptr> add_empty(key_ptr p_pos)
    {
      auto [key_pos, key_inserted] = m_keys.add_empty(p_pos);
      if(!key_inserted)
      {
        throw std::runtime_error("flat_map::add_empty() key add_empty() failed!");
      }

      auto [value_pos, value_inserted] = m_values.add_empty(m_values.begin() + (key_pos - m_keys.begin()));
      if(!value_inserted)
      {
        throw std::runtime_error("flat_map::add_empty() value add_empty() failed!");
      }

      return std::make_tuple(key_pos, value_pos);
    }

    [[nodiscard]]
    constexpr key_ptr key(size_type idx) noexcept
    {
      return m_keys.data() + idx;
    }

    [[nodiscard]]
    constexpr const_key_ptr key(size_type idx) const noexcept
    {
      return m_keys.data() + idx;
    }

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
        key_ptr iter = nullptr;
        bool is_end = false;
    };

    struct const_iter_end_type final
    {
        const_key_ptr iter = nullptr;
        bool is_end = false;
    };

    struct iter_found_type final
    {
        key_ptr iter = nullptr;
        bool found = false;
    };

    struct const_iter_found_type final
    {
        const_key_ptr iter = nullptr;
        bool found = false;
    };

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr iter_end_type do_lower_bound_raw(const KeyParamType & p_key) noexcept
    {
      key_ptr begin = m_keys.begin();
      key_ptr end = m_keys.end();
      //key_ptr iter = yy_data::lower_bound(begin, end, p_key);
      key_ptr iter = std::lower_bound(begin, end, p_key);

      return iter_end_type{iter, iter == end};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr const_iter_end_type do_lower_bound_raw(const KeyParamType & p_key) const noexcept
    {
      const_key_ptr begin = m_keys.begin();
      const_key_ptr end = m_keys.end();
      //const_key_ptr iter = yy_data::lower_bound(begin, end, p_key);
      const_key_ptr iter = std::lower_bound(begin, end, p_key);

      return const_iter_end_type{iter, iter == end};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_end_type do_lower_bound(const KeyParamType & p_key) const noexcept
    {
      auto [iter, is_end] = do_lower_bound_raw(p_key);

      return pos_end_type{static_cast<size_type>(iter - m_keys.data()), is_end};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr iter_end_type do_upper_bound_raw(const KeyParamType & p_key) noexcept
    {
      key_ptr begin = m_keys.begin();
      key_ptr end = m_keys.end();
      key_ptr iter = std::upper_bound(begin, end, p_key);

      return iter_end_type{iter, iter == end};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr const_iter_end_type do_upper_bound_raw(const KeyParamType & p_key) const noexcept
    {
      const_key_ptr begin = m_keys.begin();
      const_key_ptr end = m_keys.end();
      const_key_ptr iter = std::upper_bound(begin, end, p_key);

      return const_iter_end_type{iter, iter == end};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_end_type do_upper_bound(const KeyParamType & p_key) const noexcept
    {
      auto [iter, is_end] = do_upper_bound_raw(p_key);

      return pos_end_type{static_cast<size_type>(iter - m_keys.data()), is_end};
    }

    using range = yy_util::Range<key_ptr>;

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr range do_range_raw(const KeyParamType & p_key) noexcept
    {
      auto [begin, begin_is_end] = lower_bound_raw(m_keys.begin(), m_keys.end(), p_key);

      if(begin_is_end)
      {
        return range{begin, begin};
      }

      auto [end, end_is_end] = lower_bound_raw(begin, m_keys.end(), p_key);

      return range{begin, end};
    }

    using const_range = yy_util::Range<const_key_ptr>;

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr range do_range_raw(const KeyParamType & p_key) const noexcept
    {
      auto [begin, begin_is_end] = lower_bound_raw(m_keys.begin(), m_keys.end(), p_key);

      if(begin_is_end)
      {
        return const_range{begin, begin};
      }

      auto [end, end_is_end] = lower_bound_raw(begin, m_keys.end(), p_key);

      return const_range{begin, end};
    }

    using pos_range = yy_util::Range<size_type>;

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_range do_range(const KeyParamType & p_key) const noexcept
    {
      auto range_found = do_range_raw(p_key);
      const_key_ptr begin = m_keys.begin();

      return pos_range{range_found.begin() - begin, range_found.end() - begin()};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr iter_found_type do_find_raw(const KeyParamType & p_key) noexcept
    {
      auto [key_iter, is_end] = do_lower_bound_raw(p_key);

      bool found = !is_end && (*key_iter == p_key);

      return iter_found_type{key_iter, found};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr const_iter_found_type do_find_raw(const KeyParamType & p_key) const noexcept
    {
      auto [key_iter, is_end] = do_lower_bound_raw(p_key);

      bool found = !is_end && (*key_iter == p_key);

      return const_iter_found_type{key_iter, found};
    }

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_found_type do_find(const KeyParamType & p_key) const noexcept
    {
      auto [key_iter, found] = do_find_raw(p_key);

      return pos_found_type{static_cast<size_type>(key_iter - m_keys.data()), found};
    }

    key_vector m_keys;
    value_vector m_values;
};

} // namespace yafiyogi::yy_data
