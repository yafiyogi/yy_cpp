/*

  MIT License

  Copyright (c) 2019-2024 Yafiyogi

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

#ifndef SubjectObserver_h
#define SubjectObserver_h

#include <algorithm>
#include <memory>
#include <unordered_map>

#include "yy_func_traits.h"

namespace yafiyogi::yy_data {

template<typename ReturnType,
         typename... OtherArgs>
class observer_base
{
  public:
    using ptr_type = std::unique_ptr<observer_base<ReturnType,
                                                   OtherArgs...>>;

    constexpr observer_base() noexcept = default;
    observer_base(const observer_base &) = delete;
    observer_base(observer_base &&) = delete;
    constexpr virtual ~observer_base() noexcept = default;

    observer_base & operator=(const observer_base &) = delete;
    observer_base & operator=(observer_base &&) = delete;

    virtual ReturnType event(const void * data, OtherArgs && ...args) = 0;
};

template<typename T,
         typename ReturnType,
         typename ParamType,
         typename... OtherArgs>
class observer_class_method final:
      public observer_base<ReturnType,
                           OtherArgs...>
{
  public:
    using method_ptr = ReturnType (T::*)(const ParamType *, OtherArgs &&...);
    using object_ptr = std::shared_ptr<T>;

    constexpr explicit observer_class_method(const object_ptr & obj,
                                             method_ptr method) noexcept:
      m_obj(obj),
      m_method(std::move(method))
    {
    }

    constexpr explicit observer_class_method(object_ptr && obj,
                                             method_ptr method) noexcept:
      m_obj(std::move(obj)),
      m_method(std::move(method))
    {
    }

    observer_class_method() = delete;
    observer_class_method(const observer_class_method &) = delete;
    observer_class_method(observer_class_method &&) = delete;
    constexpr ~observer_class_method() noexcept = default;

    observer_class_method & operator=(const observer_class_method &) = delete;
    observer_class_method & operator=(observer_class_method &&) = delete;

    constexpr ReturnType event(const void * data,
                               OtherArgs && ...args) override
    {
      T * obj = m_obj.get();

      return (obj->*m_method)(static_cast<const ParamType *>(data),
                              std::forward<OtherArgs>(args)...);
    }

  private:
    object_ptr m_obj;
    method_ptr m_method;
};

template<typename T,
         typename ReturnType,
         typename... OtherArgs>
class observer_func final:
      public observer_base<ReturnType,
                           OtherArgs...>
{
  public:
    using func_traits = yy_traits::func_traits<T>;
    using arg_type = typename func_traits::arg_types::template arg_type<0>;

    constexpr explicit observer_func(T func) noexcept:
      m_func(std::move(func))
    {
    }

    observer_func() = delete;
    observer_func(const observer_func &) = delete;
    observer_func(observer_func &&) = delete;
    constexpr ~observer_func() noexcept= default;

    observer_func & operator=(const observer_func &) = delete;
    observer_func & operator=(observer_func &&) = delete;

    constexpr ReturnType event(const void * data,
                               OtherArgs && ...args) override
    {
      return m_func(static_cast<arg_type>(data),
                    std::forward<OtherArgs>(args)...);
    }

  private:
    T m_func;
};

template<typename KeyType,
         typename ReturnType,
         typename... OtherArgs>
class subject final
{
  public:
    using observer_type = observer_base<ReturnType,
                                        OtherArgs...>;
    using map_type = std::unordered_map<KeyType,
                                        typename observer_type::ptr_type>;

    constexpr subject() noexcept = default;
    subject(const subject &) = delete;
    constexpr subject(subject &&) noexcept = default;
    constexpr ~subject() noexcept = default;

    subject & operator=(const subject &) = delete;
    subject & operator=(subject &&) noexcept = default;

    constexpr std::tuple<bool, ReturnType> event(const KeyType & key,
                                                 const void * data,
                                                 OtherArgs && ...args)
    {
      auto found = m_observers.find(key);

      if(m_observers.end() != found)
      {
        return {true, found->second->event(data,
                                           std::forward<OtherArgs>(args)...)};
      }
      return {false, ReturnType{}};
    }

    // Add object method.
    template<typename T,
             typename ParamType>
    constexpr bool add(const KeyType & key,
                       typename std::shared_ptr<T> & obj,
                       ReturnType (T::*method)(const ParamType *,
                                               OtherArgs && ...args))
    {
      bool added = false;
      if(obj)
      {
        typename map_type::iterator not_used;
        std::tie(not_used, added) = m_observers.try_emplace(
          key,
          std::make_unique<observer_class_method<T, ReturnType, ParamType, OtherArgs...>>(obj,
                                                                                          method));
      }
      return added;
    }

    template<typename T>
    constexpr bool add(const KeyType & key,
                       T && func)
    {
      auto [not_used, added] = m_observers.try_emplace(
        key,
        std::make_unique<observer_func<T, ReturnType, OtherArgs...>>(std::forward<T>(func)));

      return added;
    }

    constexpr void erase(const KeyType & key)
    {
      m_observers.erase(key);
    }

  private:
    map_type m_observers;
};

template<typename KeyType,
         typename... OtherArgs>
class subject<KeyType,
              void,
              OtherArgs...> final
{
  public:
    using observer_ptr = typename observer_base<void,
                                                OtherArgs...>::ptr_type;
    using map_type = std::unordered_map<KeyType,
                                        observer_ptr>;

    constexpr subject() noexcept = default;
    subject(const subject &) = delete;
    constexpr subject(subject &&) noexcept = default;
    constexpr ~subject() noexcept = default;

    subject & operator=(const subject &) = delete;
    subject & operator=(subject &&) noexcept = default;

    constexpr bool event(const KeyType & key,
                         const void * data,
                         OtherArgs && ...args)
    {
      auto found = m_observers.find(key);

      const bool call = m_observers.end() != found;
      if(call)
      {
        found->second->event(data,
                             std::forward<OtherArgs>(args)...);
      }

      return call;
    }

    // Add object method.
    template<typename T,
             typename ParamType>
    constexpr bool add(const KeyType & key,
                       typename std::shared_ptr<T> & obj,
                       void (T::*method)(const ParamType *,
                                         OtherArgs && ...args))
    {
      bool added = false;
      if(obj)
      {
        typename map_type::iterator not_used;
        std::tie(not_used, added) = m_observers.try_emplace(
          key,
          std::make_unique<observer_class_method<T, void, ParamType, OtherArgs...>>(obj,
                                                                                    std::move(method)));
      }
      return added;
    }

    template<typename T>
    constexpr bool add(const KeyType & key,
                       T func)
    {
      auto [not_used, added] = m_observers.try_emplace(
        key,
        std::make_unique<observer_func<T, void, OtherArgs...>>(std::move(func)));

      return added;
    }

    constexpr void erase(const KeyType & key)
    {
      m_observers.erase(key);
    }

  private:
    map_type m_observers;
};

} // namespace yafiyogi::yy_data

#endif // SubjectObserver_h
