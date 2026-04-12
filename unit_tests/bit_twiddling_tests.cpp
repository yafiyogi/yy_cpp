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

#include <array>
#include <bit>
#include <bitset>
#include <limits>
#include <random>

#include "gtest/gtest.h"

#include "yy_bit_twiddling.h"

inline constexpr const int max_size = 10000;

template<typename T>
static std::array<T, max_size> set_numbers()
{
  std::array<T, max_size> num;

  std::random_device rd{};
  std::mt19937 generator{rd()};

  std::uniform_int_distribution<uint64_t> uniform_dist{1, std::numeric_limits<T>::max()};

  for(size_t idx = 0; idx < num.size(); ++idx)
  {
    num[idx] = static_cast<T>(uniform_dist(generator));
  }

  return num;
}

namespace yafiyogi::yy_cpp::tests {

class TestBitTwiddling_64:
      public testing::Test
{
  public:
    using bit_twiddling = yafiyogi::yy_bit_twiddling::bits<uint64_t>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TestBitTwiddling_64, test_round_up_pow2)
{
  uint64_t one_64{1};

  for(std::uint64_t pow2 = 0; pow2 < 2; ++pow2)
  {
    std::uint64_t val = one_64 << pow2;

    EXPECT_EQ(val, bit_twiddling::round_up_pow2(val));
  }

  for(std::uint64_t pow2 = 3; pow2 < 64; ++pow2)
  {
    std::uint64_t val = one_64 << pow2;

    EXPECT_EQ(val, bit_twiddling::round_up_pow2(val));
    EXPECT_EQ(std::bit_ceil(val), bit_twiddling::round_up_pow2(val));
    EXPECT_EQ(val, bit_twiddling::round_up_pow2(val - 1));
    EXPECT_EQ(std::bit_ceil(val - 1), bit_twiddling::round_up_pow2(val - 1));
  }
}

TEST_F(TestBitTwiddling_64, test_round_down_pow2)
{
  uint64_t one_64{1};

  for(std::uint64_t pow2 = 0; pow2 < 2; ++pow2)
  {
    std::uint64_t val = one_64 << pow2;

    EXPECT_EQ(val, bit_twiddling::round_down_pow2(val));
  }

  for(std::uint64_t pow2 = 3; pow2 < 64; ++pow2)
  {
    std::uint64_t val = one_64 << pow2;

    EXPECT_EQ(val, bit_twiddling::round_down_pow2(val));
    EXPECT_EQ(std::bit_floor(val), bit_twiddling::round_down_pow2(val));
    EXPECT_EQ(std::bit_floor(val - 1), bit_twiddling::round_down_pow2(val - 1));
  }
}

class TestBitTwiddling_8:
      public testing::Test
{
  public:
    using bit_twiddling = yafiyogi::yy_bit_twiddling::bits<uint8_t>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TestBitTwiddling_8, test_round_up_pow2)
{
  uint8_t one_8{1};

  for(std::uint8_t pow2 = 0; pow2 < 2; ++pow2)
  {
    std::uint8_t val = one_8 << pow2;

    EXPECT_EQ(val, bit_twiddling::round_up_pow2(val));
  }

  for(std::uint8_t pow2 = 3; pow2 < 8; ++pow2)
  {
    std::uint8_t val = one_8 << pow2;

    EXPECT_EQ(val, bit_twiddling::round_up_pow2(val));
    EXPECT_EQ(std::bit_ceil(val), bit_twiddling::round_up_pow2(val));
    EXPECT_EQ(val, bit_twiddling::round_up_pow2(val - 1));
    EXPECT_EQ(std::bit_ceil(static_cast<uint64_t>(val) - 1), bit_twiddling::round_up_pow2(val - 1));
  }
}

TEST_F(TestBitTwiddling_8, test_round_down_pow2)
{
  uint8_t one_8{1};

  for(std::uint8_t pow2 = 0; pow2 < 2; ++pow2)
  {
    std::uint8_t val = one_8 << pow2;

    EXPECT_EQ(val, bit_twiddling::round_down_pow2(val));
  }

  for(std::uint8_t pow2 = 3; pow2 < 8; ++pow2)
  {
    std::uint8_t val = one_8 << pow2;

    EXPECT_EQ(val, bit_twiddling::round_down_pow2(val));
    EXPECT_EQ(std::bit_floor(static_cast<uint64_t>(val)), bit_twiddling::round_down_pow2(val));
    EXPECT_EQ(std::bit_floor(static_cast<uint64_t>(val) - 1), bit_twiddling::round_down_pow2(val - 1));
  }
}

TEST_F(TestBitTwiddling_8, test_nlz_8)
{
  for(uint64_t num = 0; num < 256; ++num)
  {
    const uint8_t num_8 = static_cast<uint8_t>(num);

    ASSERT_EQ(std::countl_zero(static_cast<uint8_t>(num_8)), bit_twiddling::nlz(static_cast<uint8_t>(num_8)));
  }
}


TEST_F(TestBitTwiddling_8, test_ntz_8)
{
  for(uint64_t num = 1; num < 256; ++num)
  {
    const uint8_t num_8 = static_cast<uint8_t>(num);

    ASSERT_EQ(std::countr_zero(num_8), bit_twiddling::ntz(num_8));
  }
}

} // namespace yafiyogi::yy_cpp::tests
