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

#include <vector>

#include "gtest/gtest.h"

#include "yy_compare_util.h"


namespace yafiyogi::yy_cpp::tests {

class TestCompareUtil:
      public testing::Test
{
  public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
    using data_type = std::vector<int>;
    using char_data_type = std::vector<char>;
};

TEST_F(TestCompareUtil, equal_is_equal)
{
  data_type a{1, 2, 3, 4};
  data_type b{a};

  EXPECT_TRUE(yafiyogi::yy_util::equal(a, b));
}

TEST_F(TestCompareUtil, equal_different_size)
{
  data_type a{1, 2, 3, 4};
  data_type b{a};

  b.pop_back();

  EXPECT_FALSE(yafiyogi::yy_util::equal(a, b));
}

TEST_F(TestCompareUtil, equal_different_data)
{
  data_type a{1, 2, 3, 4};
  data_type b{1, 2, 4, 3};

  b.pop_back();

  EXPECT_FALSE(yafiyogi::yy_util::equal(a, b));
}

TEST_F(TestCompareUtil, char_equal_is_equal)
{
  char_data_type a{'1', '2', '3', '4'};
  char_data_type b{a};

  EXPECT_TRUE(yafiyogi::yy_util::equal(a, b));
}

TEST_F(TestCompareUtil, char_equal_different_size)
{
  char_data_type a{'1', '2', '3', '4'};
  char_data_type b{a};

  b.pop_back();

  EXPECT_FALSE(yafiyogi::yy_util::equal(a, b));
}

TEST_F(TestCompareUtil, char_equal_different_data)
{
  char_data_type a{'1', '2', '3', '4'};
  char_data_type b{'1', '2', '4', '3'};

  b.pop_back();

  EXPECT_FALSE(yafiyogi::yy_util::equal(a, b));
}

TEST_F(TestCompareUtil, less_than)
{
  {
    data_type a{1, 2, 3, 4};
    data_type b{1, 2, 3, 5};

    EXPECT_TRUE(yafiyogi::yy_util::less_than(a, b));
  }

  {
    data_type a{1, 2, 3};
    data_type b{1, 2, 3, 4};

    EXPECT_TRUE(yafiyogi::yy_util::less_than(a, b));
  }
}

TEST_F(TestCompareUtil, less_than_equal)
{
  data_type a{1, 2, 3, 4};
  data_type b{a};

  EXPECT_FALSE(yafiyogi::yy_util::less_than(a, b));
}

TEST_F(TestCompareUtil, less_than_greater)
{
  {
    data_type a{1, 2, 3, 5};
    data_type b{1, 2, 3, 4};

    EXPECT_FALSE(yafiyogi::yy_util::less_than(a, b));
  }

  {
    data_type a{1, 2, 3, 4};
    data_type b{1, 2, 3};

    EXPECT_FALSE(yafiyogi::yy_util::less_than(a, b));
  }
}

TEST_F(TestCompareUtil, char_less_than)
{
  {
    char_data_type a{1, 2, 3, 4};
    char_data_type b{1, 2, 3, 5};

    EXPECT_TRUE(yafiyogi::yy_util::less_than(a, b));
  }

  {
    char_data_type a{1, 2, 3};
    char_data_type b{1, 2, 3, 4};

    EXPECT_TRUE(yafiyogi::yy_util::less_than(a, b));
  }
}

TEST_F(TestCompareUtil, char_less_than_equal)
{
  char_data_type a{1, 2, 3, 4};
  char_data_type b{a};

  EXPECT_FALSE(yafiyogi::yy_util::less_than(a, b));
}

TEST_F(TestCompareUtil, char_less_than_greater)
{
  {
    char_data_type a{1, 2, 3, 5};
    char_data_type b{1, 2, 3, 4};

    EXPECT_FALSE(yafiyogi::yy_util::less_than(a, b));
  }

  {
    char_data_type a{1, 2, 3, 4};
    char_data_type b{1, 2, 3};

    EXPECT_FALSE(yafiyogi::yy_util::less_than(a, b));
  }
}

} // namespace yafiyogi::yy_cpp::tests
