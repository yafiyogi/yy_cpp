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

class TestSpanIter:
      public testing::Test
{
  public:
    using vector = yy_quad::vector<int>;
    using span_type = yy_quad::span<int>;
    using const_span_type = yy_quad::const_span<int>;
    using ssize_type = span_type::ssize_type;
    using size_type = span_type::size_type;

    void SetUp() override
    {
      yy_test::TestCount::reset();
    }

    void TearDown() override
    {
    }
};

TEST_F(TestSpanIter, begin_end_default)
{
  vector vec{};

  ASSERT_EQ(nullptr, vec.data());
  EXPECT_EQ(0, vec.size());
  EXPECT_TRUE(vec.empty());

  auto span = yy_quad::make_span(vec);
  EXPECT_EQ(span.begin(), span.end());
}

TEST_F(TestSpanIter, begin_inc)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto span = yy_quad::make_span(vec);
  EXPECT_NE(span.begin(), span.end());

  {
    auto iter = span.begin();
    ++iter;
    EXPECT_EQ(iter, span.end());
  }

  {
    auto iter = span.begin();
    auto rv = iter++;
    EXPECT_EQ(iter, span.end());
    EXPECT_EQ(rv, span.begin());
  }
}

TEST_F(TestSpanIter, end_dec)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto span = yy_quad::make_span(vec);
  EXPECT_NE(span.begin(), span.end());

  {
    auto iter = span.end();
    --iter;
    EXPECT_EQ(iter, span.begin());
  }

  {
    auto iter = span.end();
    auto rv = iter--;
    EXPECT_EQ(iter, span.begin());
    EXPECT_EQ(rv, span.end());
  }
}

TEST_F(TestSpanIter, offset_plus)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto span = yy_quad::make_span(vec);
  EXPECT_NE(span.begin(), span.end());

  {
    auto iter = span.begin();
    iter += 1;
    EXPECT_EQ(iter, span.end());
  }

  {
    auto iter = span.begin();
    iter += ssize_type{1};
    EXPECT_EQ(iter, span.end());
  }

  {
    auto iter = span.begin();
    iter += size_type{1};
    EXPECT_EQ(iter, span.end());
  }

  {
    auto iter_1 = span.begin();
    auto iter_2 = iter_1 + 1;
    EXPECT_EQ(iter_2, span.end());
  }

  {
    auto iter_1 = span.begin();
    auto iter_2 = iter_1 + ssize_type{1};
    EXPECT_EQ(iter_2, span.end());
  }

  {
    auto iter_1 = span.begin();
    auto iter_2 = iter_1 + size_type{1};
    EXPECT_EQ(iter_2, span.end());
  }
}

TEST_F(TestSpanIter, offset_minus)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto span = yy_quad::make_span(vec);
  EXPECT_NE(span.begin(), span.end());

  {
    auto iter = span.end();
    iter -= 1;
    EXPECT_EQ(iter, span.begin());
  }

  {
    auto iter = span.end();
    iter -= ssize_type{1};
    EXPECT_EQ(iter, span.begin());
  }

  {
    auto iter = span.end();
    iter -= size_type{1};
    EXPECT_EQ(iter, span.begin());
  }

  {
    auto iter_1 = span.end();
    auto iter_2 = iter_1 - 1;
    EXPECT_EQ(iter_2, span.begin());
  }

  {
    auto iter_1 = span.end();
    auto iter_2 = iter_1 - ssize_type{1};
    EXPECT_EQ(iter_2, span.begin());
  }

  {
    auto iter_1 = span.end();
    auto iter_2 = iter_1 - size_type{1};
    EXPECT_EQ(iter_2, span.begin());
  }
}

TEST_F(TestSpanIter, distance)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto span = yy_quad::make_span(vec);
  EXPECT_NE(span.begin(), span.end());

  auto iter = span.begin() + 1;

  EXPECT_EQ(1, (iter - span.begin()));
}

TEST_F(TestSpanIter, const_begin_end_default)
{
  vector vec{};

  ASSERT_EQ(nullptr, vec.data());
  EXPECT_EQ(0, vec.size());
  EXPECT_TRUE(vec.empty());

  auto const_span = yy_quad::make_const_span(vec);
  EXPECT_EQ(const_span.begin(), const_span.end());
}

TEST_F(TestSpanIter, const_begin_inc)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto const_span = yy_quad::make_const_span(vec);
  EXPECT_NE(const_span.begin(), const_span.end());

  {
    auto iter = const_span.begin();
    ++iter;
    EXPECT_EQ(iter, const_span.end());
  }

  {
    auto iter = const_span.begin();
    auto rv = iter++;
    EXPECT_EQ(iter, const_span.end());
    EXPECT_EQ(rv, const_span.begin());
  }
}

TEST_F(TestSpanIter, const_end_dec)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto const_span = yy_quad::make_const_span(vec);
  EXPECT_NE(const_span.begin(), const_span.end());

  {
    auto iter = const_span.end();
    --iter;
    EXPECT_EQ(iter, const_span.begin());
  }

  {
    auto iter = const_span.end();
    auto rv = iter--;
    EXPECT_EQ(iter, const_span.begin());
    EXPECT_EQ(rv, const_span.end());
  }
}

TEST_F(TestSpanIter, const_offset_plus)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto const_span = yy_quad::make_const_span(vec);
  EXPECT_NE(const_span.begin(), const_span.end());

  {
    auto iter = const_span.begin();
    iter += 1;
    EXPECT_EQ(iter, const_span.end());
  }

  {
    auto iter = const_span.begin();
    iter += ssize_type{1};
    EXPECT_EQ(iter, const_span.end());
  }

  {
    auto iter = const_span.begin();
    iter += size_type{1};
    EXPECT_EQ(iter, const_span.end());
  }

  {
    auto iter_1 = const_span.begin();
    auto iter_2 = iter_1 + 1;
    EXPECT_EQ(iter_2, const_span.end());
  }

  {
    auto iter_1 = const_span.begin();
    auto iter_2 = iter_1 + ssize_type{1};
    EXPECT_EQ(iter_2, const_span.end());
  }

  {
    auto iter_1 = const_span.begin();
    auto iter_2 = iter_1 + size_type{1};
    EXPECT_EQ(iter_2, const_span.end());
  }
}

TEST_F(TestSpanIter, const_offset_minus)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto const_span = yy_quad::make_const_span(vec);
  EXPECT_NE(const_span.begin(), const_span.end());

  {
    auto iter = const_span.end();
    iter -= 1;
    EXPECT_EQ(iter, const_span.begin());
  }

  {
    auto iter = const_span.end();
    iter -= ssize_type{1};
    EXPECT_EQ(iter, const_span.begin());
  }

  {
    auto iter = const_span.end();
    iter -= size_type{1};
    EXPECT_EQ(iter, const_span.begin());
  }

  {
    auto iter_1 = const_span.end();
    auto iter_2 = iter_1 - 1;
    EXPECT_EQ(iter_2, const_span.begin());
  }

  {
    auto iter_1 = const_span.end();
    auto iter_2 = iter_1 - ssize_type{1};
    EXPECT_EQ(iter_2, const_span.begin());
  }

  {
    auto iter_1 = const_span.end();
    auto iter_2 = iter_1 - size_type{1};
    EXPECT_EQ(iter_2, const_span.begin());
  }
}

TEST_F(TestSpanIter, const_distance)
{
  vector vec{};

  vec.emplace_back(1);
  ASSERT_NE(nullptr, vec.data());
  EXPECT_EQ(1, vec.size());
  EXPECT_FALSE(vec.empty());

  auto const_span = yy_quad::make_const_span(vec);
  EXPECT_NE(const_span.begin(), const_span.end());

  auto iter = const_span.begin() + 1;

  EXPECT_EQ(1, (iter - const_span.begin()));
}

} // namespace yafiyogi::yy_cpp::tests
