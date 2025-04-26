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

#include "bench_trie_lookup.h"

namespace yafiyogi::benchmark {

BENCHMARK_F(TrieLookup, map_lookup)(::benchmark::State & state)
{
  size_t idx = 0;
  std::size_t count = 0;
  std::string query;
  query.reserve(256);

  while(state.KeepRunning())
  {
    auto query_view = TrieLookup::query(idx);
    query.assign(query_view.begin(), query_view.end());

    auto iter = map.find(query);
    ::benchmark::DoNotOptimize(iter);
    if(iter != map.end())
    {
        ::benchmark::DoNotOptimize(++count);
    }

    ++idx;
    if(query_size() == idx)
    {
      idx = 0;
    }
  }
}

} // namespace yafiyogi::benchmark
