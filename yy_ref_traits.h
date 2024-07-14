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

#include <type_traits>

#include "yy_type_traits.h"

namespace yafiyogi::yy_traits {

template<typename T,
         typename Enable = void>
struct ref_traits
{
    using type = yy_traits::remove_cvr_t<T>;
    using l_value_ref = std::add_lvalue_reference_t<type>;
    using const_l_value_ref = std::add_lvalue_reference_t<std::add_const_t<type>>;
    using r_value_ref = std::add_rvalue_reference_t<type>;
};

template<typename T>
struct ref_traits<T,
                  std::enable_if_t<std::is_trivially_copyable_v<yy_traits::remove_cvr_t<T>>>>
{
    using type = yy_traits::remove_cvr_t<T>;
    using l_value_ref = type;
    using const_l_value_ref = std::add_const_t<type>;
    using r_value_ref = type;
};

} // namespace yafiyogi::yy_traits
