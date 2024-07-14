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

#include <cstddef>

#include <tuple>

#include "yy_cpp/yy_static_flat_map.h"

namespace yafiyogi::yy_data {
namespace make_lookup_detail {

template<typename Key,
         typename Value,
         std::size_t Capacity>
class const_lookup final
{
  public:
    constexpr explicit const_lookup(std::tuple<Key, Value> (&& arr)[Capacity]) noexcept
    {
      for(std::size_t idx = 0; idx < Capacity; ++idx)
      {
        m_lookup.emplace(std::move(std::get<0>(arr[idx])), std::move(std::get<1>(arr[idx])));
      }
    }

    [[nodiscard]]
    constexpr Value lookup(Key key, Value value = Value{}) const noexcept
    {
      auto do_lookup = [&value](const auto visitor_value, auto /*pos*/) {
        value = *visitor_value;
      };

      [[maybe_unused]]
      auto found = m_lookup.find_value(do_lookup, key);

      return value;
    }

  private:
    yy_data::static_flat_map<Key, Value, Capacity> m_lookup{};
};

} // namespace make_lookup_detail


template<typename Key,
         typename Value,
         std::size_t N>
      [[maybe_unused]]
constexpr auto make_lookup(std::tuple<Key, Value> (&& arr)[N])
{
  return make_lookup_detail::const_lookup{std::forward<std::tuple<Key, Value>[N]>(arr)};
}

} // namespace yafiyogi::yy_data
