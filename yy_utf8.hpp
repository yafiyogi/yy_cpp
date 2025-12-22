/*

  MIT License

  Copyright (c) 2025 Yafiyogi

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

#include <stddef.h>
#include <string_view>

namespace yafiyogi::yy_util {
namespace utf8_detail {

constexpr char utf8_bits_mask = static_cast<char>(0xC0);
constexpr char utf8_bits = static_cast<char>(0x80);

constexpr char utf8_start = static_cast<char>(0xC0);
constexpr char utf8_start_mask = static_cast<char>(0xC0);

constexpr char utf8_2 = static_cast<char>(0xC0);
constexpr char utf8_2_mask = static_cast<char>(0xE0);

constexpr char utf8_3 = static_cast<char>(0xE0);
constexpr char utf8_3_mask = static_cast<char>(0xF0);

constexpr char utf8_4 = static_cast<char>(0xF0);
constexpr char utf8_4_mask = static_cast<char>(0xF8);

} // namespace utf8_detail

inline size_t utf8_len(const char ch) noexcept
{
  size_t size = (utf8_detail::utf8_bits & ch) == 0;

  if((utf8_detail::utf8_start_mask & ch) == utf8_detail::utf8_start)
  {
    size = 1;

    size += (utf8_detail::utf8_2 & ch) == utf8_detail::utf8_2;
    size += (utf8_detail::utf8_3 & ch) == utf8_detail::utf8_3;
    size += (utf8_detail::utf8_4 & ch) == utf8_detail::utf8_4;
  }

  return size;
}

std::string_view find(std::string_view sv, std::string_view delim) noexcept;
std::string_view find_first_of(std::string_view sv,
                               std::string_view delim) noexcept;
size_t utf8_find_last(std::string_view sv) noexcept;

} // namespace yafiyogi::yy_util
