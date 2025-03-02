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

#include <variant>
#include <stdexcept>
#include <type_traits>

#include "yy_type_traits.h"
#include "yy_types.hpp"

namespace yafiyogi::yy_util {
namespace variant_detail {

template<typename V,
         typename T,
         size_type N>
struct check_variant_for_type final
{
    using variant_type = std::variant_alternative_t<N - 1, V>;
    using raw_type = std::remove_pointer_t<yy_traits::remove_cvr_t<variant_type>>;
    static constexpr bool has_type = std::is_same_v<T, variant_type>
                                     || (std::is_pointer_v<T>
                                         && std::is_base_of_v<raw_type, T>)
                                     || check_variant_for_type<V, T, N - 1>::has_type;
};

template<typename V,
         typename T>
struct check_variant_for_type<V, T, 0> final
{
    static constexpr bool has_type = false;
};

} // namespace variant_detail

template<typename VariantType,
         typename T>
inline static constexpr bool check_variant_for_type_v = variant_detail::check_variant_for_type<VariantType,
                                                                                               yy_traits::remove_cvr_t<T>,
                                                                                               std::variant_size_v<VariantType>>::has_type;

// From https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Types>
struct overloaded:
      Types...
{
  using Types::operator()...;
};

// explicit deduction guide (not needed as of C++20)
template<class... Types>
overloaded(Types...) -> overloaded<Types...>;

} // namespace yafiyogi::yy_util
