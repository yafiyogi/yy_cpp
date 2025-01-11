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
#include <array>
#include <tuple>

#include "benchmark/benchmark.h"


struct CompareUtil:
      public ::benchmark::Fixture
{
  public:
    CompareUtil() {};
    void SetUp(const ::benchmark::State &) override {}
};

std::array<std::tuple<std::string_view, std::string_view>, 6> strs{{
  {"1234567890", "1234567890"},
  {"1234567890", "1234567891"},
  {"1234567891", "1234567890"},
  {"9876543210", "9876543210"},
  {"9876543210", "9876543211"},
  {"9876543211", "9876543212"}
}};

BENCHMARK_F(CompareUtil, lexicographical_compare)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto [a , b] = strs[idx];
    const auto a_size{a.size()};
    const auto a_begin{a.data()};
    const auto b_size{b.size()};
    const auto b_begin{b.data()};

    auto rv = std::lexicographical_compare(a_begin, a_begin + a_size,
                                           b_begin, b_begin + b_size);

    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(strs.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(CompareUtil, char_traits_compare)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto [a , b] = strs[idx];
    const auto a_size{a.size()};

    auto rv = std::char_traits<char>::compare(a.data(), b.data(), a_size);

    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(strs.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(CompareUtil, equal)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto [a , b] = strs[idx];
    const auto a_size{a.size()};
    const auto a_begin{a.data()};

    auto rv = std::equal(a_begin, a_begin + a_size, b.data());

    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(strs.size() == idx)
    {
      idx = 0;
    }
  }
}
