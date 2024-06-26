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

#include "fmt/format.h"

#include <gtest/gtest.h>

#include "yy_bit_twiddling.h"

namespace yafiyogi::yy_cpp::tests {

class TestBitTwiddling:
      public testing::Test
{
  public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TestBitTwiddling, test_round_up_pow2)
{
  uint64_t one_64{1};

  for(std::uint64_t pow2 = 0; pow2 < 2; ++pow2)
  {
    std::uint64_t val = one_64 << pow2;

    EXPECT_EQ(val, yy_bit_twiddling::round_up_pow2(val));
  }

  for(std::uint64_t pow2 = 3; pow2 < 64; ++pow2)
  {
    std::uint64_t val = one_64 << pow2;

    EXPECT_EQ(val, yy_bit_twiddling::round_up_pow2(val));
    EXPECT_EQ(val, yy_bit_twiddling::round_up_pow2(val - 1));
  }
}

} // namespace yafiyogi::yy_cpp::tests
