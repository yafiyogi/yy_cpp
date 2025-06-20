/*

  MIT License

  Copyright (c) 2022-2025 Yafiyogi

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

#include <string>
#include <string_view>
#include <type_traits>

#include "yy_type_traits.h"
#include "yy_types.hpp"

namespace yafiyogi::yy_traits {
namespace traits_detail {

template<typename T>
struct container_traits<std::basic_string<T>>:
      std::true_type
{
    using value_type =
      yy_traits::remove_cvr_t<typename std::basic_string<T>::value_type>;
};

template<typename T>
struct container_traits<std::basic_string_view<T>>:
      std::true_type
{
    using value_type =
      yy_traits::remove_cvr_t<typename std::basic_string_view<T>::value_type>;
};

template<>
struct container_traits<const char *>:
      std::true_type
{
    using value_type = char;
};

template<>
struct container_traits<char *>:
      std::true_type
{
    using value_type = char;
};

template<>
struct container_traits<const char8_t *>:
      std::true_type
{
    using value_type = char8_t;
};

template<>
struct container_traits<char8_t *>:
      std::true_type
{
    using value_type = char8_t;
};

template<size_type N>
struct container_traits<char[N]>:
      std::true_type
{
    using value_type = char;
};

template<size_type N>
struct container_traits<char8_t[N]>:
      std::true_type
{
    using value_type = char8_t;
};

template<typename T,
         typename Enable = void>
struct is_std_string_traits:
      std::false_type
{
};

template<typename T>
struct is_std_string_traits<T,
                            typename std::enable_if_t<std::is_base_of_v<std::string,
                                                                        yy_traits::remove_cvr_t<T>>>>:
      std::true_type
{
};

template<typename T>
struct is_std_string_traits<T,
                            typename std::enable_if_t<std::is_base_of_v<std::u8string,
                                                                        yy_traits::remove_cvr_t<T>>>>:
      std::true_type
{
};

template<typename T,
         typename Enable = void>
struct is_std_string_view_traits:
      std::false_type
{
};

template<typename T>
struct is_std_string_view_traits<T,
                                 typename std::enable_if_t<std::is_base_of_v<std::string_view,
                                                                             yy_traits::remove_cvr_t<T>>>>:
      std::true_type
{
};

template<typename T>
struct is_std_string_view_traits<T,
                                 typename std::enable_if_t<std::is_base_of_v<std::u8string_view,
                                                                             yy_traits::remove_cvr_t<T>>>>:
      std::true_type
{
};

template<typename T,
         typename Enable = void>
struct is_c_string_traits:
      std::false_type
{
};

// is_c_string_traits for char *
template<typename T>
struct is_c_string_traits<T,
                          typename std::enable_if_t<std::is_pointer_v<T>
                                                    && std::is_same_v<char,
                                                                      yy_traits::remove_cvr_t<std::remove_pointer_t<T>>>>>:
      std::true_type
{
};

// is_c_string_traits for char[]
template<typename T>
struct is_c_string_traits<T,
                          typename std::enable_if_t<std::is_array_v<T>
                                                    && std::is_same_v<char,
                                                                      yy_traits::remove_cvr_t<std::remove_all_extents_t<T>>>>>:
      std::true_type
{
};

// is_c_string_traits for char8_t *
template<typename T>
struct is_c_string_traits<T,
                          typename std::enable_if_t<std::is_pointer_v<T>
                                                    && std::is_same_v<char8_t,
                                                                      yy_traits::remove_cvr_t<std::remove_pointer_t<T>>>>>:
      std::true_type
{
};

// is_c_string_traits for char8_t[]
template<typename T>
struct is_c_string_traits<T,
                          typename std::enable_if_t<std::is_array_v<T>
                                                    && std::is_same_v<char8_t,
                                                                      yy_traits::remove_cvr_t<std::remove_all_extents_t<T>>>>>:
      std::true_type
{
};

template<typename T,
         typename Enable = void>
struct is_char_array_traits:
      std::false_type
{
};

template<typename T>
struct is_char_array_traits<T,
                            typename std::enable_if_t<std::is_array_v<T>
                                                      && std::is_same_v<char,
                                                                        yy_traits::remove_cvr_t<std::remove_all_extents_t<T>>>>>:
      std::true_type
{
};

} // namespace traits_detail

/**
 * @brief is_std_string type trait
 */
template<typename T>
using is_std_string = traits_detail::is_std_string_traits<remove_cvr_t<T>>;

template<typename T>
inline constexpr bool is_std_string_v = is_std_string<T>::value;

template<typename T>
using is_std_string_t = typename is_std_string<T>::type;

/**
 * @brief is_std_string_view type trait
 */
template<typename T>
using is_std_string_view = traits_detail::is_std_string_view_traits<remove_cvr_t<T>>;

template<typename T>
inline constexpr bool is_std_string_view_v = is_std_string_view<T>::value;

template<typename T>
using is_std_string_view_t = typename is_std_string_view<T>::type;

/**
 * @brief is_c_string type trait
 */
template<typename T>
using is_c_string = traits_detail::is_c_string_traits<remove_cvr_t<T>>;

template<typename T>
inline constexpr bool is_c_string_v = is_c_string<T>::value;

template<typename T>
using is_c_string_t = typename is_c_string<T>::type;

/**
 * @brief is_string type trait
 */
template<typename T, typename Enable = void>
struct is_string: std::false_type
{
};

template<typename T>
struct is_string<T,
                 typename std::enable_if_t<is_c_string_v<T>
                                           || is_std_string_v<T>
                                           || is_std_string_view_v<T>>>:
      std::true_type
{
};

template<typename T>
inline constexpr bool is_string_v = is_string<T>::value;

template<typename T>
using is_string_t = typename is_string<T>::type;


/**
 * @brief is_c_string type trait
 */
template<typename T>
using is_char_array = traits_detail::is_char_array_traits<remove_cvr_t<T>>;

template<typename T>
inline constexpr bool is_char_array_v = is_char_array<T>::value;

template<typename T>
using is_char_array_t = typename is_char_array<T>::type;

} // namespace yafiyogi::yy_traits
