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

#include <cstring>

#include <optional>

#include "yy_fast_atoi.h"
#include "yy_int_util.h"
#include "yy_span.h"
#include "yy_traits_array.hpp"
#include "yy_traits_span.hpp"
#include "yy_traits_string.hpp"
#include "yy_traits_vector.hpp"
#include "yy_types.hpp"

namespace yafiyogi::yy_util {
namespace scan_str_detail {

using scan_type = yy_quad::const_span<char>;
using span_type = yy_quad::span<char>;
using const_span_type = yy_quad::const_span<char>;
using width_type = std::optional<size_type>;

inline constexpr const size_type default_width = 9999;
inline constexpr const size_type max_width_width = 4;

inline constexpr const char g_digits_raw[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
inline constexpr const scan_type g_digits{yy_quad::make_const_span(g_digits_raw)};

static constexpr const char g_whitespace_raw[] = {'\000', ' ', '\t', '\n'};
static constexpr const scan_type g_whitespace{yy_quad::make_const_span(g_whitespace_raw)};

template<typename T,
         typename Enable = void>
struct is_string_param_traits:
      std::false_type
{
};

template<typename T>
struct is_string_param_traits<T,
                              std::enable_if_t<(yy_traits::is_vector_v<T>
                                                && std::is_same_v<char, typename T::value_type>)
                                               || yy_traits::is_std_string_v<T>
                                               || yy_traits::is_std_string_view_v<T>
                                               || std::is_same_v<const_span_type, T>>>:
      std::true_type
{
};

template<size_type N>
struct is_string_param_traits<char[N]>:
      std::true_type
{
};

template<typename T>
using is_string_param = is_string_param_traits<yy_traits::remove_cvr_t<T>>;

template<typename T>
inline constexpr bool is_string_param_v = is_string_param<T>::value;

template<typename T>
using is_string_param_t = typename is_string_param<T>::type;


size_type find_first_of(scan_type p_source,
                        const scan_type p_chars) noexcept;

size_type find_first_not_of(scan_type p_source,
                            const scan_type p_chars) noexcept;

struct capture_type final
{
    size_type count = 0;
    bool more = true;

    constexpr capture_type(size_type p_count,
                           bool p_more) noexcept:
      count(p_count),
      more(p_more)
    {
    }

    constexpr capture_type() noexcept = default;
    constexpr capture_type(const capture_type &) noexcept = default;
    constexpr capture_type(capture_type && ) noexcept = default;

    capture_type & operator=(const capture_type &) = delete;
    capture_type & operator=(capture_type && ) = delete;

    constexpr operator bool() const noexcept
    {
      return more;
    }

    constexpr capture_type & operator+=(const capture_type & p_other) noexcept
    {
      count += p_other.count;
      more = more && p_other.more;

      return * this;
    }
};

size_type get_width(scan_type & p_format) noexcept;
bool scan_leading(scan_type & p_source,
                  scan_type & p_format) noexcept;

void scan_span(scan_type & p_source,
               span_type p_dest) noexcept;

size_type scan_string(scan_type & p_source,
                      std::string & p_dest) noexcept;

template<size_type N>
inline size_type scan_string(scan_type & p_source,
                             char (&p_dest)[N]) noexcept
{
  if constexpr (N > 0)
  {
    scan_span(p_source, yy_quad::make_span(p_dest));
    return 1;
  }

  return 0;
}

template<typename T,
         std::enable_if_t<yy_traits::is_vector_v<T>
                          && std::is_same_v<char, typename T::value_type>, bool> = true>
inline size_type scan_string(scan_type & p_source,
                             T & p_dest) noexcept
{
  if(p_dest.size() > 0)
  {
    scan_span(p_source, yy_quad::make_span(p_dest));

    return 1;
  }

  return 0;
}

template<typename T,
         std::enable_if_t<yy_traits::is_std_string_view_v<T>
                          || std::is_same_v<const_span_type, T>, bool> = true>
inline size_type scan_string(scan_type & p_source,
                             T & p_dest) noexcept
{
  size_type copy_size = p_source.size();

  p_dest = T{p_source.data(), copy_size};

  return 1;
}

template<typename T,
         std::enable_if_t<is_string_param_v<T>, bool> = true>
inline capture_type scan(scan_type & p_source,
                         scan_type & p_format,
                         width_type & p_width,
                         T && p_value) noexcept
{
  size_type captured = 0;

  if(p_width.has_value()
     || scan_leading(p_source, p_format))
  {
    if(!p_format.empty()
       && !p_source.empty())
    {
      size_type width{p_width.has_value() ? p_width.value() : get_width(p_format)};
      p_width.reset();

      if('s' == p_format[0])
      {
        // Consume 's'.
        p_format.inc_begin();

        width = find_first_of(p_source.subspan(0, width), g_whitespace);

        scan_type l_source{p_source.data(), width};

        captured = scan_string(l_source, p_value);
        p_source.inc_begin(l_source.size());
        return capture_type{captured, 0 != captured};
      }
    }
  }

  return capture_type{0, false};
}

template<typename T,
         std::enable_if_t<std::is_integral_v<std::remove_reference_t<T>>, bool> = true>
inline capture_type scan(scan_type & p_source,
                         scan_type & p_format,
                         width_type & p_width,
                         T && p_value) noexcept
{
  using int_type = std::remove_reference_t<T>;
  using digits_type = yy_util::Digits<int_type>;

  if(p_width.has_value()
     || scan_leading(p_source, p_format))
  {
    if(!p_format.empty()
       && !p_source.empty())
    {
      size_type width{p_width.has_value() ? p_width.value() : std::min(digits_type::digits, get_width(p_format))};
      p_width.reset();

      switch(p_format[0])
      {
        case '*':
          // Consume '*'.
          p_format.inc_begin();
          p_width = std::min(max_width_width, static_cast<width_type::value_type>(p_value));
          return capture_type{0, true};

        case 'd':
        {
          // Consume 'd'.
          p_format.inc_begin();
          const bool is_negative = '-' == p_source[0];
          if(!std::numeric_limits<int_type>::is_signed && is_negative)
          {
            // Early exit.
            return capture_type{0, false};
          }

          if(is_negative)
          {
            p_source.inc_begin();
          }

          width = find_first_of(p_source.subspan(0, width), g_whitespace);
          scan_type l_source{p_source.data(), width};

          width = find_first_not_of(l_source, g_digits);
          l_source = scan_type{l_source.data(), width};

          if(auto [l_value, l_valid] = fast_atoi<int_type>::convert(l_source);
             FastFloat::Ok == l_valid)
          {
            p_value = l_value;
            if(is_negative)
            {
              p_value = -l_value;
            }
            p_source.inc_begin(l_source.size());
            return capture_type{1, true};
          }
        }
      }
    }
  }

  return capture_type{0, false};
}

} // namespace scan_str_detail

template<typename... Args>
inline size_type scan_str(scan_str_detail::scan_type p_source,
                          scan_str_detail::scan_type p_format,
                          Args && ...p_args) noexcept
{
  scan_str_detail::width_type l_width;
  scan_str_detail::capture_type captured{};

  (..., (captured && (captured += scan_str_detail::scan(p_source, p_format, l_width, p_args))));

  return captured.count;
}

template<typename... Args>
inline size_type scan_str(std::string_view p_source,
                          std::string_view p_format,
                          Args && ...p_args) noexcept
{
  return scan_str(yy_quad::make_const_span(p_source),
                  yy_quad::make_const_span(p_format),
                  std::forward<Args>(p_args)...);
}

} // namespace yafiyogi::yy_util
