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

#include "yy_func_traits.h"

namespace yafiyogi {

template<typename R>
class ObserverBase
{
public:
    virtual ~ObserverBase() {}
    typedef std::unique_ptr< ObserverBase<R>> Ptr;

    virtual R event( const void * data) = 0;
};

template <typename T, typename A, typename R>
class ObserverMP:
        public ObserverBase<R>
{
public:
    typedef R (T::*MethodPtr)(const A *);
    typedef std::shared_ptr<T> ObjPtr;

    ObserverMP( const ObjPtr & obj,
               MethodPtr method):
        m_obj( obj),
        m_method( method) {}

    R event( const void * data) override
    {
        T * obj = m_obj.get();

        return (obj->*m_method)( reinterpret_cast<const A *>( data));
    }

private:
    ObjPtr m_obj;
    MethodPtr m_method;
};

template<typename T, typename R>
class ObserverFunc:
        public ObserverBase<R>
{
public:
    typedef yy_func_traits<T> traits;
    typedef typename traits::template arg<0> arg;

    ObserverFunc( T func):
        m_func( func) {}

    R event( const void * data)
    {
        return m_func( reinterpret_cast<const typename arg::type>( data));
    }

private:
    T m_func;
};

template< typename K, typename R>
class Subject
{
public:
    typedef typename ObserverBase<R>::Ptr observer_ptr;
    typedef std::unordered_map<K, observer_ptr> Map;

    R event( const K & key,
            const void * data)
    {
        auto && found = m_observers.find( key);

        R rv = R();
        if( m_observers.end() != found)
        {
            rv = std::move( found->second->event( data));
        }
        return rv;
    }

    template<typename T, typename A>
    void add( const K & key,
             typename std::shared_ptr<T> & obj,
             R (T::*method)( const A *))
    {
        if( obj)
        {
            m_observers.emplace( key, observer_ptr( new ObserverMP<T, A, R>( obj, method)));
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

template< typename K>
class Subject<K, void>
{
public:
    typedef typename ObserverBase<void>::Ptr observer_ptr;
    typedef std::unordered_map<K, observer_ptr> Map;

    void event( const K & key,
               const void * data)
    {
        auto && found = m_observers.find( key);

        if( m_observers.end() != found)
        {
            found->second->event( data);
        }
    }

    template<typename T, typename A>
    void add( const K & key,
             typename std::shared_ptr<T> & obj,
             void (T::*method)( const A *))
    {
        if( obj)
        {
            m_observers.emplace( key, observer_ptr( new ObserverMP<T, A, void>( obj, method)));
        }
    }

    template<typename T>
    void add( const K & key,
             T && func)
    {
        m_observers.emplace( key, observer_ptr( new ObserverFunc<T, void>( func)));
    }

    void erase( const K & key)
    {
        m_observers.erase( key);
    }

    Map m_observers;
};
} // Namespace yafiyogi

#endif // SubjectObserver_h
