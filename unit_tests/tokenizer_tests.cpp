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

#include "yy_tokenizer.h"

namespace yafiyogi::yy_cpp::tests {

class TestTokenizer:
      public testing::Test
{
  public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
    static constexpr char delim{'/'};
};

TEST_F(TestTokenizer, TokenizeEmpty)
{
  std::string_view str{};
  yy_util::tokenizer<std::string_view::value_type> tokenizer{yy_quad::make_const_span(str),
                                                             delim};
  auto span = tokenizer.scan();
  EXPECT_TRUE(tokenizer.empty());
  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0);
  EXPECT_EQ(span, std::string_view{""});
}

TEST_F(TestTokenizer, TokenizeSimple)
{
  std::string_view str{"/abc"};
  yy_util::tokenizer<std::string_view::value_type> tokenizer{yy_quad::make_const_span(str),
                                                             delim};
  yy_quad::const_span<char> span{tokenizer.scan()};
  EXPECT_FALSE(tokenizer.empty());
  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0);
  EXPECT_EQ(span, std::string_view{""});

  span = tokenizer.scan();
  EXPECT_TRUE(tokenizer.empty());
  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.size(), 3);
  EXPECT_EQ(span, std::string_view{"abc"});

  span = tokenizer.scan();
  EXPECT_TRUE(tokenizer.empty());
  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0);
  EXPECT_EQ(span, std::string_view{""});
}

}
