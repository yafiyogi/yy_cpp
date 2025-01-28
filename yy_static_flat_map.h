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
#include <tuple>
#include <type_traits>

#include "yy_assert.h"
#include "yy_clear_action.h"
#include "yy_find_iter_util.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_static_vector.h"

namespace yafiyogi::yy_data {
namespace static_flat_map_detail {

template<typename Key,
         typename Value,
         std::size_t Capacity,
         ClearAction KeyClearAction,
         ClearAction ValueClearAction>
struct traits_type final
{
    using key_type = yy_traits::remove_cvr_t<Key>;
    using key_vector = yy_quad::static_simple_vector<key_type, Capacity, KeyClearAction>;
    using key_ptr = key_vector::value_ptr;
    using const_key_ptr = key_vector::const_value_ptr;
    using key_l_value_ref = typename yy_traits::ref_traits<key_type>::l_value_ref;
    using key_r_value_ref = typename yy_traits::ref_traits<key_type>::r_value_ref;
    using value_type = yy_traits::remove_cvr_t<Value>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using value_vector = yy_quad::static_simple_vector<value_type, Capacity, ValueClearAction>;
    using value_ptr = value_vector::value_ptr;
    using const_value_ptr = value_vector::const_value_ptr;
    using EmplaceResult = yy_quad::static_vector_detail::EmplaceResult;

    static constexpr const size_type capacity = Capacity;
};

} // namespace static_flat_map_detail


template<typename Key,
         typename Value,
         std::size_t Capacity,
         ClearAction KeyClearAction = default_clear_action_v<Key>,
         ClearAction ValueClearAction = default_clear_action_v<Value>,
         std::size_t SearchSizeThreshold = ((yy_data::find_iter_pos_size_threshold_cache_line_size * 2) / sizeof(Key)) + 1>
class static_flat_map final
{
  public:
    using traits = static_flat_map_detail::traits_type<Key, Value, Capacity, KeyClearAction, ValueClearAction>;
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
    using key_iterator = typename traits::key_vector::iterator;
    using value_vector = typename traits::value_vector;
    using value_iterator = typename traits::value_vector::iterator;
    using EmplaceResult = typename traits::EmplaceResult;

    template<typename KeyParamType>
    using key_param_traits = yy_traits::ref_traits<KeyParamType>;
    template<typename KeyParamType>
    using key_param_l_value_ref = typename key_param_traits<KeyParamType>::l_value_ref;

    template<typename key_type,
             typename value_type>
    constexpr static_flat_map(std::initializer_list<std::tuple<key_type, value_type>> init)
    {
      for(auto & [key, value] : init)
      {
        if(EmplaceResult::Full == emplace(std::move(key), std::move(value)))
        {
          break;
        }
      }
    }

    constexpr static_flat_map() noexcept = default;
    constexpr static_flat_map(const static_flat_map &) noexcept = default;
    constexpr static_flat_map(static_flat_map &&) noexcept = default;
    constexpr ~static_flat_map() noexcept = default;

    constexpr static_flat_map & operator=(const static_flat_map &) noexcept = default;
    constexpr static_flat_map & operator=(static_flat_map &&) noexcept = default;

    using pos_end_type = find_iter_util_detail::pos_end_type<size_type>;

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr pos_end_type lower_bound_pos(const KeyParamType & p_key) const noexcept
    {
      return do_lower_bound(m_keys, p_key);
    }

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr bool lower_bound(Visitor && visitor,
                               const KeyParamType & p_key) noexcept
    {
      auto [pos, is_end] = do_lower_bound(m_keys, p_key);

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
      auto [pos, is_end] = do_lower_bound(m_keys, p_key);

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
        size_type pos = 0;
    };

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr key_value_pos_type find(const KeyParamType & p_key) noexcept
    {
      auto [pos, found] = find_iter_pos(m_keys.begin(), m_keys.end(), p_key, SearchSizeThreshold);

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
        size_type pos = 0;
    };

    template<typename KeyParamType>
    [[nodiscard]]
    constexpr const_key_value_pos_type find(const KeyParamType & p_key) const noexcept
    {
      auto [pos, found] = find_iter_pos(m_keys.begin(), m_keys.end(), p_key, SearchSizeThreshold);

      if(found)
      {
        return const_key_value_pos_type{key(pos), value(pos), pos};
      }

      return const_key_value_pos_type{nullptr, nullptr, pos};
    }

    using pos_found_type = find_iter_util_detail::pos_found_type<size_type>;

    template<typename KeyParamType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const KeyParamType & p_key) noexcept
    {
      pos_found_type pos_found{find_iter_pos(m_keys.begin(), m_keys.end(), p_key, SearchSizeThreshold)};

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
      pos_found_type pos_found{find_iter_pos(m_keys.begin(), m_keys.end(), p_key, SearchSizeThreshold)};

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
      return do_find(m_keys, p_key);
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

    struct pos_result_type final
    {
        size_type pos = 0;
        EmplaceResult result = EmplaceResult::Full;
    };

    template<typename InputValueType>
    constexpr pos_result_type emplace(size_type p_pos,
                                      key_r_value_ref p_key,
                                      InputValueType && p_value) noexcept
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");

      auto [key, value, result] = add_empty(m_keys.begin() + p_pos);

      if(EmplaceResult::Full == result)
      {
        return pos_result_type{size(), EmplaceResult::Full};
      }

      if(EmplaceResult::Ok)
      {
        key = std::forward<key_type>(p_key);
        value = std::forward<InputValueType>(p_value);
      }

      return pos_result_type{static_cast<size_type>(&key - m_keys.begin()), result};
    }


    template<typename InputKeyType,
             typename InputValueType>
    constexpr pos_result_type emplace(InputKeyType && p_key,
                                      InputValueType && p_value) noexcept
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputKeyType>, key_type>
                    || (std::is_pointer_v<InputKeyType> && std::is_base_of_v<key_type, yy_traits::remove_cvr_t<std::remove_pointer<InputKeyType>>>),
                    "p_key is of an incompatible type.");
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "p_value is of an incompatible type.");


      auto [key_iter, key_found] = find_iter(m_keys.begin(), m_keys.end(), p_key);
      auto result = EmplaceResult::NotInserted;
      auto pos = static_cast<size_type>(key_iter - m_keys.begin());

      if(!key_found)
      {
        auto [key, value, add_result] = add_empty(key_iter);
        result = add_result;

        pos = static_cast<size_type>(key - m_keys.begin());

        if(EmplaceResult::Ok == result)
        {
          *key = std::forward<InputKeyType>(p_key);
          *value = std::forward<InputValueType>(p_value);
        }
      }

      return pos_result_type{pos, result};
    }

    constexpr void swap(static_flat_map & other) noexcept
    {
      if(this != &other)
      {
        std::swap(m_keys, other.m_keys);
        std::swap(m_values, other.m_values);
      }
    }

    constexpr void swap(static_flat_map && other) noexcept
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
    constexpr bool operator<(const static_flat_map & other) const noexcept
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

        if(m_keys[idx] != other.m_keys[idx])
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
    constexpr bool operator==(const static_flat_map & other) const noexcept
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

      for(auto & key : m_keys)
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
    struct add_empty_type final
    {
        key_iterator key = nullptr;
        value_iterator value = nullptr;
        EmplaceResult result = EmplaceResult::Full;
    };

    [[nodiscard]]
    constexpr add_empty_type add_empty(key_iterator p_pos) noexcept
    {
      if(m_capacity == m_size)
      {
        return add_empty_type{m_keys.end(), m_values.end(), EmplaceResult::Full};
      }

      if(auto [key_pos, key_result] = m_keys.add_empty(p_pos);
         EmplaceResult::Ok == key_result)
      {
        if(auto [value_pos, value_result] = m_values.add_empty(m_values.begin() + (key_pos - m_keys.begin()));
           EmplaceResult::Ok == value_result)
        {
          return add_empty_type{key_pos, value_pos, EmplaceResult::Ok};
        }
      }

      return add_empty_type{m_keys.end(), m_values.end(), EmplaceResult::NotInserted};
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
    constexpr value_type * value(size_type idx) noexcept
    {
      return m_values.data() + idx;
    }

    [[nodiscard]]
    constexpr const value_type * value(size_type idx) const noexcept
    {
      return m_values.data() + idx;
    }

    key_vector m_keys{};
    value_vector m_values{};
    size_type m_size = 0;
    static constexpr const size_type m_capacity = traits::capacity;
};

} // namespace yafiyogi::yy_data
