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
#include <type_traits>

#include "gtest/gtest.h"

#include "yy_ref_traits.h"
#include "yy_span.h"

namespace yafiyogi::yy_cpp::tests {

class TestRefTraits:
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

TEST_F(TestRefTraits, TestNotTrivial)
{
  using test_type = std::string;
  using ref_traits = yy_traits::ref_traits<test_type>;

  EXPECT_TRUE(std::is_lvalue_reference_v<ref_traits::l_value_ref>);
  EXPECT_TRUE(std::is_rvalue_reference_v<ref_traits::r_value_ref>);
}

TEST_F(TestRefTraits, TestTrivial)
{
  using test_type = double;
  using ref_traits = yy_traits::ref_traits<test_type>;

  EXPECT_FALSE(std::is_lvalue_reference_v<ref_traits::l_value_ref>);
  EXPECT_FALSE(std::is_rvalue_reference_v<ref_traits::r_value_ref>);
}

TEST_F(TestRefTraits, TestUniquePtr)
{
  using test_type = std::unique_ptr<int>;
  using ref_traits = yy_traits::ref_traits<test_type>;

  EXPECT_TRUE(std::is_lvalue_reference_v<ref_traits::l_value_ref>);
  EXPECT_TRUE(std::is_rvalue_reference_v<ref_traits::r_value_ref>);
}

TEST_F(TestRefTraits, TestSharedPtr)
{
  using test_type = std::shared_ptr<int>;
  using ref_traits = yy_traits::ref_traits<test_type>;

  EXPECT_TRUE(std::is_lvalue_reference_v<ref_traits::l_value_ref>);
  EXPECT_TRUE(std::is_rvalue_reference_v<ref_traits::r_value_ref>);
}

TEST_F(TestRefTraits, TestSpan)
{
  using test_type = yy_quad::span<char>;
  using ref_traits = yy_traits::ref_traits<test_type>;

  EXPECT_FALSE(std::is_lvalue_reference_v<ref_traits::l_value_ref>);
  EXPECT_FALSE(std::is_rvalue_reference_v<ref_traits::r_value_ref>);
}

TEST_F(TestRefTraits, TestNonTrivialStruct)
{
  struct test_struct
  {
      std::string v;
  };
  using test_type = test_struct;
  using ref_traits = yy_traits::ref_traits<test_type>;

  EXPECT_TRUE(std::is_lvalue_reference_v<ref_traits::l_value_ref>);
  EXPECT_TRUE(std::is_rvalue_reference_v<ref_traits::r_value_ref>);
}

TEST_F(TestRefTraits, TestTrivialStruct)
{
  struct test_struct
  {
      int int_v;
      char ch_v;
  };
  using test_type = test_struct;
  using ref_traits = yy_traits::ref_traits<test_type>;

  EXPECT_FALSE(std::is_lvalue_reference_v<ref_traits::l_value_ref>);
  EXPECT_FALSE(std::is_rvalue_reference_v<ref_traits::r_value_ref>);
}
} // namespace yafiyogi::yy_cpp::tests
