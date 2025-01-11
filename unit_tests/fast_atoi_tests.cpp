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

#include <string_view>

#include "gtest/gtest.h"

#include "yy_fast_atoi.h"

namespace yafiyogi::yy_cpp::tests {

class TestFastAtoi:
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

TEST_F(TestFastAtoi, fast_atoi_ok)
{
  using fast_atoi = yafiyogi::yy_util::fast_atoi<size_t>;
  using rv_t = fast_atoi::val_valid_type;

  EXPECT_EQ((rv_t{12345678901234567890UL, yafiyogi::yy_util::FastFloatRV::Ok}), fast_atoi::convert(std::string_view{"12345678901234567890"}));
}

TEST_F(TestFastAtoi, fast_atoi_no_value)
{
  using fast_atoi = yafiyogi::yy_util::fast_atoi<int>;
  using rv_t = fast_atoi::val_valid_type;

  EXPECT_EQ((rv_t{0, yafiyogi::yy_util::FastFloatRV::NoValue}), fast_atoi::convert(std::string_view{"12345678901234567890"}));
}

TEST_F(TestFastAtoi, fast_atoi_overflow)
{
  using fast_atoi = yafiyogi::yy_util::fast_atoi<size_t>;
  using rv_t = fast_atoi::val_valid_type;

  EXPECT_EQ((rv_t{0, yafiyogi::yy_util::FastFloatRV::NoValue}), fast_atoi::convert(std::string_view{"123456789012345678901"}));
}

} // namespace yafiyogi::yy_cpp::tests
