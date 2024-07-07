/*

  MIT License

  Copyright (c) 2024 Yafiyogi

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

namespace yafiyogi::yy_bit_twiddling {

// From https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
constexpr uint64_t round_up_pow2(uint64_t v)
{
  --v;
  v |= v >> uint64_t{1};
  v |= v >> uint64_t{2};
  v |= v >> uint64_t{4};
  v |= v >> uint64_t{8};
  v |= v >> uint64_t{16};
  v |= v >> uint64_t{32};
  ++v;

  return v;
}

constexpr uint64_t round_down_pow2(uint64_t v)
{
  v |= v >> uint64_t{1};
  v |= v >> uint64_t{2};
  v |= v >> uint64_t{4};
  v |= v >> uint64_t{8};
  v |= v >> uint64_t{16};
  v |= v >> uint64_t{32};

  return v - (v >> 1);
}

constexpr uint64_t pop(uint64_t v)
{
  v = v - ((v >> uint64_t{1}) & uint64_t{0x5555555555555555});
  v = (v & uint64_t{0x3333333333333333}) + ((v >> uint64_t{2}) & uint64_t{0x3333333333333333});
  return (((v + (v >> uint64_t{4})) & uint64_t{0xF0F0F0F0F0F0F0F}) * uint64_t{0x101010101010101}) >> uint64_t{56};
}

constexpr uint64_t nlz(uint64_t v)
{
  v |= v >> uint64_t{1};
  v |= v >> uint64_t{2};
  v |= v >> uint64_t{4};
  v |= v >> uint64_t{8};
  v |= v >> uint64_t{16};
  v |= v >> uint64_t{32};

  return pop(~v);
}

inline constexpr uint64_t all_bits{uint64_t{0} - uint64_t{1}};

constexpr uint64_t ntz(uint64_t v)
{
  uint64_t mask = all_bits + static_cast<uint64_t>(0 == v);
  return mask & (uint64_t{63} - nlz(v));
}

} // namespace yafiyogi::yy_bit_twiddling
