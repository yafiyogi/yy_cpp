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

#include "gtest/gtest.h"

#include "fmt/format.h"

#include "yy_find_iter_util.hpp"

namespace yafiyogi::yy_cpp::tests {

class TestFindIterUtil:
      public testing::Test
{
  public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    struct val
    {
        int k{};
        int v{};

        static int compare(const val & v,
                           const int target)
        {
          return v.k - target;
        }
    };
};

TEST_F(TestFindIterUtil, lower_bound_simple)
{
  std::vector<int> vec = {2, 3, 4, 5, 7, 8};

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 1);

    ASSERT_EQ(iter, vec.begin());
    EXPECT_NE(1, *iter);
  }

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 2);

    ASSERT_EQ(iter, vec.begin());
    EXPECT_EQ(2, *iter);
  }

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 4);

    ASSERT_EQ(iter, vec.begin() + 2);
    EXPECT_EQ(4, *iter);
  }

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 8);

    ASSERT_EQ(iter, vec.begin() + 5);
    EXPECT_EQ(8, *iter);
  }

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 9);

    ASSERT_EQ(iter, vec.end());
    EXPECT_NE(9, *iter);
  }
}

TEST_F(TestFindIterUtil, lower_bound_compare)
{
  auto lower_bound_compare = [](const val & item,
                                int target) -> bool {
    return val::compare(item, target) < 0;
  };

  std::vector<val> vec = {val{2, 1}, val{3, 1}, val{4, 1}, val{5, 1}, val{7, 1}, val{8, 1}};

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 1, lower_bound_compare);

    ASSERT_EQ(iter, vec.begin());
    EXPECT_NE(1, iter->k);
  }

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 2, lower_bound_compare);

    ASSERT_EQ(iter, vec.begin());
    EXPECT_EQ(2, iter->k);
  }

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 4, lower_bound_compare);

    ASSERT_EQ(iter, vec.begin() + 2);
    EXPECT_EQ(4, iter->k);
  }

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 8, lower_bound_compare);

    ASSERT_EQ(iter, vec.begin() + 5);
    EXPECT_EQ(8, iter->k);
  }

  {
    auto [iter, is_end] = yy_data::lower_bound_iter(vec.begin(), vec.end(), 9, lower_bound_compare);

    ASSERT_EQ(iter, vec.end());
    EXPECT_NE(9, iter->k);
  }
}

TEST_F(TestFindIterUtil, upper_bound_simple)
{
  std::vector<int> vec = {2, 3, 4, 5, 7, 8};

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 1);

    ASSERT_EQ(iter, vec.begin());
    EXPECT_LT(1, *iter);
  }

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 2);

    ASSERT_EQ(iter, vec.begin() + 1);
    EXPECT_LT(2, *iter);
  }

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 4);

    ASSERT_EQ(iter, vec.begin() + 3);
    EXPECT_LT(4, *iter);
  }

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 8);

    ASSERT_EQ(iter, vec.end());
  }

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 9);

    ASSERT_EQ(iter, vec.end());
  }
}

TEST_F(TestFindIterUtil, upper_bound_compare)
{
  auto upper_bound_compare = [](int target,
                                const val & item) -> bool {
    return val::compare(item, target) > 0;
  };

  std::vector<val> vec = {val{2, 1}, val{3, 1}, val{4, 1}, val{5, 1}, val{7, 1}, val{8, 1}};

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 1, upper_bound_compare);

    ASSERT_EQ(iter, vec.begin());
    EXPECT_LT(1, iter->k);
  }

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 2, upper_bound_compare);

    ASSERT_EQ(iter, vec.begin() + 1);
    EXPECT_LT(2, iter->k);
  }

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 4, upper_bound_compare);

    ASSERT_EQ(iter, vec.begin() + 3);
    EXPECT_LT(4, iter->k);
  }

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 8, upper_bound_compare);

    ASSERT_EQ(iter, vec.end());
  }

  {
    auto [iter, is_end] = yy_data::upper_bound_iter(vec.begin(), vec.end(), 9, upper_bound_compare);

    ASSERT_EQ(iter, vec.end());
  }
}

TEST_F(TestFindIterUtil, range_simple)
{
  std::vector<int> vec = {2, 3, 4, 5, 7, 8};

  {
    auto r = yy_data::range_iter(vec, 1);

    EXPECT_EQ(r.begin(), r.end());
  }

  {
    auto r = yy_data::range_iter(vec, 2);

    EXPECT_NE(r.begin(), r.end());
    EXPECT_EQ(r.begin() + 1, r.end());
    EXPECT_EQ(2, *r.begin());
  }

  {
    auto r = yy_data::range_iter(vec, 4);

    EXPECT_NE(r.begin(), r.end());
    EXPECT_EQ(r.begin() + 1, r.end());
    EXPECT_EQ(4, *r.begin());
  }

  {
    auto r = yy_data::range_iter(vec, 8);

    EXPECT_NE(r.begin(), r.end());
    EXPECT_EQ(r.begin() + 1, r.end());
    EXPECT_EQ(8, *r.begin());
  }

  {
    auto r = yy_data::range_iter(vec, 9);

    EXPECT_EQ(r.begin(), r.end());
    EXPECT_EQ(r.begin(), vec.end());
  }
}

TEST_F(TestFindIterUtil, range_compare)
{
  std::vector<val> vec = {val{2, 1}, val{3, 1}, val{4, 1}, val{5, 1}, val{7, 1}, val{8, 1}};

  {
    auto r = yy_data::range_iter(vec, 1, val::compare);

    EXPECT_EQ(r.begin(), r.end());
  }

  {
    auto r = yy_data::range_iter(vec, 2, val::compare);

    EXPECT_NE(r.begin(), r.end());
    EXPECT_EQ(r.begin() + 1, r.end());
    EXPECT_EQ(2, r.begin()->k);
  }

  {
    auto r = yy_data::range_iter(vec, 4, val::compare);

    EXPECT_NE(r.begin(), r.end());
    EXPECT_EQ(r.begin() + 1, r.end());
    EXPECT_EQ(4, r.begin()->k);
  }

  {
    auto r = yy_data::range_iter(vec, 8, val::compare);

    EXPECT_NE(r.begin(), r.end());
    EXPECT_EQ(r.begin() + 1, r.end());
    EXPECT_EQ(8, r.begin()->k);
  }

  {
    auto r = yy_data::range_iter(vec, 9, val::compare);

    EXPECT_EQ(r.begin(), r.end());
    EXPECT_EQ(r.begin(), vec.end());
  }
}

TEST_F(TestFindIterUtil, find_iter_simple)
{
  std::vector<int> vec = {2, 3, 4, 5, 7, 8};

  {
    auto [iter, found] = yy_data::find_iter(vec, 1);

    EXPECT_FALSE(found);
  }

  {
    auto [iter, found] = yy_data::find_iter(vec, 2);

    EXPECT_TRUE(found);
    EXPECT_EQ(2, *iter);
  }

  {
    auto [iter, found] = yy_data::find_iter(vec, 4);

    EXPECT_TRUE(found);
    EXPECT_EQ(4, *iter);
  }

  {
    auto [iter, found] = yy_data::find_iter(vec, 8);

    EXPECT_TRUE(found);
    EXPECT_EQ(8, *iter);
  }

  {
    auto [iter, found] = yy_data::find_iter(vec, 9);

    EXPECT_FALSE(found);
  }
}

TEST_F(TestFindIterUtil, find_iter_compare)
{
  std::vector<val> vec = {val{2, 1}, val{3, 1}, val{4, 1}, val{5, 1}, val{7, 1}, val{8, 1}};

  {
    auto [iter, found] = yy_data::find_iter(vec, 1, val::compare);

    EXPECT_FALSE(found);
  }

  {
    auto [iter, found] = yy_data::find_iter(vec, 2, val::compare);

    EXPECT_TRUE(found);
    EXPECT_EQ(2, iter->k);
  }

  {
    auto [iter, found] = yy_data::find_iter(vec, 4, val::compare);

    EXPECT_TRUE(found);
    EXPECT_EQ(4, iter->k);
  }

  {
    auto [iter, found] = yy_data::find_iter(vec, 8, val::compare);

    EXPECT_TRUE(found);
    EXPECT_EQ(8, iter->k);
  }

  {
    auto [iter, found] = yy_data::find_iter(vec, 9, val::compare);

    EXPECT_FALSE(found);
  }
}

} // namespace yafiyogi::yy_cpp::tests
