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

#include "yy_test/yy_test_count.h"

#include "yy_vector.h"

namespace yafiyogi::yy_cpp::tests {

class TestVectorIter:
      public testing::Test
{
  public:
    using vector = yy_quad::vector<int>;

    void SetUp() override
    {
      yy_test::TestCount::reset();
    }

    void TearDown() override
    {
    }
};

TEST_F(TestVectorIter, begin_end_default)
{
  vector vec{};

  ASSERT_EQ(nullptr, vec.data());
  EXPECT_EQ(0, vec.size());
  EXPECT_EQ(0, vec.capacity());
  EXPECT_TRUE(vec.empty());

  EXPECT_EQ(vec.begin(), vec.end());
}

TEST_F(TestVectorIter, begin_inc)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(vec.begin(), vec.end());

  {
    auto iter = vec.begin();
    ++iter;
    EXPECT_EQ(iter, vec.end());
  }

  {
    auto iter = vec.begin();
    auto rv = iter++;
    EXPECT_EQ(iter, vec.end());
    EXPECT_EQ(rv, vec.begin());
  }
}

TEST_F(TestVectorIter, end_dec)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(vec.begin(), vec.end());

  {
    auto iter = vec.end();
    --iter;
    EXPECT_EQ(iter, vec.begin());
  }

  {
    auto iter = vec.end();
    auto rv = iter--;
    EXPECT_EQ(iter, vec.begin());
    EXPECT_EQ(rv, vec.end());
  }
}

TEST_F(TestVectorIter, offset_plus)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(vec.begin(), vec.end());

  {
    auto iter = vec.begin();
    iter += 1;
    EXPECT_EQ(iter, vec.end());
  }

  {
    auto iter = vec.begin();
    iter += ssize_type{1};
    EXPECT_EQ(iter, vec.end());
  }

  {
    auto iter = vec.begin();
    iter += size_type{1};
    EXPECT_EQ(iter, vec.end());
  }

  {
    auto iter_1 = vec.begin();
    auto iter_2 = iter_1 + 1;
    EXPECT_EQ(iter_2, vec.end());
  }

  {
    auto iter_1 = vec.begin();
    auto iter_2 = iter_1 + ssize_type{1};
    EXPECT_EQ(iter_2, vec.end());
  }

  {
    auto iter_1 = vec.begin();
    auto iter_2 = iter_1 + size_type{1};
    EXPECT_EQ(iter_2, vec.end());
  }
}

TEST_F(TestVectorIter, offset_minus)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(vec.begin(), vec.end());

  {
    auto iter = vec.end();
    iter -= 1;
    EXPECT_EQ(iter, vec.begin());
  }

  {
    auto iter = vec.end();
    iter -= ssize_type{1};
    EXPECT_EQ(iter, vec.begin());
  }

  {
    auto iter = vec.end();
    iter -= size_type{1};
    EXPECT_EQ(iter, vec.begin());
  }

  {
    auto iter_1 = vec.end();
    auto iter_2 = iter_1 - 1;
    EXPECT_EQ(iter_2, vec.begin());
  }

  {
    auto iter_1 = vec.end();
    auto iter_2 = iter_1 - ssize_type{1};
    EXPECT_EQ(iter_2, vec.begin());
  }

  {
    auto iter_1 = vec.end();
    auto iter_2 = iter_1 - size_type{1};
    EXPECT_EQ(iter_2, vec.begin());
  }
}

TEST_F(TestVectorIter, distance)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(vec.begin(), vec.end());

  auto iter = vec.begin() + 1;

  EXPECT_EQ(1, (iter - vec.begin()));
}

TEST_F(TestVectorIter, const_begin_end_default)
{
  vector vec{};
  const auto & const_vec = vec;

  ASSERT_EQ(nullptr, vec.data());
  EXPECT_EQ(0, vec.size());
  EXPECT_EQ(0, vec.capacity());
  EXPECT_TRUE(vec.empty());

  EXPECT_EQ(const_vec.begin(), const_vec.end());
}

TEST_F(TestVectorIter, const_begin_inc)
{
  vector vec{};
  const auto & const_vec = vec;

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(const_vec.begin(), const_vec.end());

  {
    auto iter = const_vec.begin();
    ++iter;
    EXPECT_EQ(iter, const_vec.end());
  }

  {
    auto iter = const_vec.begin();
    auto rv = iter++;
    EXPECT_EQ(iter, const_vec.end());
    EXPECT_EQ(rv, const_vec.begin());
  }
}

TEST_F(TestVectorIter, const_end_dec)
{
  vector vec{};
  const auto & const_vec = vec;

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(const_vec.begin(), const_vec.end());

  {
    auto iter = const_vec.end();
    --iter;
    EXPECT_EQ(iter, const_vec.begin());
  }

  {
    auto iter = const_vec.end();
    auto rv = iter--;
    EXPECT_EQ(iter, const_vec.begin());
    EXPECT_EQ(rv, const_vec.end());
  }
}

TEST_F(TestVectorIter, const_offset_plus)
{
  vector vec{};
  const auto & const_vec = vec;

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(const_vec.begin(), const_vec.end());

  {
    auto iter = const_vec.begin();
    iter += 1;
    EXPECT_EQ(iter, const_vec.end());
  }

  {
    auto iter = const_vec.begin();
    iter += ssize_type{1};
    EXPECT_EQ(iter, const_vec.end());
  }

  {
    auto iter = const_vec.begin();
    iter += size_type{1};
    EXPECT_EQ(iter, const_vec.end());
  }

  {
    auto iter_1 = const_vec.begin();
    auto iter_2 = iter_1 + 1;
    EXPECT_EQ(iter_2, const_vec.end());
  }

  {
    auto iter_1 = const_vec.begin();
    auto iter_2 = iter_1 + ssize_type{1};
    EXPECT_EQ(iter_2, const_vec.end());
  }

  {
    auto iter_1 = const_vec.begin();
    auto iter_2 = iter_1 + size_type{1};
    EXPECT_EQ(iter_2, const_vec.end());
  }
}

TEST_F(TestVectorIter, const_offset_minus)
{
  vector vec{};
  const auto & const_vec = vec;

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(const_vec.begin(), const_vec.end());

  {
    auto iter = const_vec.end();
    iter -= 1;
    EXPECT_EQ(iter, const_vec.begin());
  }

  {
    auto iter = const_vec.end();
    iter -= ssize_type{1};
    EXPECT_EQ(iter, const_vec.begin());
  }

  {
    auto iter = const_vec.end();
    iter -= size_type{1};
    EXPECT_EQ(iter, const_vec.begin());
  }

  {
    auto iter_1 = const_vec.end();
    auto iter_2 = iter_1 - 1;
    EXPECT_EQ(iter_2, const_vec.begin());
  }

  {
    auto iter_1 = const_vec.end();
    auto iter_2 = iter_1 - ssize_type{1};
    EXPECT_EQ(iter_2, const_vec.begin());
  }

  {
    auto iter_1 = const_vec.end();
    auto iter_2 = iter_1 - size_type{1};
    EXPECT_EQ(iter_2, const_vec.begin());
  }
}

TEST_F(TestVectorIter, const_distance)
{
  vector vec{};
  const auto & const_vec = vec;

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_EQ(2, vec.capacity());
  EXPECT_FALSE(vec.empty());

  EXPECT_NE(const_vec.begin(), const_vec.end());

  auto iter = const_vec.begin() + 1;

  EXPECT_EQ(1, (iter - const_vec.begin()));
}

} // namespace yafiyogi::yy_cpp::tests
