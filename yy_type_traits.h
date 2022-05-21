/*

  MIT License

  Copyright (c) 2021-2022 Yafiyogi

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
#ifndef yy_type_traits_h
#define yy_type_traits_h

#include <memory>
#include <optional>
#include <type_traits>

namespace yafiyogi::yy_traits {

/**
 * @brief remove_rcv_t type trait. Removes reference, const & volitile from type.
 */
template<typename T>
using remove_rcv_t = std::remove_cv_t<std::remove_reference_t<T>>;

namespace detail {

template<typename T>
struct container_traits: std::false_type
{
    using value_type = void;
};

} // namespace detail

/**
 * @brief is_container type trait
 */
template<typename T>
struct is_container: detail::container_traits<yy_traits::remove_rcv_t<T>>
{
};

template<typename T>
inline constexpr bool is_container_v = is_container<T>::value;

template<typename T>
using is_container_t = typename is_container<T>::type;

template<typename T>
using container_type_t =
  typename detail::container_traits<remove_rcv_t<T>>::value_type;

/**
 * @brief is_smart_ptr type trait
 */
template<typename T>
struct is_smart_ptr: std::false_type
{
};

template<typename T>
struct is_smart_ptr<std::shared_ptr<T>>: std::true_type
{
};

template<typename T>
struct is_smart_ptr<std::unique_ptr<T>>: std::true_type
{
};

template<typename T>
inline constexpr bool is_smart_ptr_v = is_smart_ptr<T>::value;

template<typename T>
using is_smart_ptr_t = typename is_smart_ptr<T>::type;

/**
 * @brief is_optional type trait
 */
template<typename T>
struct is_optional: std::false_type
{
};

template<typename T>
struct is_optional<std::optional<T>>: std::true_type
{
};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template<typename T>
using is_optional_t = typename is_optional<T>::type;

} // namespace yafiyogi::yy_traits

#endif // yy_type_traits_h
