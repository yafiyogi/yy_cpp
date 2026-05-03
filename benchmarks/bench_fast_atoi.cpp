/*

  MIT License

  Copyright (c) 2026 Yafiyogi

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

#include <array>
#include <limits>
#include <random>
#include <string>
#include <string_view>

#include "benchmark/benchmark.h"
#include "fmt/format.h"

#include "bench_fast_atoi.hpp"

FastAtoi::strings_type FastAtoi::strings;

void FastAtoi::SetUp(const ::benchmark::State &)

{
  if(!setup)
  {
    setup = true;

    std::random_device rd{};
    std::mt19937 generator{rd()};
    std::uniform_int_distribution<std::size_t> int_dist{0, std::numeric_limits<yafiyogi::size_type>::max()};

    for(std::string & str : strings)
    {
      const std::size_t int_value = int_dist(generator);
      str = fmt::format("{}", int_value);
    }
  }
}
