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
#include "fmt/format.h"
#include "gtest/gtest.h"

#include "yy_flat_set.h"

namespace yafiyogi::yy_cpp::tests {

class TestSetMap:
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


TEST_F(TestSetMap, LessThanSimple)
{
  {
  yy_data::flat_set<int> vec_1{};
  yy_data::flat_set<int> vec_2{};

    EXPECT_FALSE(vec_1 < vec_2);
  }

  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_2.emplace(1);

    EXPECT_TRUE(vec_1 < vec_2);
  }

  // vec_1 == vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);

    vec_2.emplace(1);

    EXPECT_FALSE(vec_1 < vec_2);
  }

  // vec_1 < vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);

    vec_2.emplace(2);

    EXPECT_TRUE(vec_1 < vec_2);
  }

  // vec_1 > vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(2);

    vec_2.emplace(1);

    EXPECT_FALSE(vec_1 < vec_2);
  }
}

TEST_F(TestSetMap, EqualToMultiple)
{
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);

    vec_2.emplace(1);
    vec_2.emplace(2);

    EXPECT_FALSE(vec_1 == vec_2);
  }

  // vec_1 == vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);
    vec_1.emplace(2);

    vec_2.emplace(1);
    vec_2.emplace(2);

    EXPECT_TRUE(vec_1 == vec_2);
  }

  // vec_1 < vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);
    vec_1.emplace(2);

    vec_2.emplace(1);
    vec_2.emplace(3);

    EXPECT_FALSE(vec_1 == vec_2);
  }

  // vec_1 > vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);
    vec_1.emplace(3);

    vec_2.emplace(1);
    vec_2.emplace(2);

    EXPECT_FALSE(vec_1 == vec_2);
  }
}

TEST_F(TestSetMap, EqualToSimple)
{
  {
  yy_data::flat_set<int> vec_1{};
  yy_data::flat_set<int> vec_2{};

    EXPECT_TRUE(vec_1 == vec_2);
  }

  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_2.emplace(1);

    EXPECT_FALSE(vec_1 == vec_2);
  }

  // vec_1 == vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);

    vec_2.emplace(1);

    EXPECT_TRUE(vec_1 == vec_2);
  }

  // vec_1 < vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);

    vec_2.emplace(2);

    EXPECT_FALSE(vec_1 == vec_2);
  }

  // vec_1 > vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(2);

    vec_2.emplace(1);

    EXPECT_FALSE(vec_1 == vec_2);
  }
}

TEST_F(TestSetMap, LessThanMultiple)
{
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    EXPECT_FALSE(vec_1 < vec_2);
  }

  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);

    vec_2.emplace(1);
    vec_2.emplace(2);

    EXPECT_TRUE(vec_1 < vec_2);
  }

  // vec_1 == vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);
    vec_1.emplace(2);

    vec_2.emplace(1);
    vec_2.emplace(2);

    EXPECT_FALSE(vec_1 < vec_2);
  }

  // vec_1 < vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);
    vec_1.emplace(2);

    vec_2.emplace(1);
    vec_2.emplace(3);

    EXPECT_TRUE(vec_1 < vec_2);
  }

  // vec_1 > vec_2
  {
    yy_data::flat_set<int> vec_1{};
    yy_data::flat_set<int> vec_2{};

    vec_1.emplace(1);
    vec_1.emplace(3);

    vec_2.emplace(1);
    vec_2.emplace(2);

    EXPECT_FALSE(vec_1 < vec_2);
  }
}

} // namespace yafiyogi::yy_cpp::tests
