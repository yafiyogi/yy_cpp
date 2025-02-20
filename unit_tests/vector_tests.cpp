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

#include <string>
#include "fmt/format.h"
#include "gtest/gtest.h"

#include "yy_test/yy_test_count.h"

#include "yy_vector.h"

namespace yafiyogi::yy_cpp::tests {

class TestVector:
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

    size_type calc_capacity(size_type capacity, int size)
    {
      auto u_size{static_cast<size_type>(size)};

      if(capacity < u_size)
      {
        capacity = u_size * 2;
      }

      return capacity;
    }
};

TEST_F(TestVector, DefaltConstructor)
{
  vector vec{};

  ASSERT_EQ(nullptr, vec.data());
  EXPECT_EQ(0, vec.size());
  EXPECT_EQ(0, vec.capacity());
  EXPECT_TRUE(vec.empty());
}

TEST_F(TestVector, TestEmplaceBackNoCapacity)
{
  vector vec{};
  size_type capacity = 0;

  for(int ii = 0; ii < 65; ++ii)
  {
    vec.emplace_back(ii + 7);

    capacity = calc_capacity(capacity, ii + 1);

    ASSERT_EQ(ii + 1, vec.size());
    ASSERT_EQ(capacity, vec.capacity());

    for(size_t jj = 0; jj < vec.size(); ++jj)
    {
      ASSERT_EQ(jj + 7, vec[jj]);
    }
  }
}

TEST_F(TestVector, TestEmplaceBackPlentyCapacity)
{
  vector vec{};
  vec.reserve(100);

  for(int ii = 0; ii < 65; ++ii)
  {
    vec.emplace_back(ii + 7);
    ASSERT_EQ(ii + 1, vec.size());
    ASSERT_EQ(100, vec.capacity());

    for(size_t jj = 0; jj < vec.size(); ++jj)
    {
      ASSERT_EQ(jj + 7, vec[jj]);
    }
  }
}

TEST_F(TestVector, TestEmplaceNoCapacity)
{
  vector vec{};

  vec.emplace_back(1);
  vec.emplace_back(3);

  auto rv = vec.emplace(vec.begin() + 1, 2);
  ASSERT_TRUE(rv.inserted);
  ASSERT_EQ(2, *rv.iter);
  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(6, vec.capacity());

  EXPECT_EQ(1, vec[0]);
  EXPECT_EQ(2, vec[1]);
  EXPECT_EQ(3, vec[2]);
}

TEST_F(TestVector, TestEmplacePentyCapacity)
{
  vector vec{};
  vec.reserve(100);

  vec.emplace_back(1);
  vec.emplace_back(3);

  auto rv = vec.emplace(vec.begin() + 1, 2);
  ASSERT_TRUE(rv.inserted);
  ASSERT_EQ(2, *rv.iter);
  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(100, vec.capacity());

  EXPECT_EQ(1, vec[0]);
  EXPECT_EQ(2, vec[1]);
  EXPECT_EQ(3, vec[2]);
}

TEST_F(TestVector, TestEmplaceAtBeginningNoCapacity)
{
  vector vec{};

  ASSERT_TRUE(vec.emplace(vec.begin(), 3).inserted);
  ASSERT_EQ(3, vec[0]);
  ASSERT_TRUE(vec.emplace(vec.begin(), 2).inserted);
  ASSERT_EQ(2, vec[0]);
  ASSERT_TRUE(vec.emplace(vec.begin(), 1).inserted);
  ASSERT_EQ(1, vec[0]);

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(6, vec.capacity());
}

TEST_F(TestVector, TestEmplaceAtBeginningPlentyCapacity)
{
  vector vec{};
  vec.reserve(100);

  ASSERT_TRUE(vec.emplace(vec.begin(), 3).inserted);
  ASSERT_EQ(3, vec[0]);
  ASSERT_TRUE(vec.emplace(vec.begin(), 2).inserted);
  ASSERT_EQ(2, vec[0]);
  ASSERT_TRUE(vec.emplace(vec.begin(), 1).inserted);
  ASSERT_EQ(1, vec[0]);

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(100, vec.capacity());
}

TEST_F(TestVector, TestEmplaceAtEndNoCapacity)
{
  vector vec{};

  ASSERT_TRUE(vec.emplace(vec.end(), 3).inserted);
  ASSERT_EQ(3, vec.back());
  ASSERT_TRUE(vec.emplace(vec.end(), 2).inserted);
  ASSERT_EQ(2, vec.back());
  ASSERT_TRUE(vec.emplace(vec.end(), 1).inserted);
  ASSERT_EQ(1, vec.back());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(6, vec.capacity());
}

TEST_F(TestVector, TestEmplaceAtEndPlentyCapacity)
{
  vector vec{};

  vec.reserve(100);
  ASSERT_TRUE(vec.emplace(vec.end(), 3).inserted);
  ASSERT_EQ(3, vec.back());
  ASSERT_TRUE(vec.emplace(vec.end(), 2).inserted);
  ASSERT_EQ(2, vec.back());
  ASSERT_TRUE(vec.emplace(vec.end(), 1).inserted);
  ASSERT_EQ(1, vec.back());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(100, vec.capacity());
}

TEST_F(TestVector, TestCopyConstruct)
{
  vector vec_1{};

  vec_1.reserve(100);
  vec_1.emplace_back(1);
  vec_1.emplace_back(2);
  vec_1.emplace_back(3);

  ASSERT_EQ(3, vec_1.size());
  ASSERT_EQ(100, vec_1.capacity());

  vector vec_2{vec_1};

  ASSERT_EQ(3, vec_2.size());
  ASSERT_EQ(3, vec_2.capacity());
  ASSERT_NE(vec_1.begin(), vec_2.begin());
  ASSERT_NE(vec_1.end(), vec_2.end());

  for(std::size_t idx = 0; idx < vec_2.size(); ++idx)
  {
    ASSERT_EQ(vec_1[idx], vec_2[idx]);
  }
}

TEST_F(TestVector, TestEraseFirstClear)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto begin = vec.begin();

  EXPECT_TRUE(vec.erase(vec.begin(), yy_quad::ClearAction::Clear));

  ASSERT_EQ(7, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(1, vec.offset());

  ASSERT_EQ(begin + 1, vec.begin());
}

TEST_F(TestVector, TestEraseFirstKeep)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto begin = vec.begin();

  EXPECT_TRUE(vec.erase(vec.begin(), yy_quad::ClearAction::Keep));

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(1, vec.offset());

  ASSERT_EQ(begin + 1, vec.begin());
}

TEST_F(TestVector, TestEraseLastClear)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto end = vec.end();

  EXPECT_TRUE(vec.erase(std::prev(vec.end()), yy_quad::ClearAction::Clear));

  ASSERT_EQ(7, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(end, vec.end() + 1);
}

TEST_F(TestVector, TestEraseLastKeep)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto end = vec.end();

  EXPECT_TRUE(vec.erase(std::prev(vec.end()), yy_quad::ClearAction::Keep));

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(end, vec.end() + 1);
}

TEST_F(TestVector, TestEraseEnd)
{
  vector vec{};
  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(6, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto end = vec.end();

  EXPECT_FALSE(vec.erase(vec.end()));

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(6, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(end, vec.end());
}


TEST_F(TestVector, TestEraseMiddleClear)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto end = vec.end();

  EXPECT_TRUE(vec.erase(vec.begin() + 1, yy_quad::ClearAction::Clear));

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(end, vec.end() + 1);
}

TEST_F(TestVector, TestEraseMiddleKeep)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto end = vec.end();

  EXPECT_TRUE(vec.erase(vec.begin() + 1, yy_quad::ClearAction::Keep));

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(end, vec.end() + 1);
}

TEST_F(TestVector, TestEraseRangeAllClear)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  vec.erase(vec.begin(), vec.end(), yy_quad::ClearAction::Clear);

  ASSERT_EQ(0, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());
}

TEST_F(TestVector, TestEraseRangeAllKeep)
{

  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  vec.erase(vec.begin(), vec.end(), yy_quad::ClearAction::Keep);

  ASSERT_EQ(0, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());
}

TEST_F(TestVector, TestEraseRangeSomeFromBeginClear)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  vec.erase(vec.begin(), std::prev(vec.end()), yy_quad::ClearAction::Clear);

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(3, vec.offset());
  ASSERT_EQ(7, vec[0].value());
}

TEST_F(TestVector, TestEraseRangeSomeFromBeginKeep)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  vec.erase(vec.begin(), std::prev(vec.end()), yy_quad::ClearAction::Keep);

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(3, vec.offset());
  ASSERT_EQ(7, vec[0].value());
}

TEST_F(TestVector, TestEraseRangeSomeFromEndClear)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  vec.erase(vec.begin() + 1, vec.end(), yy_quad::ClearAction::Clear);

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());
  ASSERT_EQ(4, vec[0].value());
}

TEST_F(TestVector, TestEraseRangeSomeFromEndKeep)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  vec.erase(vec.begin() + 1, vec.end(), yy_quad::ClearAction::Keep);

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());
  ASSERT_EQ(4, vec[0].value());
}

TEST_F(TestVector, TestEraseRangeMiddleClear)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  vec.erase(vec.begin() + 1, vec.end() - 1, yy_quad::ClearAction::Clear);

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());
  ASSERT_EQ(4, vec[0].value());
  ASSERT_EQ(7, vec[1].value());
}

TEST_F(TestVector, TestEraseRangeMiddleKeep)
{
  yy_quad::vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  vec.erase(vec.begin() + 1, vec.end() - 1, yy_quad::ClearAction::Keep);

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(0, vec.offset());
  ASSERT_EQ(4, vec[0].value());
  ASSERT_EQ(7, vec[1].value());
}

TEST_F(TestVector, TestResetOffsetFromFront)
{
  vector vec{};
  vec.reserve(3);

  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto begin = vec.begin();

  EXPECT_TRUE(vec.erase(vec.begin()));
  EXPECT_TRUE(vec.erase(vec.begin()));

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(2, vec.offset());

  ASSERT_EQ(begin + 2, vec.begin());

  EXPECT_TRUE(vec.erase(vec.begin()));

  ASSERT_EQ(0, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(begin, vec.begin());
  ASSERT_EQ(vec.begin(), vec.end());
}

TEST_F(TestVector, TestResetOffsetFromLast)
{
  vector vec{};
  vec.reserve(3);

  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  auto end = vec.end();

  EXPECT_TRUE(vec.erase(std::prev(vec.end())));
  EXPECT_TRUE(vec.erase(std::prev(vec.end())));

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(end, vec.end() + 2);

  EXPECT_TRUE(vec.erase(std::prev(vec.end())));

  ASSERT_EQ(0, vec.size());
  ASSERT_EQ(3, vec.capacity());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(vec.begin(), vec.end());
}

TEST_F(TestVector, TestClearWithActionClear)
{
  yy_quad::vector<yy_test::TestCount> vec;

  vec.reserve(3);
  EXPECT_EQ(3, yy_test::TestCount::construct_count());
  EXPECT_EQ(0, yy_test::TestCount::destruct_count());

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_FALSE(vec.empty());
  ASSERT_EQ(3, vec.size());

  vec.clear(yy_quad::ClearAction::Clear);

  EXPECT_EQ(9, yy_test::TestCount::construct_count());
  EXPECT_EQ(6, yy_test::TestCount::destruct_count());

  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(0, vec.size());
}

TEST_F(TestVector, TestClearWithActionKeep)
{
  yy_quad::vector<yy_test::TestCount> vec;

  vec.reserve(3);
  EXPECT_EQ(3, yy_test::TestCount::construct_count());
  EXPECT_EQ(0, yy_test::TestCount::destruct_count());

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_FALSE(vec.empty());
  ASSERT_EQ(3, vec.size());

  vec.clear(yy_quad::ClearAction::Keep);

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(0, vec.size());
}

TEST_F(TestVector, TestCopyEmptyActionClear)
{
  yy_quad::vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::vector<yy_test::TestCount> vec_2;

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Clear);

  EXPECT_EQ(9, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());
}

TEST_F(TestVector, TestCopyEmptyActionKeep)
{
  yy_quad::vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::vector<yy_test::TestCount> vec_2;

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Keep);

  EXPECT_EQ(9, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());
}

TEST_F(TestVector, TestCopySameSizeActionClear)
{
  yy_quad::vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::vector<yy_test::TestCount> vec_2;
  vec_2.reserve(3);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(12, yy_test::TestCount::construct_count());
  EXPECT_EQ(6, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Clear);

  EXPECT_EQ(12, yy_test::TestCount::construct_count());
  EXPECT_EQ(6, yy_test::TestCount::destruct_count());
}

TEST_F(TestVector, TestCopySameSizeActionKeep)
{
  yy_quad::vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::vector<yy_test::TestCount> vec_2;
  vec_2.reserve(3);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(12, yy_test::TestCount::construct_count());
  EXPECT_EQ(6, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Keep);

  EXPECT_EQ(12, yy_test::TestCount::construct_count());
  EXPECT_EQ(6, yy_test::TestCount::destruct_count());
}

TEST_F(TestVector, TestCopySmallerActionClear)
{
  yy_quad::vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::vector<yy_test::TestCount> vec_2;
  vec_2.reserve(2);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(10, yy_test::TestCount::construct_count());
  EXPECT_EQ(5, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Clear);

  EXPECT_EQ(13, yy_test::TestCount::construct_count());
  EXPECT_EQ(7, yy_test::TestCount::destruct_count());
}

TEST_F(TestVector, TestCopySmallerActionKeep)
{
  yy_quad::vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::vector<yy_test::TestCount> vec_2;
  vec_2.reserve(2);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(10, yy_test::TestCount::construct_count());
  EXPECT_EQ(5, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Keep);

  EXPECT_EQ(13, yy_test::TestCount::construct_count());
  EXPECT_EQ(7, yy_test::TestCount::destruct_count());
}

TEST_F(TestVector, TestCopyLargerActionClear)
{
  yy_quad::vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::vector<yy_test::TestCount> vec_2;
  vec_2.reserve(4);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(14, yy_test::TestCount::construct_count());
  EXPECT_EQ(7, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Clear);

  EXPECT_EQ(15, yy_test::TestCount::construct_count());
  EXPECT_EQ(8, yy_test::TestCount::destruct_count());
}

TEST_F(TestVector, TestCopyLargerActionKeep)
{
  yy_quad::vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::vector<yy_test::TestCount> vec_2;
  vec_2.reserve(4);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(14, yy_test::TestCount::construct_count());
  EXPECT_EQ(7, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Keep);

  EXPECT_EQ(14, yy_test::TestCount::construct_count());
  EXPECT_EQ(7, yy_test::TestCount::destruct_count());
}

TEST_F(TestVector, TestFront)
{
  vector vec;
  vec.reserve(4);
  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);
  vec.emplace_back(999);

  ASSERT_EQ(4, vec.size());

  ASSERT_EQ(668, vec.front());

  vec.erase(vec.begin());

  ASSERT_EQ(777, vec.front());
}

TEST_F(TestVector, TestBack)
{
  vector vec;
  vec.reserve(4);
  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);
  vec.emplace_back(999);

  ASSERT_EQ(4, vec.size());

  ASSERT_EQ(999, vec.back());

  vec.erase(std::prev(vec.end()));
  ASSERT_EQ(888, vec.back());
}

TEST_F(TestVector, TestFullPopFrontAddBack)
{
  vector vec;
  vec.reserve(4);

  auto raw_data = vec.data();

  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);
  vec.emplace_back(999);

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(0, vec.offset());

  vec.pop_front();

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(1, vec.offset());

  ASSERT_EQ(777, vec[0]);
  ASSERT_EQ(888, vec[1]);
  ASSERT_EQ(999, vec[2]);

  vec.emplace_back(555);

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(777, vec[0]);
  ASSERT_EQ(888, vec[1]);
  ASSERT_EQ(999, vec[2]);
  ASSERT_EQ(555, vec[3]);

  ASSERT_EQ(raw_data, vec.data());
}

TEST_F(TestVector, TestFullPopFrontAddFront)
{
  vector vec;
  vec.reserve(4);

  auto raw_data = vec.data();

  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);
  vec.emplace_back(999);

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(0, vec.offset());

  vec.pop_front();

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(1, vec.offset());

  ASSERT_EQ(777, vec[0]);
  ASSERT_EQ(888, vec[1]);
  ASSERT_EQ(999, vec[2]);

  vec.emplace(vec.begin(), 555);

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(555, vec[0]);
  ASSERT_EQ(777, vec[1]);
  ASSERT_EQ(888, vec[2]);
  ASSERT_EQ(999, vec[3]);

  ASSERT_EQ(raw_data, vec.data());
}

TEST_F(TestVector, TestFullPopFrontAddMid)
{
  vector vec;
  vec.reserve(4);

  auto raw_data = vec.data();

  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);
  vec.emplace_back(999);

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(0, vec.offset());

  vec.pop_front();

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(1, vec.offset());

  ASSERT_EQ(777, vec[0]);
  ASSERT_EQ(888, vec[1]);
  ASSERT_EQ(999, vec[2]);

  vec.emplace(vec.begin() + 2, 555);

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(0, vec.offset());

  ASSERT_EQ(777, vec[0]);
  ASSERT_EQ(888, vec[1]);
  ASSERT_EQ(555, vec[2]);
  ASSERT_EQ(999, vec[3]);

  ASSERT_EQ(raw_data, vec.data());
}

} // namespace yafiyogi::yy_cpp::tests
