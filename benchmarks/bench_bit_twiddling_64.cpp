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
#include <bit>
#include <limits>
#include <random>

#include "benchmark/benchmark.h"

#include "yy_bit_twiddling.h"

inline constexpr const int max_size = 10000;

using bit_twiddling = yafiyogi::yy_bit_twiddling::bits<uint64_t>;

static std::array<uint64_t, max_size> set_numbers()
{
  std::array<uint64_t, max_size> num;

  std::random_device rd{};
  std::mt19937 generator{rd()};

  std::uniform_int_distribution<uint64_t> uniform_dist{1, std::numeric_limits<uint64_t>::max()};

  for(size_t idx = 0; idx < num.size(); ++idx)
  {
    num[idx] = uniform_dist(generator);
  }

  return num;
}

static auto nums = set_numbers();

struct BitCeil64:
      public ::benchmark::Fixture
{
  public:
    BitCeil64() {};
    void SetUp(const ::benchmark::State &) override {}
};


BENCHMARK_F(BitCeil64, std_bit_ceil)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = std::bit_ceil(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(BitCeil64, round_up_pow2)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = bit_twiddling::round_up_pow2(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

struct BitFloor64:
      public ::benchmark::Fixture
{
  public:
    BitFloor64() {};
    void SetUp(const ::benchmark::State &) override {}
};


BENCHMARK_F(BitFloor64, std_bit_floor)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = std::bit_floor(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(BitFloor64, round_down_pow2)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = bit_twiddling::round_down_pow2(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

struct PopCount64:
      public ::benchmark::Fixture
{
  public:
    PopCount64() {};
    void SetUp(const ::benchmark::State &) override {}
};


BENCHMARK_F(PopCount64, std_popcount)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = std::popcount(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(PopCount64, pop)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = bit_twiddling::pop(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

struct CountZeros64:
      public ::benchmark::Fixture
{
  public:
    CountZeros64() {};
    void SetUp(const ::benchmark::State &) override {}
};


BENCHMARK_F(CountZeros64, std_countl_zero)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = std::countl_zero(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(CountZeros64, nlz)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = bit_twiddling::nlz(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(CountZeros64, std_countr_zero)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = std::countr_zero(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}

BENCHMARK_F(CountZeros64, ntz)(::benchmark::State & state)
{
  size_t idx = 0;

  while(state.KeepRunning())
  {
    auto rv = bit_twiddling::ntz(nums[idx]);
    ::benchmark::DoNotOptimize(rv);

    ++idx;
    if(nums.size() == idx)
    {
      idx = 0;
    }
  }
}
