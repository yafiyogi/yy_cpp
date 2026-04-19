/*

  MIT License

  Copyright (c) 2025 Yafiyogi

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

#include "yy_utf8.hpp"

namespace yafiyogi::yy_data::tests {

constexpr auto utf8_1_start{yy_quad::make_const_span("\x00")};
constexpr auto utf8_1_end{yy_quad::make_const_span("\x7f")};

constexpr auto utf8_2_start{yy_quad::make_const_span("\xC2\x80")};
constexpr auto utf8_2_end{yy_quad::make_const_span("\xDf\xBf")};

constexpr auto utf8_3_start{yy_quad::make_const_span("\xE1\x80\x80")};
constexpr auto utf8_3_end{yy_quad::make_const_span("\xEf\xBf\xBf")};

constexpr auto utf8_4_start{yy_quad::make_const_span("\xF1\x80\x80\x80")};
constexpr auto utf8_4_end{yy_quad::make_const_span("\xF4\xBf\xBf\xBf")};

class TestUTF8: public testing::Test
{
  public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TestUTF8, test_utf8_len)
{
  auto ch_1{yy_quad::make_const_span("Z")};
  auto ch_2{yy_quad::make_const_span("Č")};
  auto ch_3{yy_quad::make_const_span("Ḋ")};
  auto ch_4{yy_quad::make_const_span("\xf8\xA0\xA1\xA2")};

  EXPECT_EQ(yy_util::utf8_len(ch_1[0]), 1);
  EXPECT_EQ(yy_util::utf8_len(ch_2[0]), 2);
  EXPECT_EQ(yy_util::utf8_len(ch_3[0]), 3);
  EXPECT_EQ(yy_util::utf8_len(ch_4[0]), 4);

  EXPECT_EQ(yy_util::utf8_len(utf8_1_end[0]), 1);
  EXPECT_EQ(yy_util::utf8_len(utf8_2_start[0]), 2);
  EXPECT_EQ(yy_util::utf8_len(utf8_2_end[0]), 2);
  EXPECT_EQ(yy_util::utf8_len(utf8_3_start[0]), 3);
  EXPECT_EQ(yy_util::utf8_len(utf8_3_end[0]), 3);
  EXPECT_EQ(yy_util::utf8_len(utf8_4_start[0]), 4);
  EXPECT_EQ(yy_util::utf8_len(utf8_4_end[0]), 4);
}

TEST_F(TestUTF8, test_utf8_find)
{
  auto ch_1{yy_quad::make_const_span("Z")};
  auto ch_2{yy_quad::make_const_span("Č")};
  auto ch_3{yy_quad::make_const_span("Ḋ")};
  auto ch_4{yy_quad::make_const_span("\xf8\xA0\xA1\xA2")};
  auto str{yy_quad::make_const_span("12345Z" "Č" "Ḋ" "\xf8\xA0\xA1\xA2" "6789")};

  EXPECT_EQ(yy_util::utf8_find(str, ch_1), (yy_util::utf8_result{5, 1}));
  EXPECT_EQ(yy_util::utf8_find(str, ch_2), (yy_util::utf8_result{6, 2}));
  EXPECT_EQ(yy_util::utf8_find(str, ch_3), (yy_util::utf8_result{8, 3}));
  EXPECT_EQ(yy_util::utf8_find(str, ch_4), (yy_util::utf8_result{11, 4}));
  EXPECT_EQ(yy_util::utf8_find(str, yy_quad::make_const_span("A")), (yy_util::utf8_result{str.size(), 0}));
}

TEST_F(TestUTF8, test_utf8_find_1)
{
  auto delim{yy_quad::make_const_span("\xC2\x81")};
  auto str{yy_quad::make_const_span("12345" "\xC2\x80" "\xf8\xA0\xA1\xA2" "\xC2\x81" "6789")};

  EXPECT_EQ(yy_util::utf8_find(str, delim), (yy_util::utf8_result{11, 2}));
}

TEST_F(TestUTF8, test_utf8_find_first_not)
{
  auto delim{yy_quad::make_const_span("A")};

  EXPECT_EQ(yy_util::utf8_find_first_not(yy_quad::make_const_span("AAAAB"), delim), (yy_util::utf8_result{4, 1}));
  EXPECT_EQ(yy_util::utf8_find_first_not(yy_quad::make_const_span("A"), delim), (yy_util::utf8_result{1, 1}));
  EXPECT_EQ(yy_util::utf8_find_first_not(yy_quad::make_const_span(""), delim), (yy_util::utf8_result{0, 0}));
  EXPECT_EQ(yy_util::utf8_find_first_not(yy_quad::make_const_span("BBBBB"), delim), (yy_util::utf8_result{0, 1}));
  EXPECT_EQ(yy_util::utf8_find_first_not(yy_quad::make_const_span("B"), delim), (yy_util::utf8_result{0, 1}));
}

TEST_F(TestUTF8, test_utf8_find_last_ch)
{
  EXPECT_EQ(yy_util::utf8_find_last_ch(utf8_1_start),
            (yy_util::utf8_result{utf8_1_start.size(), 0}));
  EXPECT_EQ(yy_util::utf8_find_last_ch(utf8_1_end), (yy_util::utf8_result{0, 1}));
  EXPECT_EQ(yy_util::utf8_find_last_ch(utf8_2_start), (yy_util::utf8_result{0, 2}));
  EXPECT_EQ(yy_util::utf8_find_last_ch(utf8_2_end), (yy_util::utf8_result{0, 2}));
  EXPECT_EQ(yy_util::utf8_find_last_ch(utf8_3_start), (yy_util::utf8_result{0, 3}));
  EXPECT_EQ(yy_util::utf8_find_last_ch(utf8_3_end), (yy_util::utf8_result{0, 3}));
  EXPECT_EQ(yy_util::utf8_find_last_ch(utf8_4_start), (yy_util::utf8_result{0, 4}));
  EXPECT_EQ(yy_util::utf8_find_last_ch(utf8_4_end), (yy_util::utf8_result{0, 4}));
}

TEST_F(TestUTF8, test_utf8_find_first_of)
{
  auto delim{yy_quad::make_const_span("\xC2\x80\xC2\x81")};

  EXPECT_EQ(yy_util::utf8_find_first_of(yy_quad::make_const_span("12345" "\xC2\x82" "\xf8\xA0\xA1\xA2" "\xC2\x81" "6789"), delim), (yy_util::utf8_result{11, 2}));
  EXPECT_EQ(yy_util::utf8_find_first_of(yy_quad::make_const_span("12345" "\xC2\x82" "\xf8\xA0\xA1\xA2" "\xC2\x80" "6789"), delim), (yy_util::utf8_result{11, 2}));
  EXPECT_EQ(yy_util::utf8_find_first_of(yy_quad::make_const_span("12345" "\xC2\x82" "\xf8\xA0\xA1\xA2" "6789"), delim), (yy_util::utf8_result{15, 1}));
}

} // namespace yafiyogi::yy_data::tests
