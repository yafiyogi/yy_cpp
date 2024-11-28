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

#include <algorithm>

#include "gtest/gtest.h"

#include "yy_binary_search.h"


namespace yafiyogi::yy_cpp::tests {

class TestBinarySearch:
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

TEST_F(TestBinarySearch, yy_vs_std)
{
  std::vector<int> vec = {2, 3, 4, 5, 7, 8};
  auto begin = vec.data();
  auto end = vec.data() + vec.size();

  for(int val = 0; val < (vec.back() + 2); ++val)
  {
    auto std_item = std::lower_bound(begin, end, val);
    auto std_found = std::binary_search(begin, end, val);

    auto [yy_item, yy_found] = yy_data::binary_search(begin, end, val);

    EXPECT_EQ(std_item, yy_item);
    EXPECT_EQ(std_found, yy_found);

    if(std_found)
    {
      EXPECT_EQ(*std_item, *yy_item);
    }
  }
}

TEST_F(TestBinarySearch, yy_test_bs)
{
  std::vector<size_t> vec = {2, 3, 4, 5, 7, 8};
  std::vector<int> result_offs = {0, 0, 0, 1, 2, 3, 4, 4, 5, 6, 6};
  std::vector<bool> result_flag = {false, false, true, true, true, true, false, true, true, false, false};

  ASSERT_EQ(result_offs.size(), result_flag.size());

  for(size_t val = 0; val < result_flag.size(); ++val)
  {
    auto [yy_item, found] = yy_data::binary_search(vec.data(), vec.data() + vec.size(), val);
    EXPECT_EQ(vec.data() + result_offs[val], yy_item);
    EXPECT_EQ(found, result_flag[val]);
  }
}

} // namespace yafiyogi::yy_cpp::tests
