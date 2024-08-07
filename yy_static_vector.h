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
#include <cstdint>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "yy_clear_action.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_utility.h"
#include "yy_vector_traits.h"

namespace yafiyogi {
namespace yy_quad {

using ClearAction = yy_data::ClearAction;

namespace static_vector_detail {

enum class EmplaceResult:uint8_t {Ok, NotInserted, Full};

template<typename T,
         std::size_t Capacity>
struct vector_traits final
{
    using value_type = yy_traits::remove_cvr_t<T>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using vector_type = std::array<value_type, Capacity>;
    using size_type = vector_type::size_type;
    static constexpr const size_type vector_capacity = Capacity;
    using value_ptr = vector_type::pointer;
    using const_value_ptr = vector_type::const_pointer;
    using iterator = value_ptr;
    using const_iterator = const_value_ptr;
    using distance_type = std::ptrdiff_t;
    struct return_value
    {
        iterator iter{};
        EmplaceResult result = EmplaceResult::NotInserted;
    };

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
    using iterator = typename traits::iterator;
    using const_iterator = typename traits::const_iterator;
    using size_type = typename traits::size_type;
    using distance_type = typename traits::distance_type;
    using EmplaceResult = static_vector_detail::EmplaceResult;
    using return_value = typename traits::return_value;
    using vector_type = typename traits::vector_type;

    static_assert(std::is_default_constructible_v<value_type>, "T must be default contructable.");
    static_assert(std::is_destructible_v<value_type>, "T must be destructable.");

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

    constexpr static_vector(std::initializer_list<value_type> p_il) noexcept
    {
      for(auto & item : p_il)
      {
        if(EmplaceResult::Full == emplace_back(std::move(item)))
        {
          break;
        }
      }
    }

    constexpr ~static_vector() noexcept
    {
      clear(ClearAction::Keep);
    }

    constexpr static_vector & operator=(const static_vector & other)
    {
      static_assert(std::is_copy_assignable_v<value_type>, "T must be copy assignable.");

      if(this != &other)
      {
        copy(other);
      }

      return *this;
    }

    constexpr static_vector & operator=(static_vector && other) noexcept
    {
      if(this != &other)
      {
        move(std::move(other));
      }

      return *this;
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(begin(), end(),
                                          other.data(), other.data() + other.size());
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator<(const type & a,
                                    const static_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(a.data(), a.data() + a.size(),
                                          b.begin(), b.end());
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (size() == other.size())
        && std::equal(begin(), end(), other.data());
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator==(const type & a,
                                     const static_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (a.size() == b.size())
        && std::equal(a.data(), a.data() + a.size(), b.begin());
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept
    {
      return raw_data() + m_offset;
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    {
      return raw_data() + m_offset;
    }

    [[nodiscard]]
    constexpr iterator end() noexcept
    {
      return raw_data() + m_size;
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    {
      return raw_data() + m_size;
    }

    constexpr value_type & operator[](size_type pos) noexcept
    {
      return *(begin() + pos);
    }

    constexpr const value_type & operator[](size_type pos) const noexcept
    {
      return *(begin() + pos);
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
    constexpr return_value add_empty(value_ptr pos)
    {
      if(m_capacity == m_size)
      {
        return return_value{end(), EmplaceResult::Full};
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

      return return_value{iter, result};
    }

    template<typename InputValueType>
    constexpr return_value emplace(iterator pos, InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      return_value rv = add_empty(pos);

      if(EmplaceResult::Ok == rv.result)
      {
        *rv.iter = std::forward<InputValueType>(value);
      }

      return rv;
    }

    template<typename InputValueType>
    constexpr EmplaceResult emplace_back(InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      auto [ignore, result] = emplace(end(), std::forward<InputValueType>(value));

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
        distance_type distance = p_begin - begin();
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
      if(this != &other)
      {
        move(std::move(other), action);
      }
    }

  private:
    constexpr void move(static_vector && other,
                        ClearAction action = default_action) noexcept
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
        size_type distance{};
        bool valid = false;
    };

    constexpr distance_valid_type distance_valid(const_iterator pos, size_type max) const noexcept
    {
      const auto distance = std::distance(begin(), pos);

      max = std::min(max, m_capacity);

      return distance_valid_type{static_cast<size_type>(distance),
                                 (distance >= 0) && (static_cast<size_type>(distance) < max)};

    }

    constexpr void clear(size_type start, ClearAction action = default_action) noexcept
    {
      if(!empty() && (ClearAction::Clear == action))
      {
        for(auto range{yy_util::make_range(begin() + start, end())};
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
    size_type m_size{};
    size_type m_offset{};
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
    using iterator = typename traits::iterator;
    using const_iterator = typename traits::const_iterator;
    using size_type = typename traits::size_type;
    using distance_type = typename traits::distance_type;
    using EmplaceResult = static_vector_detail::EmplaceResult;
    using return_value = typename traits::return_value;
    using vector_type = typename traits::vector_type;

    static_assert(std::is_default_constructible_v<value_type>, "T must be default contructable.");
    static_assert(std::is_destructible_v<value_type>, "T must be destructable.");

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

    constexpr static_simple_vector(std::initializer_list<value_type> p_il) noexcept
    {
      reserve(p_il.size());
      for(auto & item : p_il)
      {
        if(EmplaceResult::Full == emplace_back(std::move(item)))
        {
          break;
        }
      }
    }

    constexpr ~static_simple_vector() noexcept
    {
      clear(ClearAction::Keep);
    }

    constexpr static_simple_vector & operator=(const static_simple_vector & other)
    {
      static_assert(std::is_copy_assignable_v<value_type>, "T must be copy assignable.");

      if(this != &other)
      {
        copy(other);
      }

      return *this;
    }

    constexpr static_simple_vector & operator=(static_simple_vector && other) noexcept
    {
      if(this != &other)
      {
        move(std::move(other));
      }

      return *this;
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(begin(), end(),
                                          other.data(), other.data() + other.size());

    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator<(const type & a,
                                    const static_simple_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(a.data(), a.data() + a.size(),
                                          b.begin(), b.end());
    }

    template<typename type>
    [[nodiscard]]
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (size() == other.size())
        && std::equal(begin(), end(), other.data());
    }

    template<typename type>
    [[nodiscard]]
    friend constexpr bool operator==(const type & a,
                                     const static_simple_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (a.size() == b.size())
        && std::equal(a.data(), a.data() + a.size(), b.begin());
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept
    {
      return data();
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    {
      return data();
    }

    [[nodiscard]]
    constexpr iterator end() noexcept
    {
      return data() + m_size;
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    {
      return data() + m_size;
    }

    [[nodiscard]]
    constexpr value_type & operator[](size_type pos) noexcept
    {
      return *(begin() + pos);
    }

    [[nodiscard]]
    constexpr const value_type & operator[](size_type pos) const noexcept
    {
      return *(begin() + pos);
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
    constexpr return_value add_empty(value_ptr pos)
    {
      if(m_capacity == m_size)
      {
        return return_value{end(), EmplaceResult::Full};
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

      return return_value{iter, result};
    }

    template<typename InputValueType>
    constexpr return_value emplace(iterator pos, InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      return_value rv = add_empty(pos);

      if(EmplaceResult::Ok == rv.result)
      {
        *rv.iter = std::forward<InputValueType>(value);
      }

      return rv;
    }

    template<typename InputValueType>
    constexpr EmplaceResult emplace_back(InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_cvr_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      auto [ignore, result] = emplace(end(), std::forward<InputValueType>(value));

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
        distance_type distance = p_begin - begin();
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
      if(this != &other)
      {
        move(std::move(other), action);
      }
    }

  private:
    constexpr void move(static_simple_vector && other,
                        ClearAction action = default_action) noexcept
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

    struct distance_valid_type final
    {
        size_type distance{};
        bool valid = false;
    };

    [[nodiscard]]
    constexpr distance_valid_type distance_valid(const_iterator pos, size_type max) const noexcept
    {
      const auto distance = std::distance(begin(), pos);

      max = std::min(max, m_capacity);

      return distance_valid_type{static_cast<size_type>(distance),
                                 (distance >= 0) && (static_cast<size_type>(distance) < max)};

    }

    constexpr void clear(size_type start, ClearAction action = default_action) noexcept
    {
      if(!empty() && (ClearAction::Clear == action))
      {
        for(auto range{yy_util::make_range(begin() + start, end())};
            auto & element : range)
        {
          element = value_type{};
        }
      }

      // The vector is empty so reset.
      m_size = 0;
    }

    vector_type m_data;
    size_type m_size{};
    static constexpr const size_type m_capacity = traits::vector_capacity;
};

} // namespace yy_quad

namespace yy_traits::traits_detail {

template<typename T,
         std::size_t Capacity,
         yy_quad::ClearAction ca>
struct vector_traits<yy_quad::static_vector<T, Capacity, ca>>:
      std::true_type
{
};

template<typename T,
         std::size_t Capacity,
         yy_quad::ClearAction ca>
struct container_traits<yy_quad::static_vector<T, Capacity, ca>>:
      std::true_type
{
    using value_type = typename yy_quad::static_vector<T, Capacity, ca>::value_type;
};

template<typename T,
         std::size_t Capacity,
         yy_quad::ClearAction ca>
struct vector_traits<yy_quad::static_simple_vector<T, Capacity, ca>>:
      std::true_type
{
};

template<typename T,
         std::size_t Capacity,
         yy_quad::ClearAction ca>
struct container_traits<yy_quad::static_simple_vector<T, Capacity, ca>>:
      std::true_type
{
    using value_type = typename yy_quad::static_simple_vector<T, Capacity, ca>::value_type;
};

} // namespace yy_traits::traits_detail
} // namespace yafiyogi
