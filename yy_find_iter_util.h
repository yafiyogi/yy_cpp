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

namespace yafiyogi::yy_data {
namespace find_iter_util_detail {

template<typename Iterator>
struct iter_end_type final
{
    Iterator iter{};
    bool is_end = false;
};

template<typename SizeType>
struct pos_end_type final
{
    SizeType pos = 0;
    bool is_end = false;
};

template<typename Iterator>
struct iter_found_type final
{
    Iterator iter{};
    bool found = false;
};

template<typename SizeType>
struct pos_found_type final
{
    SizeType pos = 0;
    bool found = false;
};

} // namespace find_iter_util_detail

template<typename Iterator,
         typename KeyType>
constexpr auto lower_bound_iter(const Iterator & p_begin,
                                const Iterator & p_end,
                                KeyType && p_key) noexcept
{
  using iter_end_type = find_iter_util_detail::iter_end_type<Iterator>;

  Iterator key_iter{std::lower_bound(p_begin, p_end, p_key)};

  auto is_end = p_end == key_iter;

  return iter_end_type{key_iter, is_end};
}

template<typename Iterator,
         typename KeyType>
constexpr auto lower_bound_iter_pos(const Iterator & p_begin,
                                    const Iterator & p_end,
                                    KeyType && p_key) noexcept
{
  using size_type = Iterator::size_type;
  using pos_end_type = find_iter_util_detail::pos_end_type<size_type>;

  auto [iter, is_end] = lower_bound_iter(p_begin, p_end, p_key);

  auto pos = static_cast<size_type>(iter - p_begin);

  return pos_end_type{pos, is_end};
}

template<typename Iterator,
         typename KeyType>
constexpr auto find_iter(const Iterator & p_begin,
                         const Iterator & p_end,
                         KeyType && p_key) noexcept
{
  using iter_found_type = find_iter_util_detail::iter_found_type<Iterator>;

  auto [iter, is_end] = lower_bound_iter(p_begin, p_end, p_key);

  bool found = !is_end && (*iter == p_key);

  return iter_found_type{iter, found};
}

template<typename Iterator,
         typename KeyType>
constexpr auto find_iter_pos(const Iterator & p_begin,
                             const Iterator & p_end,
                             KeyType && p_key) noexcept
{
  using size_type = Iterator::size_type;
  using pos_found_type = find_iter_util_detail::pos_found_type<size_type>;
  using iter_found_type = find_iter_util_detail::iter_found_type<Iterator>;

  iter_found_type iter_found;

  if(auto size = p_end - p_begin;
     size > 16)
  {
    iter_found = find_iter(p_begin, p_end, p_key);
  }
  else
  {
    bool is_equal = false;
    auto found{std::find_if(p_begin, p_end, [&p_key, &is_equal](const auto & value){
      bool less_than = value < p_key;
      is_equal = !less_than && (p_key == value);

      return !less_than;
    })};

    iter_found = iter_found_type{found, is_equal};
  }

  auto pos = static_cast<size_type>(iter_found.iter - p_begin);

  return pos_found_type{pos, iter_found.found};
}

} // namespace yafiyogi::yy_data
