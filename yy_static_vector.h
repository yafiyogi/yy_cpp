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
#include <cstdint>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "yy_clear_action.h"
#include "yy_compare_util.h"
#include "yy_iterator_ptr.hpp"
#include "yy_ref_traits.h"
#include "yy_span_traits.h"
#include "yy_static_vector_traits.hpp"
#include "yy_type_traits.h"
#include "yy_utility.h"

namespace yafiyogi::yy_quad {

using ClearAction = yy_data::ClearAction;

namespace static_vector_detail {

enum class EmplaceResult:uint8_t {Ok, NotInserted, Full, Part};

template<typename T,
         std::size_t Capacity>
struct vector_traits final
{
    using value_type = yy_traits::remove_cvr_t<T>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using value_ptr = std::add_pointer_t<value_type>;
    using const_value_ptr = std::add_pointer_t<std::add_const_t<value_type>>;
    using reference = std::add_lvalue_reference_t<value_type>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
    static constexpr const size_type vector_capacity = Capacity;

    static constexpr ClearAction default_action = yy_data::default_clear_action_v<value_type>;

};

} // namespace static_vector_detail

template<typename T,
         std::size_t Capacity,
         ClearAction default_action = static_vector_detail::vector_traits<T, Capacity>::default_action>
class static_vector
{
  public:
    using traits = static_vector_detail::vector_traits<T, Capacity>;
    using value_type = typename traits::value_type;
    using value_l_value_ref = typename traits::value_l_value_ref;
    using value_const_l_value_ref = typename traits::value_const_l_value_ref;
    using value_r_value_ref = typename traits::value_r_value_ref;
    using value_ptr = typename traits::value_ptr;
    using const_value_ptr = typename traits::const_value_ptr;
    using reference = typename traits::reference;
    using const_reference = typename traits::const_reference;
    using iterator = yy_data::iterator_detail::iterator_ptr<static_vector>;
    using const_iterator = yy_data::iterator_detail::const_iterator_ptr<static_vector>;

    using EmplaceResult = static_vector_detail::EmplaceResult;
    using vector_type = std::array<value_type, Capacity>;

    struct insert_result final
    {
        iterator iter{};
        EmplaceResult result = EmplaceResult::NotInserted;
    };

    static_assert(std::is_default_constructible_v<value_type>, "T must be default contructable.");
    static_assert(std::is_destructible_v<value_type>, "T must be destructable.");

    constexpr static_vector(std::initializer_list<value_type> p_il) noexcept
    {
      auto l_size = std::min(static_cast<size_type>(std::distance(p_il.begin(), p_il.end())), m_capacity);

      std::move(p_il.begin(), p_il.begin() + l_size, begin());

      m_size = p_il.size();
    }

    template<typename InputIterator>
    constexpr static_vector(InputIterator p_begin,
                            InputIterator p_end) noexcept
    {
      auto l_size = std::min(static_cast<size_type>(std::distance(p_begin, p_end)), m_capacity);

      std::copy(p_begin, p_begin + l_size, begin());

      m_size = l_size;
    }

    constexpr static_vector() noexcept = default;

    constexpr static_vector(const static_vector & other)
    {
      static_assert(std::is_copy_constructible_v<value_type>, "T must be copy constructable.");
      copy(other);
    }

    constexpr static_vector(static_vector && other) noexcept
    {
      static_assert(std::is_move_constructible_v<value_type>, "T must be move constructable.");

      move(std::move(other));
    }

    constexpr ~static_vector() noexcept
    {
      clear(ClearAction::Keep);
    }

    constexpr static_vector & operator=(const static_vector & other)
    {
      static_assert(std::is_copy_assignable_v<value_type>, "T must be copy assignable.");

      copy(other);

      return *this;
    }

    constexpr static_vector & operator=(static_vector && other) noexcept
    {
      move(std::move(other));

      return *this;
    }

    [[nodiscard]]
    constexpr bool operator<(const static_vector & other) const noexcept
    {
      return yy_util::less_than(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator<(const type & a,
                                    const static_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(a, b);
    }

    [[nodiscard]]
    constexpr bool operator==(const static_vector & other) const noexcept
    {
      return yy_util::equal(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator==(const type & a,
                                     const static_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(a, b);
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept
    {
      return iterator{this, 0};
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    {
      return const_iterator{this, 0};
    }

    [[nodiscard]]
    constexpr iterator end() noexcept
    {
      return iterator{this, size()};
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    {
      return const_iterator{this, size()};
    }

    [[nodiscard]]
    constexpr value_type & operator[](size_type pos) noexcept
    {
      return *(begin() + static_cast<ssize_type>(pos));
    }

    [[nodiscard]]
    constexpr const value_type & operator[](size_type pos) const noexcept
    {
      return *(begin() + static_cast<ssize_type>(pos));
    }

    [[nodiscard]]
    constexpr value_type & back()
    {
      if(begin() == end())
      {
        throw std::out_of_range("static_vector<T>::back(): static_vector is empty!");
      }

      return *(end() - 1);
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref back() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("static_vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    [[nodiscard]]
    constexpr value_type & front()
    {
      if(begin() == end())
      {
        throw std::out_of_range("static_vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref front() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("static_vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    [[nodiscard]]
    constexpr value_ptr data() noexcept
    {
      return raw_data() + m_offset;
    }

    [[nodiscard]]
    constexpr const_value_ptr data() const noexcept
    {
      return raw_data() + m_offset;
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      return m_size - m_offset;
    }

    [[nodiscard]]
    constexpr size_type capacity() const noexcept
    {
      return m_capacity;
    }

    [[nodiscard]]
    constexpr size_type offset() const noexcept
    {
      return m_offset;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return 0 == size();
    }

    [[nodiscard]]
    constexpr insert_result add_empty(iterator pos)
    {
      if(m_capacity == m_size)
      {
        return insert_result{end(), EmplaceResult::Full};
      }

      auto iter = end();
      auto result = EmplaceResult::NotInserted;

      auto [distance, valid] = distance_valid(pos, m_size + 1);

      if(valid)
      {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic ignored "-Wnonnull" // for g++ 12.3
#endif
        std::move_backward(pos, end(), end() + 1);

        iter = begin() + distance;
        result = EmplaceResult::Ok;
        ++m_size;
      }

      return insert_result{iter, result};
    }

    constexpr insert_result emplace(iterator pos, const value_type & value)
    {
      insert_result result{add_empty(pos)};

      if(EmplaceResult::Ok == result.result)
      {
        *result.iter = value;
      }

      return result;
    }

    constexpr insert_result emplace(iterator pos, value_type && value)
    {
      insert_result result = add_empty(pos);

      if(EmplaceResult::Ok == result.result)
      {
        *result.iter = std::move(value);
      }

      return result;
    }

    template<typename ...Args>
    constexpr insert_result emplace(iterator pos, Args && ...args)
    {
      insert_result result = add_empty(pos);

      if(EmplaceResult::Ok == result.result)
      {
        *result.iter = value_type{std::forward<Args>(args)...};
      }

      return result;
    }

    constexpr EmplaceResult emplace_back(const value_type & value)
    {
      auto [ignore, result] = emplace(end(), value);

      return result;
    }

    constexpr EmplaceResult emplace_back(value_type && value)
    {
      auto [ignore, result] = emplace(end(), std::move(value));

      return result;
    }

    template<typename ...Args>
    constexpr EmplaceResult emplace_back(Args && ...args)
    {
      auto [ignore, result] = emplace(end(), std::forward<Args>(args)...);

      return result;
    }

    constexpr bool erase(iterator pos, ClearAction action = default_action) noexcept
    {
      bool erased = false;
      if(!empty())
      {
        auto [distance, valid] = distance_valid(pos, m_size);

        if(valid)
        {
          erased = true;
          if(distance == (m_size - 1))
          {
            // Erase end element.
            if(ClearAction::Clear == action)
            {
              *pos = value_type{};
            }
            --m_size;

            reset_offset_size();
          }
          else if(distance == 0)
          {
            // Erase first element.
            if(ClearAction::Clear == action)
            {
              *pos = value_type{};
            }
            ++m_offset;

            reset_offset_size();
          }
          else
          {
            // The element is in the middle.
            if(ClearAction::Clear == action)
            {
              std::move(pos + 1, end(), pos);
            }
            --m_size;
          }
        }
      }
      return erased;
    }

    constexpr bool erase(iterator p_begin,
                         iterator p_end,
                         ClearAction action = default_action) noexcept
    {
      bool erased = false;
      if(!empty())
      {
        ssize_type distance = p_begin - begin();
        if(distance < 0)
        {
          p_begin = begin();
        }
        else if(static_cast<size_type>(distance) > m_size)
        {
          p_begin = end();
        }

        distance = p_end - p_begin;
        if(distance < 0)
        {
          p_end = p_begin;
        }
        else if(static_cast<size_type>(p_end - begin()) > m_size)
        {
          p_end = end();
        }

        distance = p_end - p_begin;
        if(0 != distance)
        {
          erased = true;

          if(begin() == p_begin)
          {
            m_offset += static_cast<size_type>(distance);
            if(ClearAction::Clear == action)
            {
              for(auto range{yy_util::make_range<iterator>(p_begin, p_end)};
                  auto & item : range)
              {
                item = value_type{};
              }
            }
          }
          else
          {
            std::move(p_end, end(), p_begin);
            if(ClearAction::Clear == action)
            {
              for(auto range{yy_util::make_range<iterator>(p_end, end())};
                  auto & item : range)
              {
                item = value_type{};
              }
            }
            m_size -= static_cast<size_type>(distance);
          }

          reset_offset_size();
        }
      }
      return erased;
    }

    constexpr void pop_back(ClearAction action = default_action) noexcept
    {
      if(!empty())
      {
        erase(end() - 1, end(), action);
      }
    }

    constexpr void pop_front(ClearAction action = default_action) noexcept
    {
      if(!empty())
      {
        erase(begin(), begin() + 1, action);
      }
    }

    constexpr void copy(const static_vector & other, ClearAction action = default_action)
    {
      if(this != &other)
      {
        // If other.size() > m_size, don't keep the elements as they will
        // all be overwritten.

        // If other.size() > m_capacity, don't keep the elements as they will
        // be freed when the buffer is deallocated.
        size_type start_of_clear = 0;
        ClearAction clear_action = ClearAction::Keep;

        // Delete the elements which are not overwritten.
        if(other.size() < size())
        {
          start_of_clear = other.size();
          clear_action = action;
        }

        clear(start_of_clear, clear_action);

        m_size = other.size();
        m_offset = 0;

        // Copy elements from othe vector.
        if(!other.empty())
        {
          std::copy(other.begin(), other.end(), begin());
        }
      }
    }

    constexpr void clear(ClearAction action = default_action) noexcept
    {
      clear(0, action);
    }

    constexpr void swap(static_vector & other) noexcept
    {
      if(this != &other)
      {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_offset, other.m_offset);
      }
    }

    constexpr void swap(static_vector && other,
                        ClearAction action = default_action) noexcept
    {
      move(std::move(other), action);
    }

  private:
    constexpr void move(static_vector && other,
                        ClearAction action = default_action) noexcept
    {
      if(this != &other)
      {
        std::move(other.begin(), other.end(), begin());

        if(ClearAction::Clear ==  action)
        {
          for(size_type idx = other.m_size; idx < m_size; ++idx)
          {
            m_data[idx] = value_type{};
          }
        }

        m_size = std::move(other.m_size);
        other.m_size = 0;
        m_offset = std::move(other.m_offset);
        other.m_offset = 0;
      }
    }

    [[nodiscard]]
    constexpr value_ptr raw_data() noexcept
    {
      return m_data.data();
    }

    [[nodiscard]]
    constexpr const_value_ptr raw_data() const noexcept
    {
      return m_data.data();
    }

    constexpr void reset_offset_size() noexcept
    {
      if(m_offset == m_size)
      {
        // The vector is empty so reset.
        m_offset = 0;
        m_size = 0;
      }
    }

    struct distance_valid_type final
    {
        size_type distance = 0;
        bool valid = false;
    };

    constexpr distance_valid_type distance_valid(const iterator pos, size_type max) const noexcept
    {
      max = std::min(max, m_capacity);

      const ssize_type distance = const_iterator{pos} - begin();

      return distance_valid_type{distance,
                                 (distance >= 0) && (static_cast<size_type>(distance) < max)};
    }

    constexpr void clear(size_type start, ClearAction action = default_action) noexcept
    {
      if(!empty() && (ClearAction::Clear == action))
      {
        for(auto range{yy_util::make_range(begin() + static_cast<ssize_type>(start), end())};
            auto & element : range)
        {
          element = value_type{};
        }
      }

      // The vector is empty so reset.
      m_size = 0;
      m_offset = 0;
    }

    vector_type m_data;
    size_type m_size = 0;
    size_type m_offset = 0;
    static constexpr const size_type m_capacity = traits::vector_capacity;
};

template<typename T,
         std::size_t Capacity,
         ClearAction default_action = static_vector_detail::vector_traits<T, Capacity>::default_action>
class static_simple_vector
{
  public:
    using traits = static_vector_detail::vector_traits<T, Capacity>;
    using value_type = typename traits::value_type;
    using value_l_value_ref = typename traits::value_l_value_ref;
    using value_const_l_value_ref = typename traits::value_const_l_value_ref;
    using value_r_value_ref = typename traits::value_r_value_ref;
    using value_ptr = typename traits::value_ptr;
    using const_value_ptr = typename traits::const_value_ptr;
    using reference = typename traits::reference;
    using const_reference = typename traits::const_reference;
    using iterator = yy_data::iterator_detail::iterator_ptr<static_simple_vector>;
    using const_iterator = yy_data::iterator_detail::const_iterator_ptr<static_simple_vector>;

    using EmplaceResult = static_vector_detail::EmplaceResult;
    using vector_type = std::array<value_type, Capacity>;

    struct insert_result final
    {
        iterator iter{};
        EmplaceResult result = EmplaceResult::NotInserted;
    };

    static_assert(std::is_default_constructible_v<value_type>, "T must be default contructable.");
    static_assert(std::is_destructible_v<value_type>, "T must be destructable.");

    constexpr static_simple_vector(std::initializer_list<value_type> p_il) noexcept
    {
      auto l_size = std::min(static_cast<size_type>(std::distance(p_il.begin(), p_il.end())), m_capacity);

      std::move(p_il.begin(), p_il.begin() + l_size, begin());

      m_size = p_il.size();
    }

    template<typename InputIterator>
    constexpr static_simple_vector(InputIterator p_begin, InputIterator p_end) noexcept
    {
      auto l_size = std::min(static_cast<size_type>(std::distance(p_begin, p_end)), m_capacity);

      std::copy(p_begin, p_begin + l_size, begin());

      m_size = l_size;
    }

    constexpr static_simple_vector() noexcept = default;

    constexpr static_simple_vector(const static_simple_vector & other)
    {
      static_assert(std::is_copy_constructible_v<value_type>, "T must be copy constructable.");
      copy(other);
    }

    constexpr static_simple_vector(static_simple_vector && other) noexcept
    {
      static_assert(std::is_move_constructible_v<value_type>, "T must be move constructable.");

      move(std::move(other));
    }

    constexpr ~static_simple_vector() noexcept
    {
      clear(ClearAction::Keep);
    }

    constexpr static_simple_vector & operator=(const static_simple_vector & other)
    {
      static_assert(std::is_copy_assignable_v<value_type>, "T must be copy assignable.");

      copy(other);

      return *this;
    }

    constexpr static_simple_vector & operator=(static_simple_vector && other) noexcept
    {
      move(std::move(other));

      return *this;
    }

    [[nodiscard]]
    constexpr bool operator<(const static_simple_vector & other) const noexcept
    {
      return yy_util::less_than(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator<(const type & a,
                                    const static_simple_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(a, b);
    }

    [[nodiscard]]
    constexpr bool operator==(const static_simple_vector & other) const noexcept
    {
      return yy_util::equal(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(*this, other);
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator==(const type & a,
                                     const static_simple_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(a, b);
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept
    {
      return iterator{this, 0};
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    {
      return const_iterator{this, 0};
    }

    [[nodiscard]]
    constexpr iterator end() noexcept
    {
      return iterator{this, size()};
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    {
      return const_iterator{this, size()};
    }

    [[nodiscard]]
    constexpr value_type & operator[](size_type pos) noexcept
    {
      return *(begin() + static_cast<ssize_type>(pos));
    }

    [[nodiscard]]
    constexpr const value_type & operator[](size_type pos) const noexcept
    {
      return *(begin() + static_cast<ssize_type>(pos));
    }

    [[nodiscard]]
    constexpr value_type & back()
    {
      if(begin() == end())
      {
        throw std::out_of_range("static_simple_vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref back() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("static_simple_vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    [[nodiscard]]
    constexpr value_type & front()
    {
      if(begin() == end())
      {
        throw std::out_of_range("static_simple_vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref front() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("static_simple_vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    [[nodiscard]]
    constexpr value_ptr data() noexcept
    {
      return m_data.data();
    }

    [[nodiscard]]
    constexpr const_value_ptr data() const noexcept
    {
      return m_data.data();
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept
    {
      return m_size;
    }

    [[nodiscard]]
    constexpr size_type capacity() const noexcept
    {
      return m_capacity;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return 0 == size();
    }

    [[nodiscard]]
    constexpr insert_result add_empty(iterator pos)
    {
      if(m_capacity == m_size)
      {
        return insert_result{end(), EmplaceResult::Full};
      }

      auto iter = end();
      auto result = EmplaceResult::NotInserted;

      auto [distance, valid] = distance_valid(pos, m_size + 1);

      if(valid)
      {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic ignored "-Wnonnull" // for g++ 12.3
#pragma GCC diagnostic ignored "-Warray-bounds" // for g++ 12.3
#endif
        std::move_backward(pos, end(), end() + 1);

        iter = begin() + distance;
        result = EmplaceResult::Ok;
        ++m_size;
      }

      return insert_result{iter, result};
    }

    constexpr insert_result emplace(iterator pos,
                                    const value_type & value)
    {
      insert_result result{add_empty(pos)};

      if(EmplaceResult::Ok == result.result)
      {
        *result.iter = value;
      }

      return result;
    }

    constexpr insert_result emplace(iterator pos,
                                    value_type && value)
    {
      insert_result result{add_empty(pos)};

      if(EmplaceResult::Ok == result.result)
      {
        *result.iter = std::move(value);
      }

      return result;
    }

    template<typename ...Args>
    constexpr insert_result emplace(iterator pos,
                                    Args && ...args)
    {
      insert_result result{add_empty(pos)};

      if(EmplaceResult::Ok == result.result)
      {
        *result.iter = value_type{std::forward<Args>(args)...};
      }

      return result;
    }

    constexpr EmplaceResult emplace_back(const value_type & value)
    {
      auto [ignore, result] = emplace(end(), value);

      return result;
    }

    constexpr EmplaceResult emplace_back(value_type && value)
    {
      auto [ignore, result] = emplace(end(), std::move(value));

      return result;
    }

    template<typename ...Args>
    constexpr EmplaceResult emplace_back(Args && ...args)
    {
      auto [ignore, result] = emplace(end(), std::forward<Args>(args)...);

      return result;
    }

    constexpr EmplaceResult push_back(const value_type & value)
    {
      auto [ignore, result] = emplace(end(), value);

      return result;
    }

    constexpr EmplaceResult push_back(value_type && value)
    {
      auto [ignore, result] = emplace(end(), std::move(value));

      return result;
    }

    constexpr bool erase(iterator pos) noexcept
    {
      bool erased = false;
      if(!empty())
      {
        auto [ignored, valid] = distance_valid(pos, m_size);

        if(valid)
        {
          erased = true;

#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow" // for g++ 12.3
#endif
          std::move(pos + 1, end(), pos);
          --m_size;
        }
      }
      return erased;
    }

    constexpr bool erase(iterator p_begin,
                         iterator p_end,
                         ClearAction action = default_action) noexcept
    {
      bool erased = false;
      if(!empty())
      {
        ssize_type distance = p_begin - begin();
        if(distance < 0)
        {
          p_begin = begin();
        }
        else if(static_cast<size_type>(distance) > m_size)
        {
          p_begin = end();
        }

        distance = p_end - p_begin;
        if(distance < 0)
        {
          p_end = p_begin;
        }
        else if(static_cast<size_type>(p_end - begin()) > m_size)
        {
          p_end = end();
        }

        distance = p_end - p_begin;
        if(0 != distance)
        {
          erased = true;

          std::move(p_end, end(), p_begin);
          if(ClearAction::Clear == action)
          {
            for(auto range{yy_util::make_range<iterator>(p_end, end())};
                auto & item : range)
            {
              item = value_type{};
            }
          }
          m_size -= static_cast<size_type>(distance);
        }
      }
      return erased;
    }

    constexpr void pop_back(ClearAction action = default_action) noexcept
    {
      if(!empty())
      {
        erase(end() - 1, end(), action);
      }
    }

    constexpr void pop_front(ClearAction action = default_action) noexcept
    {
      if(!empty())
      {
        erase(begin(), begin() + 1, action);
      }
    }

    constexpr void copy(const static_simple_vector & other, ClearAction action = default_action)
    {
      if(this != &other)
      {
        // If other.size() > m_size, don't keep the elements as they will
        // all be overwritten.

        // If other.size() > m_capacity, don't keep the elements as they will
        // be freed when the buffer is deallocated.
        size_type start_of_clear = 0;
        ClearAction clear_action = ClearAction::Keep;

        // Delete the elements which are not overwritten.
        if(other.size() < size())
        {
          start_of_clear = other.size();
          clear_action = action;
        }

        clear(start_of_clear, clear_action);

        m_size = other.size();

        // Copy elements from othe static_simple_vector.
        if(!other.empty())
        {
          std::copy(other.begin(), other.end(), begin());
        }
      }
    }

    void assign(const static_simple_vector & other,
                ClearAction action = default_action)
    {
      copy(other, action);
    }

    void assign(static_simple_vector && other)
    {
      move(std::move(other));
    }

    void assign(yy_quad::const_span<value_type> other,
                ClearAction action = default_action)
    {
      auto other_data = other.data();
      auto other_size = other.size();
      auto this_data = data();
      auto this_size = size();

      if(((other_data < this_data)
          || (other_data > (this_data + this_size)))
         && (((other_data + other_size) < this_data)
             || ((other_data + other_size) > (this_data + this_size))))

      {
        clear(action);
        reserve(other_size);

        std::copy(other.begin(), other.end(), begin());
        m_size = other_size;
      }
    }

    EmplaceResult append(const yy_quad::const_span<value_type> & other)
    {
      size_type num = std::min(other.size(), m_capacity - m_size);
      EmplaceResult result = EmplaceResult::Ok;

      if(num > 0)
      {
        if(other.size() != num)
        {
          result = EmplaceResult::Part;
        }
        std::copy(other.begin(), other.begin() + num, end());
        m_size += num;
      }

      return result;
    }

    EmplaceResult append(static_simple_vector && other)
    {
      size_type num = std::min(other.size(), m_capacity - m_size);
      EmplaceResult result = EmplaceResult::Ok;

      if(num > 0)
      {
        if(other.size() != num)
        {
          result = EmplaceResult::Part;
        }
        std::move(other.begin(), other.begin() + num, end());
        m_size += num;
      }

      return result;
    }

    constexpr void clear(ClearAction action = default_action) noexcept
    {
      clear(0, action);
    }

    constexpr void swap(static_simple_vector & other) noexcept
    {
      if(this != &other)
      {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
      }
    }

    constexpr void swap(static_simple_vector && other,
                        ClearAction action = default_action) noexcept
    {
      move(std::move(other), action);
    }

  private:
    constexpr void move(static_simple_vector && other,
                        ClearAction action = default_action) noexcept
    {
      if(this != &other)
      {
        std::move(other.begin(), other.end(), begin());

        if(ClearAction::Clear ==  action)
        {
          for(size_type idx = other.m_size; idx < m_size; ++idx)
          {
            m_data[idx] = value_type{};
          }
        }

        m_size = std::move(other.m_size);
        other.m_size = 0;
      }
    }

    struct distance_valid_type final
    {
        ssize_type distance = 0;
        bool valid = false;
    };

    [[nodiscard]]
    constexpr distance_valid_type distance_valid(const iterator pos, size_type max) const noexcept
    {
      max = std::min(max, m_capacity);
      const ssize_type distance = const_iterator{pos} - begin();

      return distance_valid_type{distance,
                                 (distance >= 0) && (static_cast<size_type>(distance) < max)};
    }

    constexpr void clear(size_type start, ClearAction action = default_action) noexcept
    {
      if(!empty() && (ClearAction::Clear == action))
      {
        for(auto range{yy_util::make_range(begin() + static_cast<ssize_type>(start), end())};
            auto & element : range)
        {
          element = value_type{};
        }
      }

      // The vector is empty so reset.
      m_size = 0;
    }

        vector_type m_data{};
    size_type m_size = 0;
    static constexpr const size_type m_capacity = traits::vector_capacity;
};

} // namespace yafiyogi::yy_quad
