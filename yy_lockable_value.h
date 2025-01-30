/*

  MIT License

  Copyright (c) 2021-2024-2025 Yafiyogi

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

#include <type_traits>
#include <utility>

#include "yy_type_traits.h"

namespace yafiyogi::yy_util {
namespace traits {

template<typename Value,
         typename Enable = void>
struct lockable_value_visitor_traits final
{
    constexpr lockable_value_visitor_traits() noexcept = default;
    lockable_value_visitor_traits(const lockable_value_visitor_traits &) = delete;
    constexpr lockable_value_visitor_traits(lockable_value_visitor_traits &&) noexcept = default;
    constexpr ~lockable_value_visitor_traits() noexcept = default;

    lockable_value_visitor_traits & operator=(const lockable_value_visitor_traits&) = delete;
    constexpr lockable_value_visitor_traits & operator=(lockable_value_visitor_traits &&) noexcept = default;

    template<typename Visitor>
    constexpr static void visit(Value & value,
                                Visitor && visitor)
    {
      visitor(value);
    }

    template<typename Visitor>
    constexpr static void visit(const Value & value,
                                Visitor && visitor)
    {
      visitor(value);
    }
};

template<typename Value>
struct lockable_value_visitor_traits<Value,
                                     std::enable_if_t<std::is_pointer_v<Value>
                                                      || yy_traits::is_smart_ptr_v<Value>>> final
{
    constexpr lockable_value_visitor_traits() noexcept = default;
    lockable_value_visitor_traits(const lockable_value_visitor_traits &) = delete;
    constexpr lockable_value_visitor_traits(lockable_value_visitor_traits &&) noexcept = default;
    constexpr ~lockable_value_visitor_traits() noexcept = default;

    lockable_value_visitor_traits & operator=(const lockable_value_visitor_traits&) = delete;
    constexpr lockable_value_visitor_traits & operator=(lockable_value_visitor_traits &&) noexcept = default;

    template<typename Visitor>
    static constexpr void visit(Value & value,
                                Visitor && visitor)
    {
      if(value)
      {
        visitor(*value);
      }
    }

    template<typename Visitor>
    static constexpr void visit(const Value & value,
                                Visitor && visitor)
    {
      if(value)
      {
        visitor(*value);
      }
    }
};

template<typename Value>
struct lockable_value_visitor_traits<Value,
                                     std::enable_if_t<yy_traits::is_optional_v<Value>>> final
{
    constexpr lockable_value_visitor_traits() noexcept = default;
    lockable_value_visitor_traits(const lockable_value_visitor_traits &) = delete;
    constexpr lockable_value_visitor_traits(lockable_value_visitor_traits &&) noexcept = default;
    constexpr ~lockable_value_visitor_traits() noexcept = default;

    lockable_value_visitor_traits & operator=(const lockable_value_visitor_traits&) = delete;
    constexpr lockable_value_visitor_traits & operator=(lockable_value_visitor_traits &&) noexcept = default;

    template<typename Visitor>
    static constexpr void visit(Value & value,
                                Visitor && visitor)
    {
      if(value.has_value())
      {
        visitor(value.value());
      }
    }

    template<typename Visitor>
    static constexpr void visit(const Value & value,
                                Visitor && visitor)
    {
      if(value.has_value())
      {
        visitor(value.value());
      }
    }
};

} // namespace traits

template<typename ValueType, typename MutexType>
class lockable_value final
{
  public:
    using value_type = ValueType;
    using mutex_type = MutexType;

    template<typename... Args>
    constexpr lockable_value(Args &&... args) noexcept:
      m_value(std::forward<Args>(args)...),
      m_mtx()
    {
    }

    constexpr lockable_value() noexcept = default;
    lockable_value(const lockable_value &) = delete;
    constexpr lockable_value(lockable_value &&) noexcept = default;
    constexpr ~lockable_value() noexcept = default;

    lockable_value & operator=(const lockable_value &) = delete;
    constexpr lockable_value & operator=(lockable_value && rhs) noexcept = default;

    template<typename LockType>
    constexpr value_type get() const
    {
      LockType lck(m_mtx);

      return m_value;
    }

    template<typename LockType>
    constexpr value_type set(const value_type & value)
    {
      LockType lck(m_mtx);

      return m_value = value;
    }

    template<typename LockType>
    constexpr value_type set(value_type && value)
    {
      LockType lck(m_mtx);

      return m_value = std::move(value);
    }

    template<typename LockType>
    constexpr value_type exchange(value_type & value)
    {
      LockType lck(m_mtx);

      std::swap(m_value, value);

      return value;
    }

    template<typename LockType>
    constexpr value_type exchange(value_type && value)
    {
      LockType lck(m_mtx);

      std::swap(m_value, value);

      return value;
    }

    template<typename LockType,
             typename Visitor>
    constexpr void visit(Visitor && visitor) const
    {
      LockType lck(m_mtx);

      using visitor_traits =
        traits::lockable_value_visitor_traits<value_type>;

      visitor_traits::visit(m_value, visitor);
    }

    template<typename LockType,
             typename Visitor>
    constexpr void visit(Visitor && visitor)
    {
      LockType lck(m_mtx);

      using visitor_traits =
        traits::lockable_value_visitor_traits<value_type>;

      visitor_traits::visit(m_value, visitor);
    }

  private:
    value_type m_value;
    mutable mutex_type m_mtx;
};

template<typename LockableValue,
         typename GuardType>
class lock_type final
{
  public:
    using value_type = typename LockableValue::value_type;
    using guard_type = GuardType;

    constexpr lock_type() noexcept = default;
    lock_type(const lock_type &) = delete;
    constexpr lock_type(lock_type &&) noexcept = default;
    constexpr ~lock_type() noexcept = default;

    lock_type & operator=(const lock_type &) = delete;
    constexpr lock_type & operator=(lock_type && rhs) noexcept = default;

    static constexpr value_type get(const LockableValue & lockable)
    {
      return lockable.template get<guard_type>();
    }

    static constexpr value_type set(LockableValue & lockable,
                                    const value_type & value)
    {
      return lockable.template set<guard_type>(value);
    }

    static constexpr value_type set(LockableValue & lockable,
                                    value_type && value)
    {
      return lockable.template set<guard_type>(std::forward<value_type>(value));
    }

    static constexpr value_type exchange(LockableValue & lockable,
                                         value_type & value)
    {
      return lockable.template exchange<guard_type>(value);
    }

    static constexpr value_type exchange(LockableValue & lockable,
                                         value_type && value)
    {
      return lockable.template exchange<guard_type>(std::forward<value_type>(value));
    }

    template<typename Visitor>
    static constexpr void visit(LockableValue & lockable,
                                Visitor && visitor)
    {
      lockable.template visit<guard_type>(std::forward<Visitor>(visitor));
    }
};

} // namespace yafiyogi::yy_util
