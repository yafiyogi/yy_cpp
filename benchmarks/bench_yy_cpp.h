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

#pragma once

#include <string_view>
#include <string>
#include <map>
#include <unordered_map>

#include "benchmark/benchmark.h"

#include "yy_trie.h"
#include "yy_radix_trie.h"

#include "yy_flat_trie.h"
#include "yy_flat_radix_trie.h"

#include "yy_fm_trie.h"
#include "yy_fm_radix_trie.h"

#include "yy_fm_flat_trie_ptr.h"


using Trie = yafiyogi::yy_data::trie<char, int>;
using RadixTrie = yafiyogi::yy_data::radix_trie<char, int>;

using FlatTrie = yafiyogi::yy_data::flat_trie<char, int>;
using FlatRadixTrie = yafiyogi::yy_data::flat_radix_trie<char, int>;

using FMTrie = yafiyogi::yy_data::fm_trie<char, int>;
using FMRadixTrie = yafiyogi::yy_data::fm_radix_trie<char, int>;

using FMFlatTriePtr = yafiyogi::yy_data::fm_flat_trie_ptr<char, int>;

using Map = std::map<std::string, int>;
using UOMap = std::unordered_map<std::string, int>;

namespace yafiyogi::benchmark {

struct TrieFixtureType:
      public ::benchmark::Fixture
{
  public:
    TrieFixtureType();
    void SetUp(const ::benchmark::State &) override;

    static std::string_view query(size_t idx);
    static size_t query_size();
    static size_t topics_size();

    static Trie trie;
    static RadixTrie radix_trie;

    static FlatTrie flat_trie;
    static FlatRadixTrie flat_radix_trie;

    static FMTrie fm_trie;
    static FMRadixTrie fm_radix_trie;

    static FMFlatTriePtr fm_flat_trie_ptr;

    static Map map;
    static UOMap uo_map;

};



} // namespace yafiyogi::benchmark
