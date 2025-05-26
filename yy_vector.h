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

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <utility>

#include "yy_clear_action.h"
#include "yy_compare_util.h"
#include "yy_iterator_ptr.hpp"
#include "yy_range.hpp"
#include "yy_ref_traits.h"
#include "yy_traits_span.hpp"
#include "yy_traits_vector.hpp"
#include "yy_type_traits.h"


namespace yafiyogi::yy_quad {

using ClearAction = yy_data::ClearAction;

namespace vector_detail {

template<typename T>
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
};

struct distance_valid_type final
{
    size_type distance = 0;
    bool valid = false;
};

} // namespace vector_detail

template<typename T,
         ClearAction default_action = yy_data::default_clear_action_v<yy_traits::remove_cvr_t<T>>>
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
    using reference = typename traits::reference;
    using const_reference = typename traits::const_reference;
    using iterator = yy_data::iterator_detail::iterator_ptr<vector>;
    using const_iterator = yy_data::iterator_detail::const_iterator_ptr<vector>;

    struct insert_result final
    {
        iterator iter{};
        bool inserted = false;
    };

    static_assert(std::is_default_constructible_v<value_type>, "T must be default contructable.");
    static_assert(std::is_destructible_v<value_type>, "T must be destructable.");

    constexpr explicit vector(size_type num):
      m_size(num)
    {
      reserve(m_size);
    }

    constexpr vector(std::initializer_list<value_type> p_il) noexcept
    {
      reserve(p_il.size());

      std::move(p_il.begin(), p_il.end(), begin());

      m_size = p_il.size();
    }

    template<typename InputIterator>
    constexpr vector(InputIterator p_begin,
                     InputIterator p_end) noexcept
    {
      auto l_size = static_cast<size_type>(std::distance(p_begin, p_end));

      reserve(l_size);

      std::copy(p_begin, p_end, begin());

      m_size = l_size;
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

      move(std::move(other));
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
      move(std::move(other));

      return *this;
    }

    [[nodiscard]]
    constexpr bool operator<(const vector & other) const noexcept
    {
      return yy_util::less_than(*this, other);
    }

    template<typename type,
             std::enable_if_t<yy_traits::is_container_v<type>
                              && !yy_traits::is_span_v<type>>>
    [[nodiscard]]
    constexpr bool operator<(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(*this, other);
    }

    template<typename type,
             std::enable_if_t<yy_traits::is_container_v<type>
                              && !yy_traits::is_span_v<type>>>
    [[nodiscard]]
    friend constexpr bool operator<(const type & a,
                                    const vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::less_than(a, b);
    }

    [[nodiscard]]
    constexpr bool operator==(const vector & other) const noexcept
    {
      return yy_util::equal(*this, other);
    }

    template<typename type,
             std::enable_if_t<yy_traits::is_container_v<type>
                              && !yy_traits::is_span_v<type>>>
    [[nodiscard]]
    constexpr bool operator==(const type & other) const noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(*this, other);
    }

    template<typename type,
             std::enable_if_t<yy_traits::is_container_v<type>
                              && !yy_traits::is_span_v<type>>>
    [[nodiscard]]
    friend constexpr bool operator==(const type & a,
                                     const vector & b) noexcept
    {
      static_assert(yy_traits::is_container_v<type>, "Type must be a container.");

      return yy_util::equal(a, b);
    }

    [[nodiscard]]
    constexpr int compare(const vector & p_other) const noexcept
    {
      return yy_util::compare(*this, p_other);
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
        throw std::out_of_range("vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref back() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("vector<T>::back(): vector is empty!");
      }

      return *(end() - 1);
    }

    [[nodiscard]]
    constexpr value_type & front()
    {
      if(begin() == end())
      {
        throw std::out_of_range("vector<T>::front(): vector is empty!");
      }

      return *begin();
    }

    [[nodiscard]]
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
      return size_type{0} == size();
    }

    [[nodiscard]]
    constexpr insert_result add_empty(iterator pos)
    {
      auto [distance, valid] = distance_valid(pos, m_size + 1);

      if(!valid)
      {
        return insert_result{end(), false};
      }

      if((0 == distance) && (m_offset != 0))
      {
        // Trying to insert at begining of vector when offset > 0
        --m_offset;
        --m_size;
      }
      else if(m_size == m_capacity)
      {
        if(0 != m_offset)
        {
          std::move(begin(), begin() + distance, raw_data());
          if(distance != m_size)
          {
            std::move(begin() + distance, end(), raw_data() + distance + 1);
          }
          m_size -= m_offset;
          m_offset = 0;
        }
        else
        {
          // In search of the perfect dynamic array growth factor: https://youtu.be/GZPqDvG615k?si=-UBeW34k9mIj3Njv
          reserve_and_move((m_size + 1) * 2, distance);
          // Can't use the parameter 'pos' after this point!
        }
      }
      else if(distance < m_size)
      {
#if defined(__GNUC__) && ! defined(__clang__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wnonnull" // for g++ 12.3
#endif
        std::move_backward(pos, end(), end() + 1);
#if defined(__GNUC__) && ! defined(__clang__)
# pragma GCC diagnostic pop
#endif
      }

      ++m_size;

      return insert_result{begin() + distance, true};
    }

    template<typename InputValueType>
    constexpr insert_result emplace(iterator pos,
                                    InputValueType && value)
    {
      insert_result result{add_empty(pos)};

      if(result.inserted)
      {
        *result.iter = std::forward<InputValueType>(value);
      }

      return result;
    }

    template<typename ...Args>
    constexpr insert_result emplace(iterator pos,
                                    Args && ...args)
    {
      insert_result result{add_empty(pos)};

      if(result.inserted)
      {
        *result.iter = value_type{std::forward<Args>(args)...};
      }

      return result;
    }

    template<typename InputValueType>
    constexpr reference emplace_back(InputValueType && value)
    {
      auto [iter, inserted] = emplace(end(), std::forward<InputValueType>(value));

      return *iter;
    }

    template<typename ...Args>
    constexpr reference emplace_back(Args && ...args)
    {
      auto [iter, inserted] = emplace(end(), std::forward<Args>(args)...);

      return *iter;
    }

    template<typename InputValueType>
    constexpr  reference push_back(InputValueType && value)
    {
      return emplace(end(), std::forward<InputValueType>(value));
    }

    template<typename InputValueType>
    constexpr reference swap_data_back(InputValueType && value)
    {
      auto [iter, inserted] = swap_data(end(), std::forward<InputValueType>(value));

      return *iter;
    }

    template<typename InputValueType>
    constexpr insert_result swap_data(iterator pos,
                                      InputValueType && value)
    {
      insert_result result{add_empty(pos)};

      if(result.inserted)
      {
        std::swap(*result.iter, std::forward<InputValueType>(value));
      }

      return result;
    }

    constexpr void reserve(size_type new_capacity)
    {
      reserve_and_move(new_capacity, m_size);
    }

    constexpr bool erase(iterator pos,
                         ClearAction action = default_action) noexcept
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
          }
          else if(distance == 0)
          {
            // Erase first element.
            if(ClearAction::Clear == action)
            {
              *pos = value_type{};
            }
            ++m_offset;
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
          reset_offset_size();
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
            if(ClearAction::Clear == action)
            {
              for(auto range{yy_util::make_range(p_begin, p_end)};
                  auto & item : range)
              {
                item = value_type{};
              }
            }
            m_offset += static_cast<size_type>(distance);
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

    constexpr void copy(const vector & other,
                        ClearAction action = default_action)
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

    void assign(const vector & other,
                ClearAction action = default_action)
    {
      copy(other, action);
    }

    void assign(vector && other)
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

    void append(const yy_quad::const_span<value_type> & other)
    {
      reserve(size() + other.size());

      std::copy(other.begin(), other.end(), end());
      m_size += other.size();
    }

    void append(vector && other)
    {
      reserve(size() + other.size());

      std::move(other.begin(), other.end(), end());
      m_size += other.size();
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
        move(std::move(other));
      }
    }

    friend constexpr void swap(vector & lhs, vector & rhs) noexcept
    {
      lhs.swap(rhs);
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) noexcept
    {
      for(auto range{yy_util::make_range(data(), data() + size())};
          auto & item : range)
      {
        visitor(item);
      }
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) const noexcept
    {
      for(auto range{yy_util::make_range(data(), data() + size())};
          const auto & item : range)
      {
        visitor(item);
      }
    }

  private:
    constexpr void move(vector && p_other) noexcept
    {
      if(this != &p_other)
      {
        m_data = std::move(p_other.m_data);
        m_capacity = p_other.m_capacity;
        p_other.m_capacity = 0;
        m_size = p_other.m_size;
        p_other.m_size = 0;
        m_offset = p_other.m_offset;
        p_other.m_offset = 0;
      }
    }

    [[nodiscard]]
    constexpr value_ptr raw_data() noexcept
    {
      return m_data.get();
    }

    [[nodiscard]]
    constexpr const_value_ptr raw_data() const noexcept
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

    constexpr vector_detail::distance_valid_type distance_valid(const iterator pos,
                                                                size_type max) const noexcept
    {
      ssize_type distance = const_iterator{pos} - begin();

      return vector_detail::distance_valid_type{static_cast<size_type>(std::max(distance, ssize_type{0})),
                                                (distance >= 0) && (static_cast<size_type>(distance) < max)};
    }

    constexpr void clear(size_type start,
                         ClearAction action = default_action) noexcept
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

    constexpr void reserve_and_move(size_type new_capacity,
                                    size_type pos) noexcept
    {
      if(new_capacity > m_capacity)
      {
        auto new_data{std::make_unique<vector_type>(new_capacity)};

        if((m_size > 0) && m_data)
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

    vector_ptr m_data = nullptr;
    size_type m_size = 0;
    size_type m_offset = 0;
    size_type m_capacity = 0;
};

template<typename T,
         ClearAction default_action = yy_data::default_clear_action_v<yy_traits::remove_cvr_t<T>>>
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
    using iterator = yy_data::iterator_detail::iterator_ptr<simple_vector>;
    using const_iterator = yy_data::iterator_detail::const_iterator_ptr<simple_vector>;
    using reference = typename traits::reference;
    using const_reference = typename traits::const_reference;

    struct insert_result final
    {
        iterator iter{};
        bool inserted = false;
    };

    static_assert(std::is_default_constructible_v<value_type>, "T must be default contructable.");
    static_assert(std::is_destructible_v<value_type>, "T must be destructable.");

    constexpr explicit simple_vector(size_type num):
      m_size(num)
    {
      reserve(m_size);
    }

    constexpr simple_vector(std::initializer_list<value_type> p_il) noexcept
    {
      reserve(p_il.size());

      std::move(p_il.begin(), p_il.end(), begin());

      m_size = p_il.size();
    }

    template<typename InputIterator>
    constexpr simple_vector(InputIterator p_begin,
                            InputIterator p_end) noexcept
    {
      auto l_size = static_cast<size_type>(std::distance(p_begin, p_end));

      reserve(l_size);

      std::copy(p_begin, p_end, begin());

      m_size = l_size;
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

      move(std::move(other));
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
      move(std::move(other));

      return *this;
    }

    [[nodiscard]]
    constexpr bool operator<(const simple_vector & other) const noexcept
    {
      return yy_util::less_than(*this, other);
    }

    template<typename type,
             std::enable_if_t<yy_traits::is_container_v<type>
                              && !yy_traits::is_span_v<type>>>
    [[nodiscard]]
    constexpr bool operator<(const type & other) const noexcept
    {
      return yy_util::less_than(*this, other);
    }

    template<typename type,
             std::enable_if_t<yy_traits::is_container_v<type>
                              && !yy_traits::is_span_v<type>>>
    [[nodiscard]]
    friend constexpr bool operator<(const type & a,
                                    const simple_vector & b) noexcept
    {
      return yy_util::less_than(a, b);
    }

    [[nodiscard]]
    constexpr bool operator==(const simple_vector & other) const noexcept
    {
      return yy_util::equal(*this, other);
    }

    template<typename type,
             std::enable_if_t<yy_traits::is_container_v<type>
                              && !yy_traits::is_span_v<type>>>
    [[nodiscard]]
    constexpr bool operator==(const type & other) const noexcept
    {
      return yy_util::equal(*this, other);
    }

    template<typename type,
             std::enable_if_t<yy_traits::is_container_v<type>
                              && !yy_traits::is_span_v<type>>>
    [[nodiscard]]
    friend constexpr bool operator==(const type & a,
                                     const simple_vector & b) noexcept
    {
      return yy_util::equal(a, b);
    }

    [[nodiscard]]
    constexpr int compare(const simple_vector & p_other) const noexcept
    {
      return yy_util::compare(*this, p_other);
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
        throw std::out_of_range("simple_vector<T>::back(): vector is empty!");
      }

      return *(data() + m_size - 1);
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref back() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("simple_vector<T>::back(): vector is empty!");
      }

      return *(data() + m_size - 1);
    }

    [[nodiscard]]
    constexpr value_type & front()
    {
      if(begin() == end())
      {
        throw std::out_of_range("simple_vector<T>::front(): vector is empty!");
      }

      return *data();
    }

    [[nodiscard]]
    constexpr value_const_l_value_ref front() const
    {
      if(begin() == end())
      {
        throw std::out_of_range("simple_vector<T>::front(): vector is empty!");
      }

      return *data();
    }

    [[nodiscard]]
    constexpr value_ptr data() noexcept
    {
      return m_data.get();
    }

    [[nodiscard]]
    constexpr const_value_ptr data() const noexcept
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
      return size_type{0} == size();
    }

    [[nodiscard]]
    constexpr insert_result add_empty(iterator pos)
    {
      insert_result result{end(), false};

      auto [distance, valid] = distance_valid(pos, m_size + 1);

      if(valid)
      {
        if(m_size == m_capacity)
        {
          // In search of the perfect dynamic array growth factor: https://youtu.be/GZPqDvG615k?si=-UBeW34k9mIj3Njv
          reserve_and_move((m_size + 1) * 2, distance);
          // Can't use the parameter 'pos' after this point!
        }
        else if(distance < m_size)
        {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull" // for g++ 12.3
#endif
          std::move_backward(pos, end(), end() + 1);
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic pop
#endif
        }

        result.iter = begin() + distance;
        result.inserted = true;
        ++m_size;
      }

      return result;
    }

    template<typename InputValueType>
    constexpr insert_result emplace(iterator pos,
                                    InputValueType && value)
    {
      insert_result result{add_empty(pos)};

      if(result.inserted)
      {
        *result.iter = std::forward<InputValueType>(value);
      }

      return result;
    }

    template<typename ...Args>
    constexpr insert_result emplace(iterator pos,
                                    Args && ...args)
    {
      insert_result result{add_empty(pos)};

      if(result.inserted)
      {
        *result.iter = value_type{std::forward<Args>(args)...};
      }

      return result;
    }

    template<typename InputValueType>
    constexpr reference emplace_back(InputValueType && value)
    {
      auto [iter, inserted] = emplace(end(), std::forward<InputValueType>(value));

      return *iter;
    }

    template<typename ...Args>
    constexpr reference emplace_back(Args && ...args)
    {
      auto [iter, inserted] = emplace(end(), std::forward<Args>(args)...);

      return *iter;
    }

    template<typename InputValueType>
    constexpr void push_back(InputValueType && value)
    {
      emplace(end(), std::forward<InputValueType>(value));
    }

    template<typename InputValueType>
    constexpr reference swap_data_back(InputValueType && value)
    {
      auto [iter, inserted] = swap_data(end(), std::forward<InputValueType>(value));

      return *iter;
    }

    template<typename InputValueType>
    constexpr insert_result swap_data(iterator pos,
                                      InputValueType && value)
    {
      insert_result result{add_empty(pos)};

      if(result.inserted)
      {
        std::swap(*result.iter, std::forward<InputValueType>(value));
      }

      return result;
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow" // for g++ 12.3
#endif
          std::move(pos + 1, end(), pos);
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic pop
#endif
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
        if(distance < ssize_type{0})
        {
          p_begin = begin();
        }
        else if(static_cast<size_type>(distance) > m_size)
        {
          p_begin = end();
        }

        if(p_end < p_begin)
        {
          p_end = p_begin;
        }
        else if(p_end > end())
        {
          p_end = end();
        }

        if(p_end != p_begin)
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
          m_size -= static_cast<size_type>(p_end - p_begin);
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

    constexpr void copy(const simple_vector & other,
                        ClearAction action = default_action)
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

    void assign(const simple_vector & other,
                ClearAction action = default_action)
    {
      copy(other, action);
    }

    void assign(simple_vector && other)
    {
      move(std::move(other));
    }

    void assign(yy_quad::const_span<value_type> other,
                ClearAction action = default_action)
    {
      if(((other.data() < data())
          || (other.data() > (data() + size())))
         && (((other.data() + other.size()) < data())
             || ((other.data() + other.size()) > (data() + size()))))
      {
        clear(action);
        reserve(other.size());

        std::copy(other.begin(), other.end(), begin());
        m_size = other.size();
      }
    }

    void append(yy_quad::const_span<value_type> other)
    {
      reserve(size() + other.size());

      std::copy(other.begin(), other.end(), end());
      m_size += other.size();
    }

    void append(simple_vector && other)
    {
      reserve(size() + other.size());

      std::move(other.begin(), other.end(), end());
      m_size += other.size();
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
      move(std::move(other));
    }

    friend constexpr void swap(simple_vector & lhs, simple_vector & rhs) noexcept
    {
      lhs.swap(rhs);
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) noexcept
    {
      for(auto range{yy_util::make_range(data(), data() + size())};
          auto & item : range)
      {
        visitor(item);
      }
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) const noexcept
    {
      for(auto range{yy_util::make_range(data(), data() + size())};
          const auto & item : range)
      {
        visitor(item);
      }
    }

  private:
    constexpr void move(simple_vector && p_other) noexcept
    {
      if(this != &p_other)
      {
        m_data = std::move(p_other.m_data);
        m_capacity = p_other.m_capacity;
        p_other.m_capacity = 0;
        m_size = p_other.m_size;
        p_other.m_size = 0;
      }
    }

    [[nodiscard]]
    constexpr vector_detail::distance_valid_type distance_valid(const iterator pos,
                                                                size_type max) noexcept
    {
      ssize_type distance = const_iterator{pos} - begin();

      return vector_detail::distance_valid_type{static_cast<size_type>(std::max(distance, ssize_type{0})),
                                                (distance >= 0) && (static_cast<size_type>(distance) < max)};
    }

    constexpr void clear(size_type start,
                         ClearAction action = default_action) noexcept
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

    constexpr void reserve_and_move(size_type new_capacity,
                                    size_type pos) noexcept
    {
      if(new_capacity > m_capacity)
      {
        auto new_data{std::make_unique<vector_type>(new_capacity)};

        if((m_size > 0) && m_data)
        {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow" // for g++ 12.3
#endif
          std::move(begin(), begin() + pos, new_data.get());
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic pop
#endif
          if(pos != m_size)
          {
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow" // for g++ 12.3
#pragma GCC diagnostic ignored "-Warray-bounds" // for g++ 12.3
#endif
            std::move(begin() + pos, end(), new_data.get() + pos + 1);
#if defined(__GNUC__) && ! defined(__clang__)
#pragma GCC diagnostic pop
#endif
          }
        }
        m_data = std::move(new_data);
        m_capacity = new_capacity;
      }
    }

    using vector_type = value_type[];
    using vector_ptr = std::unique_ptr<vector_type>;

    vector_ptr m_data = nullptr;
    size_type m_size = 0;
    size_type m_capacity = 0;
};

} // namespace yafiyogi::yy_quad
