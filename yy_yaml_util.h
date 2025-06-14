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
#include <optional>

#include "yaml-cpp/yaml.h"

#include "yy_cpp/yy_type_traits.h"
#include "yy_cpp/yy_traits_string.hpp"

namespace yafiyogi::yy_util {

inline bool yaml_is_map(const YAML::Node & node) noexcept
{
  return node && node.IsMap();
}

inline bool yaml_is_scalar(const YAML::Node & node) noexcept
{
  return node && node.IsScalar();
}

inline bool yaml_is_sequence(const YAML::Node & node) noexcept
{
  return node && node.IsSequence();
}

template<typename T,
         std::enable_if_t<!yy_traits::is_c_string_v<T>, bool> = true>
inline yy_traits::remove_cvr_t<T> yaml_get_value(const YAML::Node & node)
{
  if(yaml_is_scalar(node))
  {
    return node.as<yy_traits::remove_cvr_t<T>>();
  }

  return T{};
}

template<typename T,
         std::enable_if_t<yy_traits::is_c_string_v<T>, bool> = true>
inline std::string_view yaml_get_value(const YAML::Node & node)
{
  return yaml_get_value<std::string_view>(node);
}

template<typename T,
         std::enable_if_t<!yy_traits::is_std_string_v<T>
                          && !yy_traits::is_c_string_v<T>, bool> = true>
inline yy_traits::remove_cvr_t<T> yaml_get_value(const YAML::Node & node, T && value)
{
  if(yaml_is_scalar(node))
  {
    return node.as<yy_traits::remove_cvr_t<T>>();
  }

  return value;
}

template<typename T,
         std::enable_if_t<yy_traits::is_std_string_v<T>
                          || yy_traits::is_c_string_v<T>, bool> = true>
inline std::string_view yaml_get_value(const YAML::Node & node, T && value)
{
  return yaml_get_value(node, std::string_view{value});
}

template<typename T,
         std::enable_if_t<!yy_traits::is_c_string_v<T>, bool> = true>
inline std::optional<yy_traits::remove_cvr_t<T>> yaml_get_optional_value(const YAML::Node & node)
{
  std::optional<T> return_value{std::nullopt};
  if(yaml_is_scalar(node))
  {
    return_value = node.as<T>();
  }

  return return_value;
}

template<typename T,
         std::enable_if_t<yy_traits::is_c_string_v<T>, bool> = true>
inline std::optional<std::string_view> yaml_get_optional_value(const YAML::Node & node)
{
  return yaml_get_optional_value<std::string_view>(node);
}

} // namespace yafiyogi::yy_util
