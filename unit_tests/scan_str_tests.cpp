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

#include "fmt/format.h"
#include "fmt/ranges.h"

#include <string>
#include <string_view>

#include "gtest/gtest.h"

#include "yy_scan_str.hpp"

namespace yafiyogi::yy_cpp::tests {

class TestScanStr:
      public testing::Test
{
  public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    std::string_view str_in_1{"123456 7890"};
    std::string_view str_in_2{"1234567890"};
};

TEST_F(TestScanStr, scan_str_simple_c_array_out)
{
  char str_out[64];

  {
    std::string_view str_expect{"123456"};

    EXPECT_EQ(1, yy_util::scan_str(str_in_1, "%s", str_out));
    EXPECT_EQ(str_expect, std::string_view{str_out});
  }

  {
    std::string_view str_expect{str_in_2};

    EXPECT_EQ(1, yy_util::scan_str(str_in_2, "%s", str_out));
    EXPECT_EQ(str_expect, std::string_view{str_out});
  }

  {
    std::string_view str_expect{"12345"};

    EXPECT_EQ(1, yy_util::scan_str(str_in_2, "%5s", str_out));
    EXPECT_EQ(str_expect, std::string_view{str_out});
  }
}

TEST_F(TestScanStr, scan_str_simple_char_vector_out)
{
  std::vector<char> str_out;

  {
    std::string_view str_expect{"123456"};

    EXPECT_EQ(0, yy_util::scan_str(str_in_1, "%s", str_out));
  }

  {
    std::string_view str_expect{"123456"};

    str_out = std::vector<char>(7);

    EXPECT_EQ(1, yy_util::scan_str(str_in_1, "%s", str_out));
    EXPECT_EQ(str_expect, (std::string_view{str_out.data()}));
  }

  {
    std::string_view str_expect{str_in_2};

    str_out = std::vector<char>(str_in_2.size() + 1);

    EXPECT_EQ(1, yy_util::scan_str(str_in_2, "%s", str_out));
    EXPECT_EQ(str_expect, (std::string_view{str_out.data()}));
  }

  {
    std::string_view str_expect{"12345"};

    str_out = std::vector<char>(6);

    EXPECT_EQ(1, yy_util::scan_str(str_in_2, "%5s", str_out));
    EXPECT_EQ(str_expect, (std::string_view{str_out.data()}));
  }
}


TEST_F(TestScanStr, scan_str_std_string_out)
{
  std::string str_out;

  {
    std::string_view str_expect{"123456"};

    EXPECT_EQ(1, yy_util::scan_str(str_in_1, "%s", str_out));
    EXPECT_EQ(str_expect, std::string_view{str_out});
  }

  {
    std::string_view str_expect{str_in_2};

    EXPECT_EQ(1, yy_util::scan_str(str_in_2, "%s", str_out));
    EXPECT_EQ(str_expect, std::string_view{str_out});
  }

  {
    std::string_view str_expect{"12345"};

    EXPECT_EQ(1, yy_util::scan_str(str_in_2, "%5s", str_out));
    EXPECT_EQ(str_expect, std::string_view{str_out});
  }
}

} // namespace yafiyogi::yy_cpp::tests
