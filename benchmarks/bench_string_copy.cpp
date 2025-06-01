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

struct StringCopy:
      public ::benchmark::Fixture
{
  public:
    StringCopy() {};
    void SetUp(const ::benchmark::State &) override {}
};


static const char whitespace[] = {'\000', ' ', '\t', '\n'};

BENCHMARK_F(StringCopy, memcpy_memchr)(::benchmark::State & state)
{
  std::size_t idx = 0;
  char dest[max_dest_size];

  while(state.KeepRunning())
  {
    auto & str = strings[idx];
    const char * src = str.c_str();

    std::size_t copy_size = std::min(sizeof(dest) - 1, str.size());

    for(std::size_t ws_idx = 0; ws_idx < sizeof(whitespace); ++ws_idx)
    {
      if(auto end = static_cast<const char *>(std::memchr(src, whitespace[ws_idx], copy_size));
         nullptr != end)
      {
        copy_size = static_cast<std::size_t>(end - src);
      }
    }

    std::memcpy(dest, src, copy_size);
    *(dest + copy_size) = 0;

    ::benchmark::DoNotOptimize(dest);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(StringCopy, while_loop)(::benchmark::State & state)
{
  std::size_t idx = 0;
  char dest[max_dest_size];

  while(state.KeepRunning())
  {
    char * dst = dest;
    const char * src = strings[idx].c_str();

    std::size_t pos = 0;
    while((*dst = *src))
    {
      ++pos;
      if((pos == sizeof(dest))
         || (nullptr != std::memchr(whitespace, *src, sizeof(whitespace))))
      {
        *dst = 0;
        break;
      }

      ++dst;
      ++src;
    }

    ::benchmark::DoNotOptimize(dest);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}

static constexpr std::string_view whitespace_sv{whitespace, sizeof(whitespace)};

BENCHMARK_F(StringCopy, memcpy_sv_find)(::benchmark::State & state)
{
  std::size_t idx = 0;
  char dest[max_dest_size];

  while(state.KeepRunning())
  {
    const auto & str = strings[idx];

    const char * src = str.c_str();

    std::size_t copy_size = std::min(sizeof(dest) - 1, str.size());

    std::string_view copy_str{src, copy_size};
    std::size_t end_pos = copy_str.find_first_of(whitespace_sv);

    if(std::string_view::npos != end_pos)
    {
      copy_size = end_pos;
    }

    std::memcpy(dest, src, copy_size);
    *(dest + copy_size) = 0;

    ::benchmark::DoNotOptimize(dest);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(StringCopy, memchr_find)(::benchmark::State & state)
{
  std::size_t idx = 0;

  while(state.KeepRunning())
  {
    const auto & str = strings[idx];

    auto start = str.c_str();

    auto pos = static_cast<const char *>(memchr(start, 0, str.size() + 1)) - start;

    ::benchmark::DoNotOptimize(pos);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(StringCopy, char_traits_find)(::benchmark::State & state)
{
  std::size_t idx = 0;

  while(state.KeepRunning())
  {
    const auto & str = strings[idx];

    auto start = str.c_str();
    auto size = str.size() + 1;

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
