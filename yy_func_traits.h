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

#include <climits>
#include <cstddef>
#include <tuple>

#include "yy_arg_traits.h"
#include "yy_type_traits.h"

namespace yafiyogi::yy_traits {

template<typename T>
struct func_traits:
      public func_traits<decltype(&T::operator())>
{
};

template<typename R,
         typename... Args>
struct func_traits<R (*)(Args...)>:
      arg_traits<Args...>
{
    using result_type = remove_rcv_t<R>;
    using arg_types = arg_traits<Args...>;
    using class_type = void;
};

template<typename... Args>
struct func_traits<void (*)(Args...)>:
      arg_traits<Args...>
{
    using result_type = void;
    using arg_types = arg_traits<Args...>;
    using class_type = void;
};

template<typename C,
         typename R,
         typename... Args>
struct func_traits<R (C::*)(Args...)>:
      arg_traits<Args...>
{
    using result_type = remove_rcv_t<R>;
    using arg_types = arg_traits<Args...>;
    using class_type = remove_rcv_t<C>;
};

template<typename C,
         typename R,
         typename... Args>
struct func_traits<R (C::*)(Args...) const>:
      arg_traits<Args...>
{
    using result_type = remove_rcv_t<R>;
    using arg_types = arg_traits<Args...>;
    using class_type = remove_rcv_t<C>;
};

template<typename C, typename... Args>
struct func_traits<void (C::*)(Args...)>:
      arg_traits<Args...>
{
    using result_type = void;
    using arg_types = arg_traits<Args...>;
    using class_type = remove_rcv_t<C>;
};

template<typename C, typename... Args>
struct func_traits<void (C::*)(Args...) const>:
      arg_traits<Args...>
{
    using result_type = void;
    using arg_types = arg_traits<Args...>;
    using class_type = remove_rcv_t<C>;
};

} // namespace yafiyogi::yy_traits
