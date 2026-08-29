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

#include <string_view>

namespace yafiyogi::yy_util {

inline constexpr std::string_view linear_whitespace = {" \t"};
inline constexpr std::string_view whitespace = {" \f\n\r\t\v"}; // See https://en.cppreference.com/cpp/string/byte/isspace

std::string_view trim_left(std::string_view str,
                           const std::string_view chs) noexcept;
std::string_view trim_right(std::string_view str,
                            const std::string_view chs) noexcept;
std::string_view trim(std::string_view str,
                      const std::string_view chs) noexcept;

inline std::string_view trim_left(std::string_view str) noexcept
{
  return trim_left(str, whitespace);
}

inline std::string_view trim_right(std::string_view str) noexcept
{
  return trim_right(str, whitespace);
}

inline std::string_view trim(std::string_view str) noexcept
{
  return trim(str, whitespace);
}

} // namespace yafiyogi::yy_util
