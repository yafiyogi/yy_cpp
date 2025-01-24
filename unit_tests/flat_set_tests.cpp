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

#include "gtest/gtest.h"

#include "yy_flat_set.h"

namespace yafiyogi::yy_cpp::tests {

class TestFlatSet:
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


TEST_F(TestFlatSet, LessThanSimple)
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

TEST_F(TestFlatSet, EqualToMultiple)
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

TEST_F(TestFlatSet, EqualToSimple)
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

TEST_F(TestFlatSet, LessThanMultiple)
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

TEST_F(TestFlatSet, find)
{
  yy_data::flat_set<int> vec{};

  vec.emplace(1);
  vec.emplace(2);
  vec.emplace(3);

  {
    auto [value, pos] = vec.find(2);
    ASSERT_NE(nullptr, value);
    EXPECT_EQ(2, *value);
    EXPECT_EQ(1, pos);
  }

  {
    auto [value, pos] = vec.find(0);
    ASSERT_EQ(nullptr, value);
    EXPECT_EQ(0, pos);
  }

  {
    auto [value, pos] = vec.find(4);
    ASSERT_EQ(nullptr, value);
    EXPECT_EQ(3, pos);
  }
}

TEST_F(TestFlatSet, const_find)
{
  yy_data::flat_set<int> vec{};
  const yy_data::flat_set<int> & const_vec = vec;

  vec.emplace(1);
  vec.emplace(2);
  vec.emplace(3);

  {
    auto [value, pos] = const_vec.find(2);
    ASSERT_NE(nullptr, value);
    EXPECT_EQ(2, *value);
    EXPECT_EQ(1, pos);
  }

  {
    auto [value, pos] = const_vec.find(0);
    ASSERT_EQ(nullptr, value);
    EXPECT_EQ(0, pos);
  }

  {
    auto [value, pos] = const_vec.find(10);
    ASSERT_EQ(nullptr, value);
    EXPECT_EQ(3, pos);
  }
}

TEST_F(TestFlatSet, find_value)
{
  yy_data::flat_set<int> vec{};

  vec.emplace(1);
  vec.emplace(2);
  vec.emplace(3);

  {
    auto [found_pos, found] = vec.find_value([](auto * value, auto pos) {
      ASSERT_NE(nullptr, value);
      EXPECT_EQ(2, *value);
      EXPECT_EQ(1, pos);
    }, 2);

    EXPECT_EQ(1, found_pos);
    EXPECT_TRUE(found);
  }

  {
    auto [found_pos, found] = vec.find_value([](auto * /* value */, auto /* pos */) {
      EXPECT_FALSE(true);
    }, 0);

    EXPECT_EQ(0, found_pos);
    EXPECT_FALSE(found);
  }

  {
    auto [found_pos, found] = vec.find_value([](auto * /* value */, auto /* pos */) {
      EXPECT_FALSE(true);
    }, 10);

    EXPECT_EQ(3, found_pos);
    EXPECT_FALSE(found);
  }
}

TEST_F(TestFlatSet, const_find_value)
{
  yy_data::flat_set<int> vec{};
  const yy_data::flat_set<int> & const_vec = vec;

  vec.emplace(1);
  vec.emplace(2);
  vec.emplace(3);

  {
    auto [found_pos, found] = const_vec.find_value([](auto * value, auto pos) {
      ASSERT_NE(nullptr, value);
      EXPECT_EQ(2, *value);
      EXPECT_EQ(1, pos);
    }, 2);

    EXPECT_EQ(1, found_pos);
    EXPECT_TRUE(found);
  }

  {
    auto [found_pos, found] = const_vec.find_value([](auto * /* value */, auto /* pos */) {
      EXPECT_FALSE(true);
    }, 0);

    EXPECT_EQ(0, found_pos);
    EXPECT_FALSE(found);
  }

  {
    auto [found_pos, found] = const_vec.find_value([](auto * /* value */, auto /* pos */) {
      EXPECT_FALSE(true);
    }, 10);

    EXPECT_EQ(3, found_pos);
    EXPECT_FALSE(found);
  }
}

} // namespace yafiyogi::yy_cpp::tests
