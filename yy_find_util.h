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

#include <algorithm>
#include <type_traits>

#include "yy_find_iter_util.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_utility.h"

namespace yafiyogi::yy_data {
namespace find_util_detail {

using size_type = std::size_t;

template<typename KeyType>
struct iter_end_t;

template<typename KeyType>
struct const_iter_end_t;

template<typename KeyType>
struct iter_found_t;

template<typename KeyType>
struct const_iter_found_t;

template<typename KeyType>
struct traits_type final
{
    using key_type = yy_traits::remove_cvr_t<KeyType>;
    using key_ptr = std::add_pointer_t<key_type>;
    using const_key_ptr = std::add_pointer_t<std::add_const_t<key_type>>;
    using key_l_value_ref = typename yy_traits::ref_traits<key_type>::l_value_ref;
    using key_r_value_ref = typename yy_traits::ref_traits<key_type>::r_value_ref;
    using iter_end_type = iter_end_t<KeyType>;
    using const_iter_end_type = const_iter_end_t<KeyType>;
    using iter_found_type = iter_found_t<KeyType>;
    using const_iter_found_type = const_iter_found_t<KeyType>;
    using range = yy_util::Range<key_ptr>;
    using const_range = yy_util::Range<const_key_ptr>;
    using pos_range = yy_util::Range<size_type>;
};

template<typename KeyType>
struct iter_end_t final
{
    using traits = traits_type<KeyType>;
    using key_ptr = typename traits::key_ptr;

    key_ptr iter = nullptr;
    bool is_end = false;
};

template<typename KeyType>
struct const_iter_end_t final
{
    using traits = traits_type<KeyType>;
    using const_key_ptr = typename traits::const_key_ptr;

    const_key_ptr iter = nullptr;
    bool is_end = false;
};

template<typename KeyType>
struct iter_found_t final
{
    using traits = traits_type<KeyType>;
    using key_ptr = typename traits::key_ptr;

    key_ptr iter = nullptr;
    bool found = false;
};

template<typename KeyType>
struct const_iter_found_t final
{
    using traits = traits_type<KeyType>;
    using const_key_ptr = typename traits::const_key_ptr;

    const_key_ptr iter = nullptr;
    bool found = false;
};

struct pos_end_type final
{
    size_type pos = 0;
    bool is_end = false;
};

struct pos_found_type final
{
    size_type pos = 0;
    bool found = false;
};

} // find_util_detail

template<typename KeyType,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_lower_bound_raw(KeyType * p_begin,
                                         KeyType * p_end,
                                         const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<KeyType>;
  using iter_end_type = typename traits::iter_end_type;

  auto [iter, is_end] = lower_bound_iter(p_begin, p_end, p_key);

  return iter_end_type{iter, is_end};
}

template<typename KeyType,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_lower_bound_raw(const KeyType * p_begin,
                                         const KeyType * p_end,
                                         const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<KeyType>;
  using const_iter_end_type = typename traits::const_iter_end_type;

  auto [iter, is_end] = lower_bound_iter(p_begin, p_end, p_key);

  return const_iter_end_type{iter, is_end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_lower_bound(const KeyStore & p_keys,
                                     const InputKeyType & p_key) noexcept
{
  auto begin{p_keys.begin()};

  auto [iter, is_end] = do_lower_bound_raw(begin, p_keys.end(), p_key);

  return find_util_detail::pos_end_type{static_cast<find_util_detail::size_type>(iter - begin), is_end};
}

template<typename KeyType,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_upper_bound_raw(KeyType * p_begin,
                                         KeyType * p_end,
                                         const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<KeyType>;
  using key_ptr = typename traits::key_type;
  using iter_end_type = typename traits::iter_end_type;

  key_ptr iter{std::upper_bound(p_begin, p_end, p_key)};

  return iter_end_type{iter, iter == p_end};
}

template<typename KeyType,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_upper_bound_raw(const KeyType * p_begin,
                                         const KeyType * p_end,
                                         const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<KeyType>;
  using const_iter_end_type = typename traits::const_iter_end_type;

  auto iter{std::upper_bound(p_begin, p_end, p_key)};

  return const_iter_end_type{iter, iter == p_end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_upper_bound(const KeyStore & p_keys,
                                     const InputKeyType & p_key) noexcept
{
  auto [iter, is_end] = do_upper_bound_raw(p_keys.begin(), p_keys.end(), p_key);

  return find_util_detail::pos_end_type{static_cast<find_util_detail::size_type>(iter - p_keys.data()), is_end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_range_raw(KeyStore & p_keys,
                                   const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<typename KeyStore::value_type>;
  using range = typename traits::range;

  auto [begin, begin_is_end] = lower_bound_raw(p_keys.begin(), p_keys.end(), p_key);

  if(begin_is_end)
  {
    return range{begin, begin};
  }

  auto [end, end_is_end] = lower_bound_raw(begin, p_keys.end(), p_key);

  return range{begin, end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_range_raw(const KeyStore & p_keys,
                                   const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<typename KeyStore::value_type>;
  using const_range = typename traits::const_range;

  auto [begin, begin_is_end] = lower_bound_raw(p_keys.begin(), p_keys.end(), p_key);

  if(begin_is_end)
  {
    return const_range{begin, begin};
  }

  auto [end, end_is_end] = lower_bound_raw(begin, p_keys.end(), p_key);

  return const_range{begin, end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_range(const KeyStore & p_keys,
                               const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<typename KeyStore::value_type>;
  using const_key_ptr = typename traits::const_key_type;
  using pos_range = typename traits::pos_range;

  auto range_found{do_range_raw(p_key)};
  const_key_ptr begin{p_keys.begin()};

  return pos_range{range_found.begin() - begin, range_found.end() - begin};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_find_raw(KeyStore & p_keys,
                                  const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<typename KeyStore::value_type>;
  using key_ptr = typename traits::key_ptr;
  using iter_found_type = typename traits::iter_found_type;

  key_ptr l_begin{p_keys.data()};
  key_ptr l_end{l_begin + p_keys.size()};
  auto [key_iter, found] = find_iter(l_begin, l_end, p_key);

  return iter_found_type{key_iter, found};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_find_raw(const KeyStore & p_keys,
                                  const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::traits_type<typename KeyStore::value_type>;
  using const_key_ptr = typename traits::const_key_ptr;
  using const_iter_found_type = typename traits::const_iter_found_type;

  const_key_ptr l_begin{p_keys.data()};
  const_key_ptr l_end{l_begin + p_keys.size()};
  auto [key_iter, found] = find_iter(l_begin, l_end, p_key);

  return const_iter_found_type{key_iter, found};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto do_find(const KeyStore & p_keys,
                              const InputKeyType & p_key) noexcept
{
  auto [key_iter, found] = do_find_raw(p_keys, p_key);

  return find_util_detail::pos_found_type{static_cast<KeyStore::size_type>(key_iter - p_keys.data()), found};
}


} //namespace yafiyogi::yy_data
