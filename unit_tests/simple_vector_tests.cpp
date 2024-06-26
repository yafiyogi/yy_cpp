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

class TestSimpleVector:
      public testing::Test
{
  public:
    void SetUp() override
    {
      yy_test::TestCount::reset();
    }

    void TearDown() override
    {
    }
};


TEST_F(TestSimpleVector, DefaltConstructor)
{
  yy_quad::simple_vector<int> vec{};

  ASSERT_EQ(nullptr, vec.data());
  EXPECT_EQ(0, vec.size());
  EXPECT_EQ(0, vec.capacity());
  EXPECT_TRUE(vec.empty());
}

TEST_F(TestSimpleVector, TestEmplaceBackNoCapacity)
{
  yy_quad::simple_vector<int> vec{};

  for(int ii = 0; ii < 65; ++ii)
  {
    vec.emplace_back(ii + 7);
    ASSERT_EQ(ii + 1, vec.size());
    ASSERT_EQ(yy_bit_twiddling::round_up_pow2(vec.size()), vec.capacity());

    for(size_t jj = 0; jj < vec.size(); ++jj)
    {
      ASSERT_EQ(jj + 7, vec[jj]);
    }
  }
}

TEST_F(TestSimpleVector, TestEmplaceBackPlentyCapacity)
{
  yy_quad::simple_vector<int> vec{};
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

TEST_F(TestSimpleVector, TestEmplaceNoCapacity)
{
  yy_quad::simple_vector<int> vec{};

  vec.emplace_back(1);
  vec.emplace_back(3);

  auto rv = vec.emplace(vec.begin() + 1, 2);

  ASSERT_TRUE(rv.second);
  ASSERT_EQ(2, *rv.first);
  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(4, vec.capacity());

  EXPECT_EQ(1, vec[0]);
  EXPECT_EQ(2, vec[1]);
  EXPECT_EQ(3, vec[2]);
}

TEST_F(TestSimpleVector, TestEmplacePentyCapacity)
{
  yy_quad::simple_vector<int> vec{};
  vec.reserve(100);

  vec.emplace_back(1);
  vec.emplace_back(3);

  auto rv = vec.emplace(vec.begin() + 1, 2);
  ASSERT_TRUE(rv.second);
  ASSERT_EQ(2, *rv.first);
  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(100, vec.capacity());

  EXPECT_EQ(1, vec[0]);
  EXPECT_EQ(2, vec[1]);
  EXPECT_EQ(3, vec[2]);
}

TEST_F(TestSimpleVector, TestEmplaceAtBeginningNoCapacity)
{
  yy_quad::simple_vector<int> vec{};

  ASSERT_TRUE(vec.emplace(vec.begin(), 3).second);
  ASSERT_EQ(3, vec[0]);
  ASSERT_TRUE(vec.emplace(vec.begin(), 2).second);
  ASSERT_EQ(2, vec[0]);
  ASSERT_TRUE(vec.emplace(vec.begin(), 1).second);
  ASSERT_EQ(1, vec[0]);

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(4, vec.capacity());
}

TEST_F(TestSimpleVector, TestEmplaceAtBeginningPlentyCapacity)
{
  yy_quad::simple_vector<int> vec{};
  vec.reserve(100);

  ASSERT_TRUE(vec.emplace(vec.begin(), 3).second);
  ASSERT_EQ(3, vec[0]);
  ASSERT_TRUE(vec.emplace(vec.begin(), 2).second);
  ASSERT_EQ(2, vec[0]);
  ASSERT_TRUE(vec.emplace(vec.begin(), 1).second);
  ASSERT_EQ(1, vec[0]);

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(100, vec.capacity());
}

TEST_F(TestSimpleVector, TestEmplaceAtEndNoCapacity)
{
  yy_quad::simple_vector<int> vec{};

  ASSERT_TRUE(vec.emplace(vec.end(), 3).second);
  ASSERT_EQ(3, vec.back());
  ASSERT_TRUE(vec.emplace(vec.end(), 2).second);
  ASSERT_EQ(2, vec.back());
  ASSERT_TRUE(vec.emplace(vec.end(), 1).second);
  ASSERT_EQ(1, vec.back());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(4, vec.capacity());
}

TEST_F(TestSimpleVector, TestEmplaceAtEndPlentyCapacity)
{
  yy_quad::simple_vector<int> vec{};

  vec.reserve(100);
  ASSERT_TRUE(vec.emplace(vec.end(), 3).second);
  ASSERT_EQ(3, vec.back());
  ASSERT_TRUE(vec.emplace(vec.end(), 2).second);
  ASSERT_EQ(2, vec.back());
  ASSERT_TRUE(vec.emplace(vec.end(), 1).second);
  ASSERT_EQ(1, vec.back());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(100, vec.capacity());
}

TEST_F(TestSimpleVector, TestCopyConstruct)
{
  yy_quad::simple_vector<int> vec_1{};

  vec_1.reserve(100);
  vec_1.emplace_back(1);
  vec_1.emplace_back(2);
  vec_1.emplace_back(3);

  ASSERT_EQ(3, vec_1.size());
  ASSERT_EQ(100, vec_1.capacity());

  yy_quad::simple_vector<int> vec_2{vec_1};

  ASSERT_EQ(3, vec_2.size());
  ASSERT_EQ(3, vec_2.capacity());
  ASSERT_NE(vec_1.begin(), vec_2.begin());
  ASSERT_NE(vec_1.end(), vec_2.end());

  for(std::size_t idx = 0; idx < vec_2.size(); ++idx)
  {
    ASSERT_EQ(vec_1[idx], vec_2[idx]);
  }
}

TEST_F(TestSimpleVector, TestEraseFirst)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());

  auto begin = vec.begin();

  EXPECT_TRUE(vec.erase(vec.begin()));

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());

  ASSERT_EQ(begin, vec.begin());
}

TEST_F(TestSimpleVector, TestEraseLast)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());

  auto end = vec.end();

  EXPECT_TRUE(vec.erase(std::prev(vec.end())));

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());

  ASSERT_EQ(end, vec.end() + 1);
}

TEST_F(TestSimpleVector, TestEraseEnd)
{
  yy_quad::simple_vector<int> vec{};
  vec.emplace_back(668);
  vec.emplace_back(777);
  vec.emplace_back(888);

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(4, vec.capacity());

  auto end = vec.end();

  EXPECT_FALSE(vec.erase(vec.end()));

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(4, vec.capacity());

  ASSERT_EQ(end, vec.end());
}


TEST_F(TestSimpleVector, TestEraseMiddle)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(3);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(3, vec.size());
  ASSERT_EQ(3, vec.capacity());

  auto end = vec.end();

  EXPECT_TRUE(vec.erase(vec.begin() + 1));

  ASSERT_EQ(6, yy_test::TestCount::construct_count());
  ASSERT_EQ(3, yy_test::TestCount::destruct_count());

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(3, vec.capacity());

  ASSERT_EQ(end, vec.end() + 1);
}

TEST_F(TestSimpleVector, TestEraseRangeAllClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  vec.erase(vec.begin(), vec.end(), yy_quad::ClearAction::Clear);

  ASSERT_EQ(0, vec.size());
  ASSERT_EQ(4, vec.capacity());
}

TEST_F(TestSimpleVector, TestEraseRangeAllKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  vec.erase(vec.begin(), vec.end(), yy_quad::ClearAction::Keep);

  ASSERT_EQ(0, vec.size());
  ASSERT_EQ(4, vec.capacity());
}

TEST_F(TestSimpleVector, TestEraseRangeSomeFromBeginClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  vec.erase(vec.begin(), std::prev(vec.end()), yy_quad::ClearAction::Clear);

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(7, vec[0].value());
}

TEST_F(TestSimpleVector, TestEraseRangeSomeFromBeginKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  vec.erase(vec.begin(), std::prev(vec.end()), yy_quad::ClearAction::Keep);

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(7, vec[0].value());
}

TEST_F(TestSimpleVector, TestEraseRangeSomeFromEndClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  vec.erase(vec.begin() + 1, vec.end(), yy_quad::ClearAction::Clear);

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(4, vec[0].value());
}

TEST_F(TestSimpleVector, TestEraseRangeSomeFromEndKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  vec.erase(vec.begin() + 1, vec.end(), yy_quad::ClearAction::Keep);

  ASSERT_EQ(1, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(4, vec[0].value());
}

TEST_F(TestSimpleVector, TestEraseRangeMiddleClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  vec.erase(vec.begin() + 1, vec.end() - 1, yy_quad::ClearAction::Clear);

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(4, vec[0].value());
  ASSERT_EQ(7, vec[1].value());
}

TEST_F(TestSimpleVector, TestEraseRangeMiddleKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
  vec.reserve(4);

  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});
  vec.emplace_back(yy_test::TestCount{});

  ASSERT_EQ(8, yy_test::TestCount::construct_count());
  ASSERT_EQ(4, yy_test::TestCount::destruct_count());

  ASSERT_EQ(4, vec.size());
  ASSERT_EQ(4, vec.capacity());

  vec.erase(vec.begin() + 1, vec.end() - 1, yy_quad::ClearAction::Keep);

  ASSERT_EQ(2, vec.size());
  ASSERT_EQ(4, vec.capacity());
  ASSERT_EQ(4, vec[0].value());
  ASSERT_EQ(7, vec[1].value());
}

TEST_F(TestSimpleVector, TestClearWithActionClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec{};
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

  vec.clear();

  EXPECT_EQ(9, yy_test::TestCount::construct_count());
  EXPECT_EQ(6, yy_test::TestCount::destruct_count());

  ASSERT_TRUE(vec.empty());
  ASSERT_EQ(0, vec.size());
}

TEST_F(TestSimpleVector, TestClearWithActionKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec;
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

TEST_F(TestSimpleVector, TestCopyEmptyActionClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::simple_vector<yy_test::TestCount> vec_2;

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1);

  EXPECT_EQ(9, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());
}

TEST_F(TestSimpleVector, TestCopyEmptyActionKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::simple_vector<yy_test::TestCount> vec_2;

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Keep);

  EXPECT_EQ(9, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());
}

TEST_F(TestSimpleVector, TestCopySameSizeActionClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::simple_vector<yy_test::TestCount> vec_2;
  vec_2.reserve(3);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(12, yy_test::TestCount::construct_count());
  EXPECT_EQ(6, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1);

  EXPECT_EQ(12, yy_test::TestCount::construct_count());
  EXPECT_EQ(6, yy_test::TestCount::destruct_count());
}

TEST_F(TestSimpleVector, TestCopySameSizeActionKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::simple_vector<yy_test::TestCount> vec_2;
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

TEST_F(TestSimpleVector, TestCopySmallerActionClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::simple_vector<yy_test::TestCount> vec_2;
  vec_2.reserve(2);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(10, yy_test::TestCount::construct_count());
  EXPECT_EQ(5, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1);

  EXPECT_EQ(13, yy_test::TestCount::construct_count());
  EXPECT_EQ(7, yy_test::TestCount::destruct_count());
}

TEST_F(TestSimpleVector, TestCopySmallerActionKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::simple_vector<yy_test::TestCount> vec_2;
  vec_2.reserve(2);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(10, yy_test::TestCount::construct_count());
  EXPECT_EQ(5, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1, yy_quad::ClearAction::Keep);

  EXPECT_EQ(13, yy_test::TestCount::construct_count());
  EXPECT_EQ(7, yy_test::TestCount::destruct_count());
}

TEST_F(TestSimpleVector, TestCopyLargerActionClear)
{
  yy_quad::simple_vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::simple_vector<yy_test::TestCount> vec_2;
  vec_2.reserve(4);
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});
  vec_2.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(14, yy_test::TestCount::construct_count());
  EXPECT_EQ(7, yy_test::TestCount::destruct_count());

  vec_2.copy(vec_1);

  EXPECT_EQ(15, yy_test::TestCount::construct_count());
  EXPECT_EQ(8, yy_test::TestCount::destruct_count());
}

TEST_F(TestSimpleVector, TestCopyLargerActionKeep)
{
  yy_quad::simple_vector<yy_test::TestCount> vec_1;

  vec_1.reserve(3);
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});
  vec_1.emplace_back(yy_test::TestCount{});

  EXPECT_EQ(6, yy_test::TestCount::construct_count());
  EXPECT_EQ(3, yy_test::TestCount::destruct_count());

  yy_quad::simple_vector<yy_test::TestCount> vec_2;
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

TEST_F(TestSimpleVector, TestFront)
{
  yy_quad::simple_vector<int> vec;
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

TEST_F(TestSimpleVector, TestBack)
{
  yy_quad::simple_vector<int> vec;
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

} // namespace yafiyogi::yy_cpp::tests
