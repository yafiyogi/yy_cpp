/*

  MIT License

  Copyright (c) 2021-2022 Yafiyogi

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

#ifndef yy_lockable_value_h
#define yy_lockable_value_h

#include <utility>

#include "yy_type_traits.h"

namespace yafiyogi::yy_util {
namespace traits {

template<typename Value, typename Visitor, typename Enable = void>
struct lockable_value_visitor_traits
{
    static void visit(Value & value, Visitor && visitor)
    {
      visitor(value);
    }

    static void visit(const Value & value, Visitor && visitor)
    {
      visitor(value);
    }
};

template<typename Value, typename Visitor>
struct lockable_value_visitor_traits<
  Value,
  Visitor,
  std::enable_if_t<
    std::is_pointer_v<Value> || yy_traits::is_smart_ptr_v<Value>>>
{
    static void visit(Value & value, Visitor && visitor)
    {
      if(value)
      {
        visitor(*value);
      }
    }

    static void visit(const Value & value, Visitor && visitor)
    {
      if(value)
      {
        visitor(*value);
      }
    }
};

template<typename Value, typename Visitor>
struct lockable_value_visitor_traits<
  Value,
  Visitor,
  std::enable_if_t<yy_traits::is_optional_v<Value>>>
{
    static void visit(Value & value, Visitor && visitor)
    {
      if(value.has_value())
      {
        visitor(value.value());
      }
    }

    static void visit(const Value & value, Visitor && visitor)
    {
      if(value.has_value())
      {
        visitor(value.value());
      }
    }
};

} // namespace traits

template<typename ValueType, typename MutexType>
class lockable_value
{
  public:
    using value_type = ValueType;
    using mutex_type = MutexType;

    lockable_value() = default;
    lockable_value(const lockable_value &) = delete;
    lockable_value(lockable_value &&) = default;

    lockable_value & operator=(const lockable_value &) = delete;

    lockable_value & operator=(const lockable_value && rhs)
    {
      if(this != &rhs)
      {
        m_value = std::move(rhs.m_value);
        m_mtx = std::move(rhs.m_mtx);
      }

      return *this;
    }

    template<typename... Args>
    lockable_value(Args &&... args) :
      m_value(std::forward<Args>(args)...),
      m_mtx()
    {
    }

    template<typename LockType>
    value_type get() const
    {
      LockType lck(m_mtx);

      return m_value;
    }

    template<typename LockType>
    value_type set(value_type && value)
    {
      LockType lck(m_mtx);

      return m_value = std::forward<value_type>(value);
    }

    template<typename LockType>
    value_type exchange(value_type value)
    {
      LockType lck(m_mtx);

      std::swap(m_value, value);

      return value;
    }

    template<typename LockType, typename Visitor>
    void visit(Visitor && visitor) const
    {
      LockType lck(m_mtx);

      using visitor_traits =
        traits::lockable_value_visitor_traits<value_type, Visitor>;

      visitor_traits::visit(m_value, visitor);
    }

    template<typename LockType, typename Visitor>
    void visit(Visitor && visitor)
    {
      LockType lck(m_mtx);

      using visitor_traits =
        traits::lockable_value_visitor_traits<value_type, Visitor>;

      visitor_traits::visit(m_value, visitor);
    }

  private:
    value_type m_value;
    mutable mutex_type m_mtx;
};

template<typename LockableValue, typename GuardType>
class lock_type
{
  public:
    using value_type = typename LockableValue::value_type;
    using guard_type = GuardType;

    static value_type get(const LockableValue & lockable)
    {
      return lockable.template get<guard_type>();
    }

    static value_type set(LockableValue & lockable, value_type value)
    {
      return lockable.template set<guard_type>(std::move(value));
    }

    static value_type exchange(LockableValue & lockable, value_type value)
    {
      return lockable.template exchange<guard_type>(std::move(value));
    }

    template<typename Visitor>
    static void visit(const LockableValue & lockable, Visitor && visitor)
    {
      lockable.template visit<guard_type>(visitor);
    }

    template<typename Visitor>
    static void locked_visit(LockableValue & lockable, Visitor && visitor)
    {
      lockable.template visit<guard_type>(visitor);
    }
};

} // namespace yafiyogi::yy_util

#endif // yy_lockable_value_h
