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

template <typename T>
struct bits
{
};

template<>
struct bits<uint64_t>
{
    using value_type = uint64_t;
    // From https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    static constexpr uint64_t round_up_pow2(value_type val)
    {
      --val;
      val |= val >> value_type{1};
      val |= val >> value_type{2};
      val |= val >> value_type{4};
      val |= val >> value_type{8};
      val |= val >> value_type{16};
      val |= val >> value_type{32};
      ++val;

      return val;
    }

    static constexpr uint64_t round_down_pow2(value_type val)
    {
      val |= val >> value_type{1};
      val |= val >> value_type{2};
      val |= val >> value_type{4};
      val |= val >> value_type{8};
      val |= val >> value_type{16};
      val |= val >> value_type{32};

      return val - (val >> value_type{1});
    }

    static constexpr int pop(value_type val)
    {
      val = val - ((val >> value_type{1}) & value_type{0x5555555555555555});
      val = (val & value_type{0x3333333333333333}) + ((val >> value_type{2}) & value_type{0x3333333333333333});
      return static_cast<int>((((val + (val >> value_type{4})) & value_type{0xF0F0F0F0F0F0F0F}) * value_type{0x101010101010101}) >> value_type{56});
    }

    static constexpr int nlz(value_type val)
    {
      val |= val >> value_type{1};
      val |= val >> value_type{2};
      val |= val >> value_type{4};
      val |= val >> value_type{8};
      val |= val >> value_type{16};
      val |= val >> value_type{32};

      return pop(~val);
    }

    static constexpr int ntz(value_type val)
    {
      return std::numeric_limits<value_type>::digits - pop(val | -val);
    }
};

template<>
struct bits<uint8_t>
{
    using value_type = uint8_t;

    // From https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    static constexpr value_type round_up_pow2(value_type val)
    {
      --val;
      val |= val >> value_type{1};
      val |= val >> value_type{2};
      val |= val >> value_type{4};
      ++val;

      return val;
    }

    static constexpr value_type round_down_pow2(value_type val)
    {
      val |= val >> value_type{1};
      val |= val >> value_type{2};
      val |= val >> value_type{4};

      return val - (val >> value_type{1});
    }

    static constexpr int pop(value_type val)
    {
      val = val - ((val >> value_type{1}) & value_type{0x55});
      val = (val & value_type{0x33}) + ((val >> value_type{2}) & value_type{0x33});
      return ((val + (val >> value_type{4})) & value_type{0x0F}) * value_type{0x01};
    }

    static constexpr int nlz(value_type val)
    {
      val |= val >> value_type{1};
      val |= val >> value_type{2};
      val |= val >> value_type{4};

      return pop(static_cast<value_type>(~val));;
    }

    static constexpr int ntz(value_type val)
    {
      return std::numeric_limits<value_type>::digits - pop(val | -val);
    }
};

} // namespace yafiyogi::yy_bit_twiddling
