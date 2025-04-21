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
#include <limits>

namespace yafiyogi::yy_bit_twiddling {

// From https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
constexpr uint64_t round_up_pow2(uint64_t val)
{
  --val;
  val |= val >> uint64_t{1};
  val |= val >> uint64_t{2};
  val |= val >> uint64_t{4};
  val |= val >> uint64_t{8};
  val |= val >> uint64_t{16};
  val |= val >> uint64_t{32};
  ++val;

  return val;
}

constexpr uint64_t round_down_pow2(uint64_t val)
{
  val |= val >> uint64_t{1};
  val |= val >> uint64_t{2};
  val |= val >> uint64_t{4};
  val |= val >> uint64_t{8};
  val |= val >> uint64_t{16};
  val |= val >> uint64_t{32};

  return val - (val >> uint64_t{1});
}

constexpr uint64_t pop(uint64_t val)
{
  val = val - ((val >> uint64_t{1}) & uint64_t{0x5555555555555555});
  val = (val & uint64_t{0x3333333333333333}) + ((val >> uint64_t{2}) & uint64_t{0x3333333333333333});
  return (((val + (val >> uint64_t{4})) & uint64_t{0xF0F0F0F0F0F0F0F}) * uint64_t{0x101010101010101}) >> uint64_t{56};
}

constexpr uint64_t nlz(uint64_t val)
{
  val |= val >> uint64_t{1};
  val |= val >> uint64_t{2};
  val |= val >> uint64_t{4};
  val |= val >> uint64_t{8};
  val |= val >> uint64_t{16};
  val |= val >> uint64_t{32};

  return pop(~val);
}

inline constexpr uint64_t all_bits{std::numeric_limits<uint64_t>::max()};

constexpr uint64_t ntz(uint64_t val)
{
  uint64_t mask = all_bits + static_cast<uint64_t>(0 == val);
  return mask & (uint64_t{63} - nlz(val));
}

} // namespace yafiyogi::yy_bit_twiddling
