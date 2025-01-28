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
#include <string_view>

#include "gtest/gtest.h"

#include "yy_span.h"

namespace yafiyogi::yy_cpp::tests {

class TestSpan:
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

TEST_F(TestSpan, span_constructors)
{
  std::string str = "str";

  auto span = yafiyogi::yy_quad::make_span(str);

  EXPECT_EQ(str, span.data());

  EXPECT_EQ(str, span);
}

TEST_F(TestSpan, span_inc_begin)
{
  std::string str = "str";

  EXPECT_EQ(str.substr(1), yafiyogi::yy_quad::make_span(str).inc_begin());
  EXPECT_EQ(str.substr(2), yafiyogi::yy_quad::make_span(str).inc_begin(2));
  EXPECT_EQ(str.substr(3), yafiyogi::yy_quad::make_span(str).inc_begin(3));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_span(str).inc_begin(3));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_span(str).inc_begin(4));
}

TEST_F(TestSpan, span_dec_end)
{
  std::string str = "str";

  EXPECT_EQ(str.substr(0, str.size() - 1), yafiyogi::yy_quad::make_span(str).dec_end());
  EXPECT_EQ(str.substr(0, str.size() - 2), yafiyogi::yy_quad::make_span(str).dec_end(2));
  EXPECT_EQ(str.substr(0, str.size() - 3), yafiyogi::yy_quad::make_span(str).dec_end(3));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_span(str).dec_end(3));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_span(str).dec_end(4));
}

TEST_F(TestSpan, span_subspan)
{
  std::string str = "1234567890";

  EXPECT_EQ(str.substr(1), yafiyogi::yy_quad::make_span(str).subspan(1));
  EXPECT_EQ(str.substr(1, 3), yafiyogi::yy_quad::make_span(str).subspan(1, 3));
  EXPECT_EQ(str.substr(1, 11), yafiyogi::yy_quad::make_span(str).subspan(1, 11));
  EXPECT_EQ(str.substr(10, 2), yafiyogi::yy_quad::make_span(str).subspan(10, 2));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_span(str).subspan(10, 2));
}

TEST_F(TestSpan, span_find_pos)
{
  std::string str = "1234567890";

  EXPECT_EQ(4, yafiyogi::yy_quad::make_span(str).find_pos('5'));
  EXPECT_EQ(yafiyogi::npos, yafiyogi::yy_quad::make_span(str).find_pos('A'));
}

TEST_F(TestSpan, span_lt)
{
  auto str_a = std::string{"A"};
  auto str_b = std::string{"B"};
  auto str_b2 = std::string{"B"};
  auto str_c = std::string{"C"};

  auto span_a = yafiyogi::yy_quad::make_span(str_a);
  auto span_b = yafiyogi::yy_quad::make_span(str_b);
  auto span_b2 = yafiyogi::yy_quad::make_span(str_b2);
  auto span_c = yafiyogi::yy_quad::make_span(str_c);

  EXPECT_TRUE(span_a < span_b);
  EXPECT_FALSE(span_b < span_b2);
  EXPECT_FALSE(span_c < span_b);

  EXPECT_FALSE(span_b < str_a);
  EXPECT_FALSE(span_b < str_b2);
  EXPECT_TRUE(span_b < str_c);

  EXPECT_TRUE(str_a < span_b);
  EXPECT_FALSE(str_b2 < span_b);
  EXPECT_FALSE(str_c < span_b);
}

TEST_F(TestSpan, span_eq)
{
  auto str_a = std::string{"A"};
  auto str_b = std::string{"B"};
  auto str_b2 = std::string{"B"};
  auto str_c = std::string{"C"};

  auto span_a = yafiyogi::yy_quad::make_span(str_a);
  auto span_b = yafiyogi::yy_quad::make_span(str_b);
  auto span_b2 = yafiyogi::yy_quad::make_span(str_b2);
  auto span_c = yafiyogi::yy_quad::make_span(str_c);

  EXPECT_FALSE(span_a == span_b);
  EXPECT_TRUE(span_b == span_b2);
  EXPECT_FALSE(span_c == span_b);

  EXPECT_FALSE(span_b == str_a);
  EXPECT_TRUE(span_b == str_b2);
  EXPECT_FALSE(span_b == str_c);

  EXPECT_FALSE(str_a == span_b);
  EXPECT_TRUE(str_b2 == span_b);
  EXPECT_FALSE(str_c == span_b);
}

TEST_F(TestSpan, span_empty)
{
  std::string str = "";

  EXPECT_TRUE(yafiyogi::yy_quad::make_span(str).empty());
  str = "str";
  EXPECT_FALSE(yafiyogi::yy_quad::make_span(str).empty());
}

TEST_F(TestSpan, const_span_constructors)
{
  const char * str_1 = "str 1";
  std::string_view str_2 = "str 2";
  std::string str_3 = "str 2";

  auto span_1 = yafiyogi::yy_quad::make_const_span(str_1);
  auto span_2 = yafiyogi::yy_quad::make_const_span(str_2);
  auto span_3 = yafiyogi::yy_quad::make_const_span(str_3);

  EXPECT_EQ(str_1, span_1.data());
  EXPECT_EQ(str_2, span_2.data());
  EXPECT_EQ(str_3, span_3.data());

  EXPECT_EQ(std::string_view{str_1}, span_1.data());
  EXPECT_EQ(str_2, span_2);
  EXPECT_EQ(str_3, span_3);
}

TEST_F(TestSpan, const_span_inc_begin)
{
  std::string_view str = "str";

  EXPECT_EQ(str.substr(1), yafiyogi::yy_quad::make_const_span(str).inc_begin());
  EXPECT_EQ(str.substr(2), yafiyogi::yy_quad::make_const_span(str).inc_begin(2));
  EXPECT_EQ(str.substr(3), yafiyogi::yy_quad::make_const_span(str).inc_begin(3));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_const_span(str).inc_begin(3));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_const_span(str).inc_begin(4));
}

TEST_F(TestSpan, const_span_dec_end)
{
  std::string_view str = "str";

  EXPECT_EQ(str.substr(0, str.size() - 1), yafiyogi::yy_quad::make_const_span(str).dec_end());
  EXPECT_EQ(str.substr(0, str.size() - 2), yafiyogi::yy_quad::make_const_span(str).dec_end(2));
  EXPECT_EQ(str.substr(0, str.size() - 3), yafiyogi::yy_quad::make_const_span(str).dec_end(3));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_const_span(str).dec_end(3));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_const_span(str).dec_end(4));
}

TEST_F(TestSpan, const_span_subspan)
{
  std::string_view str = "1234567890";

  EXPECT_EQ(str.substr(1), yafiyogi::yy_quad::make_const_span(str).subspan(1));
  EXPECT_EQ(str.substr(1, 3), yafiyogi::yy_quad::make_const_span(str).subspan(1, 3));
  EXPECT_EQ(str.substr(1, 11), yafiyogi::yy_quad::make_const_span(str).subspan(1, 11));
  EXPECT_EQ(str.substr(10, 2), yafiyogi::yy_quad::make_const_span(str).subspan(10, 2));
  EXPECT_EQ(std::string_view{}, yafiyogi::yy_quad::make_const_span(str).subspan(10, 2));
}

TEST_F(TestSpan, const_span_find_pos)
{
  std::string str = "1234567890";

  EXPECT_EQ(4, yafiyogi::yy_quad::make_const_span(str).find_pos('5'));
  EXPECT_EQ(yafiyogi::npos, yafiyogi::yy_quad::make_const_span(str).find_pos('A'));
}

TEST_F(TestSpan, const_span_lt)
{
  auto str_a = std::string{"A"};
  auto str_b = std::string{"B"};
  auto str_b2 = std::string{"B"};
  auto str_c = std::string{"C"};

  auto const_span_a = yafiyogi::yy_quad::make_const_span(str_a);
  auto const_span_b = yafiyogi::yy_quad::make_const_span(str_b);
  auto const_span_b2 = yafiyogi::yy_quad::make_const_span(str_b2);
  auto const_span_c = yafiyogi::yy_quad::make_const_span(str_c);

  EXPECT_TRUE(const_span_a < const_span_b);
  EXPECT_FALSE(const_span_b < const_span_b2);
  EXPECT_FALSE(const_span_c < const_span_b);

  EXPECT_FALSE(const_span_b < str_a);
  EXPECT_FALSE(const_span_b < str_b2);
  EXPECT_TRUE(const_span_b < str_c);

  EXPECT_TRUE(str_a < const_span_b);
  EXPECT_FALSE(str_b2 < const_span_b);
  EXPECT_FALSE(str_c < const_span_b);

  auto str_a_2 = std::string{"A"};
  auto str_b_2 = std::string{"B"};
  auto str_c_2 = std::string{"C"};

  auto span_a = yafiyogi::yy_quad::make_span(str_a_2);
  auto span_b = yafiyogi::yy_quad::make_span(str_b_2);
  auto span_c = yafiyogi::yy_quad::make_span(str_c_2);

  EXPECT_FALSE(const_span_b < span_a);
  EXPECT_FALSE(const_span_b < span_b);
  EXPECT_TRUE(const_span_b < span_c);

  EXPECT_TRUE(span_a < const_span_b);
  EXPECT_FALSE(span_b < const_span_b);
  EXPECT_FALSE(span_c < const_span_b);
}

TEST_F(TestSpan, const_span_eq)
{
  auto str_a = std::string{"A"};
  auto str_b = std::string{"B"};
  auto str_b2 = std::string{"B"};
  auto str_c = std::string{"C"};

  auto const_span_a = yafiyogi::yy_quad::make_const_span(str_a);
  auto const_span_b = yafiyogi::yy_quad::make_const_span(str_b);
  auto const_span_b2 = yafiyogi::yy_quad::make_const_span(str_b2);
  auto const_span_c = yafiyogi::yy_quad::make_const_span(str_c);

  EXPECT_FALSE(const_span_a == const_span_b);
  EXPECT_TRUE(const_span_b == const_span_b2);
  EXPECT_FALSE(const_span_c == const_span_b);

  EXPECT_FALSE(const_span_b == str_a);
  EXPECT_TRUE(const_span_b == str_b2);
  EXPECT_FALSE(const_span_b == str_c);

  EXPECT_FALSE(str_a == const_span_b);
  EXPECT_TRUE(str_b2 == const_span_b);
  EXPECT_FALSE(str_c == const_span_b);

  auto str_a_2 = std::string{"A"};
  auto str_b_2 = std::string{"B"};
  auto str_c_2 = std::string{"C"};

  auto span_a = yafiyogi::yy_quad::make_span(str_a_2);
  auto span_b = yafiyogi::yy_quad::make_span(str_b_2);
  auto span_c = yafiyogi::yy_quad::make_span(str_c_2);

  EXPECT_FALSE(const_span_b == span_a);
  EXPECT_TRUE(const_span_b == span_b);
  EXPECT_FALSE(const_span_b == span_c);

  EXPECT_FALSE(span_a == const_span_b);
  EXPECT_TRUE(span_b == const_span_b);
  EXPECT_FALSE(span_c == const_span_b);
}

TEST_F(TestSpan, const_span_empty)
{
  std::string str = "";

  EXPECT_TRUE(yafiyogi::yy_quad::make_const_span(str).empty());
  str = "str";
  EXPECT_FALSE(yafiyogi::yy_quad::make_const_span(str).empty());
}

} // namespace yafiyogi::yy_cpp::tests
