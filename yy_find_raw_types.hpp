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

#include "yy_find_types.hpp"
#include "yy_range.hpp"
#include "yy_ref_traits.h"
#include "yy_types.hpp"
#include "yy_type_traits.h"

namespace yafiyogi::yy_data::find_util_detail {

template<typename KeyType>
struct raw_found_t;

template<typename KeyType>
struct const_raw_found_t;

template<typename KeyType>
struct raw_end_t;

template<typename KeyType>
struct const_raw_end_t;

template<typename KeyType>
struct raw_traits_type final
{
    using key_type = yy_traits::remove_cvr_t<KeyType>;
    using key_ptr = std::add_pointer_t<key_type>;
    using const_key_ptr = std::add_pointer_t<std::add_const_t<key_type>>;
    using key_l_value_ref = typename yy_traits::ref_traits<key_type>::l_value_ref;
    using key_r_value_ref = typename yy_traits::ref_traits<key_type>::r_value_ref;
    using found_type = raw_found_t<KeyType>;
    using const_found_type = const_raw_found_t<KeyType>;
    using end_type = raw_end_t<KeyType>;
    using const_end_type = const_raw_end_t<KeyType>;
    using range = yy_util::Range<key_ptr>;
    using const_range = yy_util::Range<const_key_ptr>;
    using pos_range = yy_util::Range<size_type>;
};

template<typename KeyType>
struct raw_found_t final
{
    using traits = raw_traits_type<KeyType>;
    using key_ptr = typename traits::key_ptr;

    key_ptr iter = nullptr;
    bool found = false;
};

template<typename KeyType>
struct const_raw_found_t final
{
    using traits = raw_traits_type<KeyType>;
    using const_key_ptr = typename traits::const_key_ptr;

    const_key_ptr iter = nullptr;
    bool found = false;
};

template<typename KeyType>
struct raw_end_t final
{
    using traits = raw_traits_type<KeyType>;
    using key_ptr = typename traits::key_ptr;

    key_ptr iter = nullptr;
    bool is_end = false;
};

template<typename KeyType>
struct const_raw_end_t final
{
    using traits = raw_traits_type<KeyType>;
    using const_key_ptr = typename traits::const_key_ptr;

    const_key_ptr iter = nullptr;
    bool is_end = false;
};

} // namespace yafiyogi::yy_data::find_util_detail
