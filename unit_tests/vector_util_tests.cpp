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
#include <vector>

#include <gtest/gtest.h>

#include "yy_vector_util.h"

namespace yafiyogi::yy_data::tests {

class TestVectorUtil:
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

TEST_F(TestVectorUtil, TestFindInt)
{
  std::vector<int> v{1, 2, 3, 4, 6, 8, 10};

  EXPECT_TRUE(std::get<1>(yy_util::find(v, 4)));
  EXPECT_TRUE(std::get<1>(yy_util::find(v, 1)));
  EXPECT_TRUE(std::get<1>(yy_util::find(v, 10)));
  EXPECT_FALSE(std::get<1>(yy_util::find(v, 0)));
  EXPECT_FALSE(std::get<1>(yy_util::find(v, 5)));
  EXPECT_FALSE(std::get<1>(yy_util::find(v, 12)));
}

TEST_F(TestVectorUtil, TestFindString)
{
  std::vector<std::string> v{"aa", "c", "da", "db"};

  EXPECT_TRUE(std::get<1>(yy_util::find(v, std::string("aa"))));
  EXPECT_TRUE(std::get<1>(yy_util::find(v, std::string("c"))));
  EXPECT_TRUE(std::get<1>(yy_util::find(v, std::string("db"))));
  EXPECT_FALSE(std::get<1>(yy_util::find(v, std::string("a"))));
  EXPECT_FALSE(std::get<1>(yy_util::find(v, std::string("b"))));
  EXPECT_FALSE(std::get<1>(yy_util::find(v, std::string("z"))));
}

TEST_F(TestVectorUtil, TestSortInt)
{
  std::vector<int> v{6, 2, 4, 10, 8};
  const std::vector<int> result{2, 4, 6, 8, 10};

  yy_util::sort(v);

  EXPECT_EQ(v, result);
}

} // Namespace yafiyogi::yy_data::tests
