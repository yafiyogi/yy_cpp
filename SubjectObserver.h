/*

  MIT License

  Copyright (c) 2019-2022 Yafiyogi

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
# define SubjectObserver_h

#include <memory>
#include <unordered_map>
#include <algorithm>

#include "yafiyogi/cpp/yy_func_traits.h"

namespace yafiyogi {

template<typename R, typename... Args>
class ObserverBase
{
public:
  ObserverBase() = default;
  ObserverBase(const ObserverBase&) = default;
  ObserverBase(ObserverBase&&) noexcept = default;
  ObserverBase & operator=(const ObserverBase&) = default;
  ObserverBase & operator=(ObserverBase&&) noexcept = default;

    virtual ~ObserverBase() = default;
    using ptr_type = std::unique_ptr< ObserverBase<R, Args...>>;

    virtual R event( const void * data, const Args & ...args) = 0;
};

template <typename T, typename R, typename A, typename... Args>
class ObserverMP:
        public ObserverBase<R, Args...>
{
public:
    using method_ptr = R (T::*)(const A *, const Args & ...);
    using object_ptr = std::shared_ptr<T>;

    ObserverMP( const object_ptr & obj,
               method_ptr method):
        m_obj( obj),
        m_method( method) {}

    R event( const void * data, const Args & ...args) override
    {
        T * obj = m_obj.get();

        return (obj->*m_method)( reinterpret_cast<const A *>( data), args...);
    }

private:
    object_ptr m_obj;
    method_ptr m_method;
};

template<typename T, typename R, typename... Args>
class ObserverSP:
        public ObserverBase<R, Args...>
{
public:
    using traits = yy_traits::func_traits<T>;
    using arg_type = typename traits::template arg_type<0>;

    ObserverSP( T func):
        m_func( func) {}

    R event( const void * data, const Args & ...args) override
    {
      return m_func( reinterpret_cast<const arg_type>( data), args...);
    }

private:
    T m_func;
};

template< typename K, typename R, typename... Args>
class Subject
{
public:
    using observer_type = ObserverBase<R, Args...>;
    using map_type = std::unordered_map<K, typename observer_type::ptr_type>;

    std::tuple<bool, R> event( const K & key,
                              const void * data,
                              const Args & ...args)
    {
        auto found = m_observers.find( key);

        auto rv = R{};

        const bool call = m_observers.end() != found;
        if( call)
        {
            rv = std::move( found->second->event( data, args...));
        }
        return std::make_tuple( call, rv);
    }

    template<typename T, typename A>
    bool add( const K & key,
             typename std::shared_ptr<T> & obj,
             R (T::*method)( const A *, const Args & ...args))
    {
        bool rv = false;
        if( obj)
        {
          typename map_type::iterator not_used;
          std::tie(not_used, rv) = m_observers.try_emplace( key, std::make_unique<ObserverMP<T, R, A, Args...>>(obj, method));
        }
        return rv;
    }

    template<typename T>
    bool add( const K & key,
             T && func)
    {
        auto [not_used, rv] = m_observers.try_emplace( key, std::make_unique<ObserverSP<T, R>>( std::forward<T>( func)));

        return rv;
    }

    void erase( const K & key)
    {
        m_observers.erase( key);
    }

    map_type m_observers;
};

template< typename K, typename... Args>
class Subject<K, void, Args...>
{
public:
    using observer_ptr = typename ObserverBase<void, Args...>::ptr_type;
    using map_type = std::unordered_map<K, observer_ptr>;

    bool event( const K & key,
               const void * data,
               const Args & ...args)
    {
        auto found = m_observers.find( key);

        const bool call = m_observers.end() != found;
        if( call)
        {
            found->second->event( data, args...);
        }

        return call;
    }

    template<typename T, typename A>
    bool add( const K & key,
             typename std::shared_ptr<T> & obj,
             void (T::*method)( const A *, const Args & ...args))
    {
        bool rv = false;
        if( obj)
        {
          typename map_type::iterator not_used;
          std::tie(not_used, rv) = m_observers.try_emplace( key, std::make_unique<ObserverMP<T, void, A, Args...>>( obj, method));
        }
        return rv;
    }

    template<typename T>
    bool add( const K & key,
             T func)
    {
      auto [not_used, rv] = m_observers.try_emplace( key, std::make_unique<ObserverSP<T, void, Args...>>( func));

      return rv;
    }

    void erase( const K & key)
    {
        m_observers.erase( key);
    }

    map_type m_observers;
};

} // Namespace yafiyogi

#endif // SubjectObserver_h
