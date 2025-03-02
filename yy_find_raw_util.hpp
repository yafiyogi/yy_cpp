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

#include <algorithm>
#include <type_traits>

#include "yy_find_raw_types.hpp"

namespace yafiyogi::yy_data {

template<typename KeyType,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto lower_bound_raw(KeyType * p_begin,
                                      KeyType * p_end,
                                      const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::raw_traits_type<KeyType>;
  using key_ptr = typename traits::key_ptr;
  using end_type = typename traits::end_type;

  key_ptr key_iter = std::lower_bound(p_begin, p_end, p_key);
  bool is_end = p_end == key_iter;

  return end_type{key_iter, is_end};
}

template<typename KeyType,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto lower_bound_raw(const KeyType * p_begin,
                                      const KeyType * p_end,
                                      const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::raw_traits_type<KeyType>;
  using const_key_ptr = typename traits::const_key_ptr;
  using const_end_type = typename traits::const_end_type;

  const_key_ptr key_iter = std::lower_bound(p_begin, p_end, p_key);
  bool is_end = p_end == key_iter;

  return const_end_type{key_iter, is_end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto lower_bound_raw_pos(const KeyStore & p_key_store,
                                          const InputKeyType & p_key) noexcept
{
  auto l_begin{p_key_store.data()};
  auto [iter, is_end] = lower_bound_raw(l_begin, l_begin + p_key_store.size(), p_key);

  return pos_end_type{static_cast<size_type>(iter - l_begin), is_end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto find_raw(KeyStore & p_key_store,
                               const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::raw_traits_type<typename KeyStore::value_type>;
  using key_ptr = typename traits::key_ptr;
  using found_type = typename traits::found_type;

  key_ptr l_begin{p_key_store.data()};
  key_ptr l_end{l_begin + p_key_store.size()};

  auto [key_iter, is_end] = lower_bound_raw(l_begin, l_end, p_key);
  bool found = !is_end && (*key_iter == p_key);

  return found_type{key_iter, found};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto find_raw(const KeyStore & p_key_store,
                               const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::raw_traits_type<typename KeyStore::value_type>;
  using const_key_ptr = typename traits::const_key_ptr;
  using const_found_type = typename traits::const_found_type;

  const_key_ptr l_begin{p_key_store.data()};
  const_key_ptr l_end{l_begin + p_key_store.size()};

  auto [key_iter, is_end] = lower_bound_raw(l_begin, l_end, p_key);
  bool found = !is_end && (*key_iter == p_key);

  return const_found_type{key_iter, found};
}

template<size_type size_threshold = find_util_detail::default_size_threshold,
         typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto find_raw_pos(const KeyStore & p_key_store,
                                   const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::raw_traits_type<typename KeyStore::value_type>;
  using found_type = typename traits::const_found_type;

  auto l_begin{p_key_store.data()};
  found_type raw_found{};

  if(p_key_store.size() > size_threshold)
  {
    raw_found = find_raw(p_key_store, p_key);
  }
  else
  {
    auto l_end{l_begin + p_key_store.size()};
    auto found{std::find_if(l_begin, l_end, [&p_key](const auto & value){
      return !(value < p_key);
    })};

    bool is_equal = (found != l_end) && (p_key == *found);

    raw_found = found_type{found, is_equal};
  }

  return pos_found_type{static_cast<size_type>(raw_found.iter - l_begin), raw_found.found};
}

} //namespace yafiyogi::yy_data
