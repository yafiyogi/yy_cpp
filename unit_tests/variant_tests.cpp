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

#include "fmt/format.h"
#include <type_traits>

#include <gtest/gtest.h>

#include "yy_variant_util.h"

namespace yafiyogi::yy_cpp::tests {

class TestVariantTraits:
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

TEST_F(TestVariantTraits, TestTypeIsInVariant)
{
  using var = std::variant<bool, double, std::string>;

  ASSERT_TRUE((yy_util::check_variant_for_type_v<var, bool>));
  ASSERT_TRUE((yy_util::check_variant_for_type_v<var, double>));
  ASSERT_TRUE((yy_util::check_variant_for_type_v<var, std::string>));
}

TEST_F(TestVariantTraits, TestTypeIsNotInVariant)
{
  using var = std::variant<bool, double, std::string>;

  ASSERT_FALSE((yy_util::check_variant_for_type_v<var, int>));
}

} // namespace yafiyogi::yy_cpp::tests
