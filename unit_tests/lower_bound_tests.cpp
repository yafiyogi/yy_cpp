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
#include "yy_lower_bound.h"

#include <gtest/gtest.h>


namespace yafiyogi::yy_cpp::tests {

class TestLowerBound:
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

TEST_F(TestLowerBound, yy_vs_std)
{
  std::vector<int> vec = {2, 3, 4, 5, 7, 8};

  for(int val = 0; val < (vec.back() + 2); ++val)
  {
    auto std_lb = std::lower_bound(vec.data(), vec.data() + vec.size(), val);
    auto yy_lb = yy_data::lower_bound(vec.data(), vec.data() + vec.size(), val);
    EXPECT_EQ(std_lb, yy_lb);

    if(val <= vec.back())
    {
      EXPECT_EQ(*std_lb, *yy_lb);
    }
  }
}

TEST_F(TestLowerBound, yy_test_lb)
{
  std::vector<size_t> vec = {2, 3, 4, 5, 7, 8};
  std::vector<int> result = {0, 0, 0, 1, 2, 3, 4, 4, 5, 6, 6};

  for(size_t val = 0; val < result.size(); ++val)
  {
    auto yy_lb = yy_data::lower_bound(vec.data(), vec.data() + vec.size(), val);
    EXPECT_EQ(vec.data() + result[val], yy_lb);
  }
}

} // namespace yafiyogi::yy_cpp::tests
