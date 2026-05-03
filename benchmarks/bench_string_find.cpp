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

#include <array>
#include <random>
#include <string>
#include <string_view>
#include <cstring>

#include "benchmark/benchmark.h"

#include "yy_find_util.hpp"

namespace {

static constexpr const int max_array_size = 100000;
static constexpr const int max_string_size = 4096;
static constexpr const int max_dest_size = 64;

using strings_type = std::array<std::string, max_array_size>;

static constexpr std::string_view charset{"\n\t01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};

strings_type set_strings()
{
  std::random_device rd{};
  std::mt19937 generator{rd()};
  std::uniform_int_distribution<std::size_t> char_dist{0, charset.size()};
  std::uniform_int_distribution<std::size_t> string_size_dist{0, max_string_size};

  strings_type strings{};

  for(std::size_t idx = 0; idx < strings.size(); ++idx)
  {
    const std::size_t str_size = string_size_dist(generator);
    auto & str = strings[idx];
    str.reserve(str_size + 1);

    for(std::size_t ch_idx = 0; ch_idx < str_size; ++ch_idx)
    {
      char ch = charset[char_dist(generator)];
      str += ch;
    }
  }

  return strings;
}

static strings_type strings = set_strings();

struct StringFind:
      public ::benchmark::Fixture
{
  public:
    StringFind() {};
    void SetUp(const ::benchmark::State &) override {}
};


static constexpr char whitespace[] = {'\000', ' ', '\t', '\n'};

}

BENCHMARK_F(StringFind, memchr)(::benchmark::State & state)
{
  std::size_t idx = 0;

  while(state.KeepRunning())
  {
    const auto & str = strings[idx];

    const auto start = str.c_str();
    auto pos = static_cast<const char *>(memchr(start, 0, str.size() + 1)) - start;

    ::benchmark::DoNotOptimize(pos);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(StringFind, std_find)(::benchmark::State & state)
{
  std::size_t idx = 0;

  while(state.KeepRunning())
  {
    const auto & str = strings[idx];

    const auto start = str.c_str();
    const auto size = str.size() + 1;
    auto end = std::find(start, start + size, 0);

    if(nullptr == end)
    {
      end = start + size;
    }

    auto pos = end - start;

    ::benchmark::DoNotOptimize(pos);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(StringFind, char_traits_find)(::benchmark::State & state)
{
  std::size_t idx = 0;

  while(state.KeepRunning())
  {
    const auto & str = strings[idx];

    const auto start = str.c_str();
    const auto size = str.size() + 1;

    auto end = std::char_traits<char>::find(start, size, 0);

    if(nullptr == end)
    {
      end = start + size;
    }

    auto pos = end - start;

    ::benchmark::DoNotOptimize(pos);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(StringFind, find_pos_linear)(::benchmark::State & state)
{
  std::size_t idx = 0;

  while(state.KeepRunning())
  {
    const auto & str = strings[idx];

    const auto start = str.c_str();
    const auto size = str.size() + 1;

    auto pos = yafiyogi::yy_data::find_pos_linear(start, size, 0).pos;

    ::benchmark::DoNotOptimize(pos);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}
