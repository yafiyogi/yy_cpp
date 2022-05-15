/*

  MIT License

  Copyright (c) 2022 Yafiyogi

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

#ifndef yy_vector_traits_h
# define yy_vector_traits_h

#include <array>
#include <vector>
#include <type_traits>

#include "yy_type_traits.h"

namespace yafiyogi::yy_traits {
namespace detail {

template<typename T,
         std::size_t N>
struct container_traits<std::array<T, N>>:
    std::true_type
{
  using value_type = remove_rcv_t<typename std::array<T, N>::value_type>;
};

template<typename T>
struct container_traits<std::vector<T>>:
    std::true_type
{
  using value_type = remove_rcv_t<typename std::vector<T>::value_type>;
};

template<typename T>
struct vector_traits:
    std::false_type
{
};

template<typename T>
struct vector_traits<std::vector<T>>:
    std::true_type
{
};

template<typename T>
struct array_traits:
    std::false_type
{
};

template<typename T, std::size_t N>
struct array_traits<std::array<T, N>>:
    std::true_type
{
};

} // namespace detail

/** @brief is_vector type trait */
template<typename T>
struct is_vector:
    detail::vector_traits<remove_rcv_t<T>>
{
};

template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

template<typename T>
using is_vector_t = typename is_vector<T>::type;


/** @brief is_array type trait */
template<typename T>
struct is_array:
    detail::array_traits<remove_rcv_t<T>>
{
};

template<typename T>
inline constexpr bool is_array_v = is_array<T>::value;

template<typename T>
using is_array_t = typename is_array<T>::type;

} // namespace yafiyogi::yy_traits

#endif // yy_vector_traits_h
