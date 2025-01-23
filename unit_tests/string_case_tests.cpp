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

#include <string>

#include "gtest/gtest.h"

#include "yy_string_case.h"

namespace yafiyogi::yy_cpp::tests {

class TestStringCase:
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

TEST_F(TestStringCase, to_lower)
{
  EXPECT_EQ("123456", yafiyogi::yy_util::to_lower("123456"));
  EXPECT_EQ("abcdef", yafiyogi::yy_util::to_lower("abcdef"));
  EXPECT_EQ("abcdef", yafiyogi::yy_util::to_lower("ABCDEF"));
  EXPECT_EQ("abcdef", yafiyogi::yy_util::to_lower("AbCdEf"));
}

TEST_F(TestStringCase, to_upper)
{
  EXPECT_EQ("123456", yafiyogi::yy_util::to_upper("123456"));
  EXPECT_EQ("ABCDEF", yafiyogi::yy_util::to_upper("ABCDEF"));
  EXPECT_EQ("ABCDEF", yafiyogi::yy_util::to_upper("abcdef"));
  EXPECT_EQ("ABCDEF", yafiyogi::yy_util::to_upper("AbCdEf"));
}

} // namespace yafiyogi::yy_cpp::tests
