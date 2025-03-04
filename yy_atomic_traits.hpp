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

#include <atomic>
#include <type_traits>

namespace yafiyogi::yy_traits {
namespace atomic_traits_detail {

template<typename T>
struct is_atomic:
      public std::false_type
{
};

template<typename T>
struct is_atomic<std::atomic<T>>:
      public std::true_type
{
};

} // namespace atomic_traits_detail

/**
 * @brief is_atomic type trait
 */
template<typename T>
using is_atomic = atomic_traits_detail::is_atomic<T>;

template<typename T>
inline constexpr bool is_atomic_v = is_atomic<T>::value;

template<typename T>
using is_atomic_t = typename is_atomic<T>::type;

} // namespace yafiyogi::yy_traits
