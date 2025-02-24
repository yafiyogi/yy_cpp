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

#include "yy_find_iter_types.hpp"

namespace yafiyogi::yy_data {

template<typename Iterator,
         typename KeyType>
[[nodiscard]]
constexpr auto lower_bound_iter(const Iterator & p_begin,
                                const Iterator & p_end,
                                KeyType && p_key) noexcept
{
  using traits = find_util_detail::iter_traits_type<KeyType,
                                                    Iterator>;
  using end_type = typename traits::end_type;

  Iterator key_iter{std::lower_bound(p_begin, p_end, p_key)};

  auto is_end = p_end == key_iter;

  return end_type{key_iter, is_end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto lower_bound_iter_pos(const KeyStore & p_key_store,
                                           const InputKeyType & p_key) noexcept
{
  auto l_begin{p_key_store.begin()};
  auto [iter, is_end] = lower_bound_iter(l_begin, p_key_store.end(), p_key);

  return pos_end_type{static_cast<size_type>(iter - l_begin), is_end};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto find_iter(KeyStore & p_key_store,
                                const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::iter_traits_type<typename KeyStore::value_type,
                                                    typename KeyStore::iterator>;
  using found_type = typename traits::found_type;

  auto [iter, is_end] = lower_bound_iter(p_key_store.begin(), p_key_store.end(), p_key);

  bool found = !is_end && (*iter == p_key);

  return found_type{iter, found};
}

template<typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr inline auto find_iter(const KeyStore & p_key_store,
                                const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::iter_traits_type<typename KeyStore::value_type,
                                                    typename KeyStore::const_iterator>;
  using found_type = typename traits::found_type;

  auto [iter, is_end] = lower_bound_iter(p_key_store.begin(), p_key_store.end(), p_key);

  bool found = !is_end && (*iter == p_key);

  return found_type{iter, found};
}

template<std::size_t size_threshold = find_util_detail::default_size_threshold,
         typename KeyStore,
         typename InputKeyType>
[[nodiscard]]
constexpr auto find_iter_pos(const KeyStore & p_key_store,
                             const InputKeyType & p_key) noexcept
{
  using traits = find_util_detail::iter_traits_type<typename KeyStore::value_type,
                                                    typename KeyStore::const_iterator>;
  using found_type = typename traits::found_type;

  auto l_begin{p_key_store.begin()};
  found_type iter_found{};

  if(p_key_store.size() > size_threshold)
  {
    iter_found = find_iter(p_key_store, p_key);
  }
  else
  {
    auto l_end{p_key_store.end()};
    auto found{std::find_if(l_begin, l_end, [&p_key](const auto & value){
      return !(value < p_key);
    })};

    bool is_equal = (found != l_end) && (p_key == *found);

    iter_found = found_type{found, is_equal};
  }

  return pos_found_type{static_cast<size_type>(iter_found.iter - l_begin), iter_found.found};
}

} // namespace yafiyogi::yy_data
