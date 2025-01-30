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

#include <cstdint>

#include <type_traits>

#include "yy_type_traits.h"

namespace yafiyogi::yy_data {

template<typename T>
using ClearActionType = yy_traits::remove_cvr_t<T>;

enum class ClearAction:uint8_t {Clear, Keep};

template<typename T>
inline constexpr const ClearAction default_clear_action_v =
  (std::is_trivially_constructible_v<ClearActionType<T>>
   && std::is_trivially_move_assignable_v<ClearActionType<T>>
   && std::is_trivially_copy_assignable_v<ClearActionType<T>>) ? ClearAction::Keep : ClearAction::Clear;


} // namespace yafiyogi::yy_data
