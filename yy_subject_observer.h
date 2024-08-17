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

#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>

#include "yy_func_traits.h"
#include "yy_type_traits.h"

namespace yafiyogi::yy_data {
namespace subject_observer_detail {

template<typename ReturnType,
         typename... OtherArgs>
class observer_base
{
  public:
    using return_type = yy_traits::remove_cvr_t<ReturnType>;
    using ptr_type = std::unique_ptr<observer_base<return_type,
                                                   OtherArgs...>>;

    constexpr observer_base() noexcept = default;
    observer_base(const observer_base &) = delete;
    observer_base(observer_base &&) = delete;
    constexpr virtual ~observer_base() noexcept = default;

    observer_base & operator=(const observer_base &) = delete;
    observer_base & operator=(observer_base &&) = delete;

    virtual return_type event(const void * data, OtherArgs && ...args) = 0;
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
    using return_type = yy_traits::remove_cvr_t<ReturnType>;
    using method_ptr = return_type (T::*)(const ParamType *, OtherArgs &&...);
    using object_ptr = std::shared_ptr<T>;

    constexpr explicit observer_class_method(object_ptr p_obj,
                                             method_ptr p_method) noexcept:
      m_obj(std::move(p_obj)),
      m_method(std::move(p_method))
    {
    }

    constexpr explicit observer_class_method(object_ptr && p_obj,
                                             method_ptr p_method) noexcept:
      m_obj(std::move(p_obj)),
      m_method(std::move(p_method))
    {
    }

    observer_class_method() = delete;
    observer_class_method(const observer_class_method &) = delete;
    observer_class_method(observer_class_method &&) = delete;

    observer_class_method & operator=(const observer_class_method &) = delete;
    observer_class_method & operator=(observer_class_method &&) = delete;

    constexpr return_type event(const void * p_data,
                                OtherArgs && ...p_args) override
    {
      T * obj = m_obj.get();

      return (obj->*m_method)(static_cast<const ParamType *>(p_data),
                              std::forward<OtherArgs>(p_args)...);
    }

  private:
    object_ptr m_obj{};
    method_ptr m_method{};
};

template<typename T,
         typename ReturnType,
         typename... OtherArgs>
class observer_functor final:
      public observer_base<ReturnType,
                           OtherArgs...>
{
  public:
    using return_type = yy_traits::remove_cvr_t<ReturnType>;
    using func_traits = yy_traits::func_traits<T>;
    using arg_type = typename func_traits::arg_types::template arg_type<0>;

    constexpr explicit observer_functor(T && p_func) noexcept:
      m_func(std::move(p_func))
    {
    }

    observer_functor() = delete;
    observer_functor(const observer_functor &) = delete;
    observer_functor(observer_functor &&) = delete;

    observer_functor & operator=(const observer_functor &) = delete;
    observer_functor & operator=(observer_functor &&) = delete;

    constexpr return_type event(const void * p_data,
                                OtherArgs && ...p_args) override
    {
      return m_func(static_cast<arg_type>(p_data),
                    std::forward<OtherArgs>(p_args)...);
    }

  private:
    T m_func{};
};

template<typename ReturnType,
         typename ParamType,
         typename... OtherArgs>
class observer_fn final:
      public observer_base<ReturnType,
                           OtherArgs...>
{
  public:
    using return_type = yy_traits::remove_cvr_t<ReturnType>;
    using fn_type = return_type (*)(ParamType *, OtherArgs && ...);

    explicit observer_fn(fn_type p_func) noexcept:
      m_func(p_func)
    {
    }

    observer_fn() = delete;
    observer_fn(const observer_fn &) = delete;
    observer_fn(observer_fn &&) = delete;

    observer_fn & operator=(const observer_fn &) = delete;
    observer_fn & operator=(observer_fn &&) = delete;

    constexpr return_type event(const void * p_data,
                                OtherArgs && ...p_args) override
    {
      return m_func(static_cast<ParamType *>(p_data),
                    std::forward<OtherArgs>(p_args)...);
    }

  private:
    fn_type m_func{};
};

template<typename ReturnType>
struct value_valid_type
{
    using return_type = yy_traits::remove_cvr_t<ReturnType>;

    return_type value{};
    bool found = false;
};

} // subject_observer_detail

template<typename KeyType,
         typename ReturnType,
         typename... OtherArgs>
class subject final
{
  public:
    using key_type = yy_traits::remove_cvr_t<KeyType>;
    using return_type = yy_traits::remove_cvr_t<ReturnType>;
    using observer_base_type = subject_observer_detail::observer_base<return_type,
                                                                      OtherArgs...>;
    using value_valid_type = subject_observer_detail::value_valid_type<return_type>;
    using map_type = std::unordered_map<key_type,
                                        typename observer_base_type::ptr_type>;

    constexpr subject() noexcept = default;
    subject(const subject &) = delete;
    constexpr subject(subject &&) noexcept = default;

    subject & operator=(const subject &) = delete;
    subject & operator=(subject &&) noexcept = default;

    constexpr value_valid_type event(const key_type & p_key,
                                     const void * p_data,
                                     OtherArgs && ...p_args)
    {
      if(auto found = m_observers.find(p_key);
         m_observers.end() != found)
      {
        return value_valid_type{found->second->event(p_data,
                                                     std::forward<OtherArgs>(p_args)...), true};
      }
      return value_valid_type{return_type{}, false};
    }

    // Add object method.
    template<typename T,
             typename ParamType>
    constexpr bool add(const key_type & p_key,
                       typename std::shared_ptr<T> & p_obj,
                       return_type (T::*p_method)(const ParamType *,
                                                  OtherArgs && ...args))
    {
      using observer_type = subject_observer_detail::observer_class_method<T, return_type, ParamType, OtherArgs...>;

      bool added = false;
      if(p_obj)
      {
        added = m_observers.try_emplace(
          p_key,
          std::make_unique<observer_type>(p_obj,
                                          p_method)).second;
      }
      return added;
    }

    template<typename ParamType>
    constexpr bool add(const key_type & p_key,
                       return_type (p_fn)(ParamType *, OtherArgs && ...))
    {
      using observer_type = subject_observer_detail::observer_fn<return_type, ParamType, OtherArgs...>;

      return m_observers.emplace(
        p_key,
        std::make_unique<observer_type>(p_fn)).second;
    }

    template<typename T>
    constexpr bool add(const key_type & p_key,
                       T && p_func)
    {
      using observer_type = subject_observer_detail::observer_functor<T, return_type, OtherArgs...>;

      return m_observers.try_emplace(
        p_key,
        std::make_unique<observer_type>(std::forward<T>(p_func))).second;
    }

    constexpr void erase(const key_type & p_key)
    {
      m_observers.erase(p_key);
    }

  private:
    map_type m_observers{};
};

template<typename KeyType,
         typename... OtherArgs>
class subject<KeyType,
              void,
              OtherArgs...> final
{
  public:
    using key_type = yy_traits::remove_cvr_t<KeyType>;
    using observer_ptr = typename subject_observer_detail::observer_base<void,
                                                                         OtherArgs...>::ptr_type;
    using map_type = std::unordered_map<key_type,
                                        observer_ptr>;

    constexpr subject() noexcept = default;
    subject(const subject &) = delete;
    constexpr subject(subject &&) noexcept = default;

    subject & operator=(const subject &) = delete;
    subject & operator=(subject &&) noexcept = default;

    constexpr bool event(const key_type & p_key,
                         const void * p_data,
                         OtherArgs && ...p_args)
    {
      auto found = m_observers.find(p_key);

      const bool call = m_observers.end() != found;
      if(call)
      {
        found->second->event(p_data,
                             std::forward<OtherArgs>(p_args)...);
      }

      return call;
    }

    // Add object method.
    template<typename T,
             typename ParamType>
    constexpr bool add(const key_type & p_key,
                       typename std::shared_ptr<T> & p_obj,
                       void (T::*p_method)(const ParamType *,
                                           OtherArgs && ...p_args))
    {
      using observer_type = subject_observer_detail::observer_class_method<T, void, ParamType, OtherArgs...>;

      bool added = false;
      if(p_obj)
      {
        typename map_type::iterator not_used;
        std::tie(not_used, added) = m_observers.try_emplace(
          p_key,
          std::make_unique<observer_type>(p_obj,
                                          std::move(p_method)));
      }
      return added;
    }

    template<typename ParamType>
    constexpr bool add(const key_type & p_key,
                       void (p_fn)(ParamType *, OtherArgs && ...))
    {
      using observer_type = subject_observer_detail::observer_fn<void, ParamType, OtherArgs...>;

      return m_observers.emplace(
        p_key,
        std::make_unique<observer_type>(p_fn)).second;
    }

    template<typename T>
    constexpr bool add(const key_type & p_key,
                       T && p_func)
    {
      using observer_type = subject_observer_detail::observer_functor<T, void, OtherArgs...>;

      auto [not_used, added] = m_observers.try_emplace(
        p_key,
        std::make_unique<observer_type>(std::forward<T>(p_func)));

      return added;
    }

    constexpr void erase(const key_type & p_key)
    {
      m_observers.erase(p_key);
    }

  private:
    map_type m_observers{};
};

} // namespace yafiyogi::yy_data
