/*

  MIT License

  Copyright (c) 2019 Yafiyogi

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
    virtual ~ObserverBase() {}
    typedef std::unique_ptr< ObserverBase<R, Args...>> Ptr;

    virtual R event( const void * data, const Args & ...args) = 0;
};

template <typename T, typename R, typename A, typename... Args>
class ObserverMP:
        public ObserverBase<R, Args...>
{
public:
    typedef R (T::*MethodPtr)(const A *, const Args & ...);
    typedef std::shared_ptr<T> ObjPtr;

    ObserverMP( const ObjPtr & obj,
               MethodPtr method):
        m_obj( obj),
        m_method( method) {}

    R event( const void * data, const Args & ...args) override
    {
        T * obj = m_obj.get();

        return (obj->*m_method)( reinterpret_cast<const A *>( data), args...);
    }

private:
    ObjPtr m_obj;
    MethodPtr m_method;
};

template<typename T, typename R, typename... Args>
class ObserverFunc:
        public ObserverBase<R, Args...>
{
public:
    typedef yy_func_traits<T> traits;
    typedef typename traits::template arg<0> arg;

    ObserverFunc( T func):
        m_func( func) {}

    R event( const void * data, const Args & ...args) override
    {
        return m_func( reinterpret_cast<const typename arg::type>( data), args...);
    }

private:
    T m_func;
};

template< typename K, typename R, typename... Args>
class Subject
{
public:
    typedef typename ObserverBase<R, Args...>::Ptr observer_ptr;
    typedef std::unordered_map<K, observer_ptr> Map;

    std::tuple<bool, R> event( const K & key,
                              const void * data,
                              const Args & ...args)
    {
        auto && found = m_observers.find( key);

        R rv = R();

        const bool call = m_observers.end() != found;
        if( call)
        {
            rv = std::move( found->second->event( data, args...));
        }
        return std::make_tuple( call, rv);
    }

    template<typename T, typename A>
    void add( const K & key,
             typename std::shared_ptr<T> & obj,
             R (T::*method)( const A *, const Args & ...args))
    {
        if( obj)
        {
            m_observers.emplace( key, observer_ptr( new ObserverMP<T, R, A, Args...>( obj, method)));
        }
    }

    template<typename T>
    void add( const K & key,
             T && func)
    {
        m_observers.emplace( key, observer_ptr( new ObserverFunc<T, R>( func)));
    }

    void erase( const K & key)
    {
        m_observers.erase( key);
    }

    Map m_observers;
};

template< typename K, typename... Args>
class Subject<K, void, Args...>
{
public:
    typedef typename ObserverBase<void, Args...>::Ptr observer_ptr;
    typedef std::unordered_map<K, observer_ptr> Map;

    bool event( const K & key,
               const void * data,
               Args & ...args)
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
    void add( const K & key,
             typename std::shared_ptr<T> & obj,
             void (T::*method)( const A *, const Args & ...args))
    {
        if( obj)
        {
            m_observers.emplace( key, observer_ptr( new ObserverMP<T, void, A, Args...>( obj, method)));
        }
    }

    template<typename T>
    void add( const K & key,
             T func)
    {
        m_observers.emplace( key, observer_ptr( new ObserverFunc<T, void, Args...>( func)));
    }

    void erase( const K & key)
    {
        m_observers.erase( key);
    }

    Map m_observers;
};
} // Namespace yafiyogi

#endif // SubjectObserver_h
