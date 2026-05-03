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

#include "variants/yy_fast_atoi_idx.h"

#include "bench_fast_atoi.hpp"

BENCHMARK_F(FastAtoi, fast_atoi_idx)(::benchmark::State & state)
{
  using size_type = yafiyogi::size_type;
  using fast_atoi = yafiyogi::yy_util::fast_atoi_idx<size_type>;

  std::size_t idx = 0;
  yafiyogi::size_type dest = 0;

  while(state.KeepRunning())
  {
    dest = fast_atoi::convert(FastAtoi::strings[idx]).value;

    ::benchmark::DoNotOptimize(dest);

    ++idx;
    if(strings.size() == idx)
    {
      idx = 0;
    }
  }
}
