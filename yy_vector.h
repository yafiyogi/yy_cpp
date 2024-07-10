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

#include <algorithm>
#include <memory>
#include <typeinfo>
#include <utility>

#include "yy_bit_twiddling.h"
#include "yy_clear_action.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_utility.h"
#include "yy_vector_traits.h"

namespace yafiyogi {
namespace yy_quad {

using ClearAction = yy_data::ClearAction;

namespace vector_detail {

template<typename T>
struct vector_traits final
{
    using value_type = yy_traits::remove_rcv_t<T>;
    using value_l_value_ref = typename yy_traits::ref_traits<value_type>::l_value_ref;
    using value_const_l_value_ref = typename yy_traits::ref_traits<value_type>::const_l_value_ref;
    using value_r_value_ref = typename yy_traits::ref_traits<value_type>::r_value_ref;
    using value_ptr = value_type *;
    using const_value_ptr = const value_ptr;
    using iterator = value_type *;
    using const_iterator = const iterator;
    using size_type = std::size_t;
    using distance_type = std::ptrdiff_t;
    struct return_value
    {
        iterator iter{};
        bool inserted{};
    };

    static constexpr ClearAction default_action = yy_data::default_action<value_type>;

};

} // namespace vector_detail

template<typename T,
         ClearAction default_action = vector_detail::vector_traits<T>::default_action>
class vector
{
  public:
    using traits = vector_detail::vector_traits<T>;
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
    using return_value = typename traits::return_value;

    static_assert(std::is_default_constructible_v<value_type>, "T must be defualt contructable.");
    static_assert(std::is_destructible_v<value_type>, "T must be destructable.");

    constexpr explicit vector(size_type num)
    {
      reserve(num);
      m_size = num;
    }

    constexpr vector() noexcept = default;

    constexpr vector(const vector & other)
    {
      static_assert(std::is_copy_constructible_v<value_type>, "T must be copy constructable.");
      copy(other);
    }

    constexpr vector(vector && other) noexcept
    {
      static_assert(std::is_move_constructible_v<value_type>, "T must be move constructable.");
      swap(other);
    }

    constexpr vector(std::initializer_list<value_type> p_il) noexcept
    {
      reserve(p_il.size());
      for(auto & item : p_il)
      {
        emplace_back(std::move(item));
      }
    }

    constexpr ~vector() noexcept
    {
      clear(ClearAction::Keep);
    }

    constexpr vector & operator=(const vector & other)
    {
      static_assert(std::is_copy_assignable_v<value_type>, "T must be copy assignable.");
      copy(other);

      return *this;
    }

    constexpr vector & operator=(vector && other) noexcept
    {
      vector tmp{};

      tmp.swap(other);
      swap(tmp);

      return *this;
    }

    constexpr bool operator<(const vector & other) const noexcept
    {
      return std::lexicographical_compare(begin(), end(),
                                          other.data(), other.data() + other.size());

    }

    template<typename type>
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(begin(), end(),
                                          other.data(), other.data() + other.size());

    }

    template<typename type>
    friend constexpr bool operator<(const type & a,
                                    const vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(a.data(), a.data() + a.size(),
                                          b.begin(), b.end());
    }

    constexpr bool operator==(const vector & other) const noexcept
    {
      return (size() == other.size())
        && std::equal(begin(), end(), other.data());
    }

    template<typename type>
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (size() == other.size())
        && std::equal(begin(), end(), other.data());
    }

    template<typename type>
    friend constexpr bool operator==(const type & a,
                                     const vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (a.size() == b.size())
        && std::equal(a.data(), a.data() + a.size(), b.begin());
    }

    constexpr iterator begin() noexcept
    {
      return raw_data() + m_offset;
    }

    constexpr const_iterator begin() const noexcept
    {
      return raw_data() + m_offset;
    }

    constexpr iterator end() noexcept
    {
      return raw_data() + m_size;
    }

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

    constexpr value_type & back()
    {
      if(begin() == end())
      {
        throw std::out_of_range("vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    constexpr value_const_l_value_ref back() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    constexpr value_type & front()
    {
      if(begin() == end())
      {
        throw std::out_of_range("vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    constexpr value_const_l_value_ref front() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    [[nodiscard]]
    constexpr value_ptr data() noexcept
    {
      return raw_data() + m_offset;
    }

    [[nodiscard]]
    constexpr const value_ptr data() const noexcept
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

    template<typename InputValueType>
    constexpr void emplace_back(InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_rcv_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_rcv_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      emplace(end(), std::forward<InputValueType>(value));
    }

    constexpr return_value add_empty(value_ptr pos)
    {
      return_value rv{end(), false};

      auto [distance, valid] = distance_valid(pos, m_size + 1);

      if(valid)
      {
        if(m_size == m_capacity)
        {
          reserve_and_move(yy_bit_twiddling::round_up_pow2(m_size + 1), static_cast<size_type>(distance));
          // Can't use the parameter pos after this point!
        }
        else
        {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic ignored "-Wnonnull" // for g++ 12.3
#endif
          std::move_backward(pos, end(), end() + 1);
        }

        rv.iter = begin() + distance;
        rv.inserted = true;
        ++m_size;
      }

      return rv;
    }

    template<typename InputValueType>
    constexpr return_value emplace(iterator pos, InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_rcv_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_rcv_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");



      return_value rv = add_empty(pos);

      if(rv.inserted)
      {
        *rv.iter = std::forward<InputValueType>(value);
      }

      return rv;
    }

    constexpr void reserve(size_type new_capacity)
    {
      reserve_and_move(new_capacity, m_size);
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
              for(auto range{yy_util::make_range(p_begin, p_end)};
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
              for(auto range{yy_util::make_range(p_end, end())};
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

    constexpr void copy(const vector & other, ClearAction action = default_action)
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

        reserve_and_move(other.size(), m_size);

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

    constexpr void swap(vector & other) noexcept
    {
      if(this != &other)
      {
        std::swap(m_data, other.m_data);
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_size, other.m_size);
        std::swap(m_offset, other.m_offset);
      }
    }

    constexpr void swap(vector && other) noexcept
    {
      if(this != &other)
      {
        std::swap(m_data, other.m_data);
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_size, other.m_size);
        std::swap(m_offset, other.m_offset);
      }
    }

  private:
    constexpr value_ptr raw_data() noexcept
    {
      return m_data.get();
    }

    constexpr const value_ptr raw_data() const noexcept
    {
      return m_data.get();
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
        size_type distance;
        bool valid;
    };

    constexpr distance_valid_type distance_valid(iterator pos, size_type max) const noexcept
    {
      const auto distance = std::distance(begin(), pos);

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

    constexpr void reserve_and_move(size_type new_capacity, size_type pos)
    {
      if(new_capacity > m_capacity)
      {
        auto new_data = std::make_unique<vector_type>(new_capacity);

        if(m_size > 0)
        {
          std::move(begin(), begin() + pos, new_data.get());
          if(pos != m_size)
          {
            std::move(begin() + pos, end(), new_data.get() + pos + 1);
          }
          m_size -= m_offset;
          m_offset = 0;
        }
        m_data = std::move(new_data);
        m_capacity = new_capacity;
      }
    }

    using vector_type = value_type[];
    using vector_ptr = std::unique_ptr<vector_type>;

    vector_ptr m_data;
    size_type m_size{};
    size_type m_offset{};
    size_type m_capacity{};
};

template<typename T,
         ClearAction default_action = vector_detail::vector_traits<T>::default_action>
class simple_vector
{
  public:
    using traits = vector_detail::vector_traits<T>;
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
    using return_value = typename traits::return_value;

    static_assert(std::is_default_constructible_v<value_type>, "T must be defualt contructable.");
    static_assert(std::is_destructible_v<value_type>, "T must be destructable.");

    constexpr explicit simple_vector(size_type num)
    {
      reserve(num);
      m_size = num;
    }

    constexpr simple_vector() noexcept = default;

    constexpr simple_vector(const simple_vector & other)
    {
      static_assert(std::is_copy_constructible_v<value_type>, "T must be copy constructable.");
      copy(other);
    }

    constexpr simple_vector(simple_vector && other) noexcept
    {
      static_assert(std::is_move_constructible_v<value_type>, "T must be move constructable.");
      swap(other);
    }

    constexpr simple_vector(std::initializer_list<value_type> p_il) noexcept
    {
      reserve(p_il.size());
      for(auto & item : p_il)
      {
        emplace_back(std::move(item));
      }
    }

    constexpr ~simple_vector() noexcept
    {
      clear(ClearAction::Keep);
    }

    constexpr simple_vector & operator=(const simple_vector & other)
    {
      static_assert(std::is_copy_assignable_v<value_type>, "T must be copy assignable.");
      copy(other);

      return *this;
    }

    constexpr simple_vector & operator=(simple_vector && other) noexcept
    {
      simple_vector tmp{};

      tmp.swap(other);
      swap(tmp);

      return *this;
    }

    constexpr bool operator<(const simple_vector & other) const noexcept
    {
      return std::lexicographical_compare(begin(), end(),
                                          other.data(), other.data() + other.size());

    }

    template<typename type>
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(begin(), end(),
                                          other.data(), other.data() + other.size());

    }

    template<typename type>
    friend constexpr bool operator<(const type & a,
                                    const simple_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return std::lexicographical_compare(a.data(), a.data() + a.size(),
                                          b.begin(), b.end());
    }

    constexpr bool operator==(const simple_vector & other) const noexcept
    {
      return (size() == other.size())
        && std::equal(begin(), end(), other.data());
    }

    template<typename type>
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (size() == other.size())
        && std::equal(begin(), end(), other.data());
    }

    template<typename type>
    friend constexpr bool operator==(const type & a,
                                     const simple_vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return (a.size() == b.size())
        && std::equal(a.data(), a.data() + a.size(), b.begin());
    }

    constexpr iterator begin() noexcept
    {
      return data();
    }

    constexpr const_iterator begin() const noexcept
    {
      return data();
    }

    constexpr iterator end() noexcept
    {
      return data() + m_size;
    }

    constexpr const_iterator end() const noexcept
    {
      return data() + m_size;
    }

    constexpr value_type & operator[](size_type pos) noexcept
    {
      return *(begin() + pos);
    }

    constexpr const value_type & operator[](size_type pos) const noexcept
    {
      return *(begin() + pos);
    }

    constexpr value_type & back()
    {
      if(begin() == end())
      {
        throw std::out_of_range("simple_vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    constexpr value_const_l_value_ref back() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("simple_vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    constexpr value_type & front()
    {
      if(begin() == end())
      {
        throw std::out_of_range("simple_vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    constexpr value_const_l_value_ref front() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("simple_vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    [[nodiscard]]
    constexpr value_ptr data() noexcept
    {
      return m_data.get();
    }

    [[nodiscard]]
    constexpr const value_ptr data() const noexcept
    {
      return m_data.get();
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

    template<typename InputValueType>
    constexpr void emplace_back(InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_rcv_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_rcv_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      emplace(end(), std::forward<InputValueType>(value));
    }

    constexpr return_value add_empty(value_ptr pos)
    {
      return_value rv{end(), false};

      auto [distance, valid] = distance_valid(pos, m_size + 1);

      if(valid)
      {
        if(m_size == m_capacity)
        {
          reserve_and_move(yy_bit_twiddling::round_up_pow2(m_size + 1), static_cast<size_type>(distance));
          // Can't use the parameter pos after this point!
        }
        else
        {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic ignored "-Wnonnull" // for g++ 12.3
#endif
          std::move_backward(pos, end(), end() + 1);
        }

        rv.iter = begin() + distance;
        rv.inserted = true;
        ++m_size;
      }

      return rv;
    }

    template<typename InputValueType>
    constexpr return_value emplace(iterator pos, InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_rcv_t<InputValueType>, value_type>
                    || (std::is_pointer_v<InputValueType> && std::is_base_of_v<value_type, yy_traits::remove_rcv_t<std::remove_pointer<InputValueType>>>),
                    "Value is of an incompatible type.");

      return_value rv = add_empty(pos);

      if(rv.inserted)
      {
        *rv.iter = std::forward<InputValueType>(value);
      }

      return rv;
    }

    constexpr void reserve(size_type new_capacity)
    {
      reserve_and_move(new_capacity, m_size);
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
            for(auto range{yy_util::make_range(p_end, end())};
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

    constexpr void copy(const simple_vector & other, ClearAction action = default_action)
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

        reserve_and_move(other.size(), m_size);

        m_size = other.size();

        // Copy elements from othe simple_vector.
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

    constexpr void swap(simple_vector & other) noexcept
    {
      if(this != &other)
      {
        std::swap(m_data, other.m_data);
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_size, other.m_size);
      }
    }

    constexpr void swap(simple_vector && other) noexcept
    {
      if(this != &other)
      {
        std::swap(m_data, other.m_data);
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_size, other.m_size);
      }
    }

  private:
    struct distance_valid_type final
    {
        size_type distance;
        bool valid;
    };

    constexpr distance_valid_type distance_valid(iterator pos, size_type max) const noexcept
    {
      const auto distance = std::distance(begin(), pos);

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

    constexpr void reserve_and_move(size_type new_capacity, size_type pos)
    {
      if(new_capacity > m_capacity)
      {
        auto new_data = std::make_unique<vector_type>(new_capacity);

        if(m_size > 0)
        {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow" // for g++ 12.3
#endif
          std::move(begin(), begin() + pos, new_data.get());
          if(pos != m_size)
          {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow" // for g++ 12.3
#endif
            std::move(begin() + pos, end(), new_data.get() + pos + 1);
          }
        }
        m_data = std::move(new_data);
        m_capacity = new_capacity;
      }
    }

    using vector_type = value_type[];
    using vector_ptr = std::unique_ptr<vector_type>;

    vector_ptr m_data;
    size_type m_size{};
    size_type m_capacity{};
};

} // namespace yy_quad

namespace yy_traits::traits_detail {

template<typename T,
         yy_quad::ClearAction ct>
struct vector_traits<yy_quad::vector<T, ct>>:
      std::true_type
{
};

template<typename T,
         yy_quad::ClearAction ct>
struct container_traits<yy_quad::vector<T, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::vector<T, ct>::value_type;
};

template<typename T,
         yy_quad::ClearAction ct>
struct vector_traits<yy_quad::simple_vector<T, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::simple_vector<T, ct>::value_type;
};

template<typename T,
         yy_quad::ClearAction ct>
struct container_traits<yy_quad::simple_vector<T, ct>>:
      std::true_type
{
    using value_type = typename yy_quad::simple_vector<T, ct>::value_type;
};

} // namespace yy_traits::traits_detail
} // namespace yafiyogi
