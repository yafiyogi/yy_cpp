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
#include <limits>
#include <unordered_map>

#include "benchmark/benchmark.h"

#include "yy_trie.h"
#include "yy_radix_trie.h"

#include "yy_flat_trie.h"
#include "yy_flat_radix_trie.h"

#include "yy_fm_trie.h"
#include "yy_fm_radix_trie.h"

#include "yy_fm_flat_trie_idx.h"
#include "yy_fm_flat_trie_ptr.h"

#include "yy_flat_map.h"


using Trie = yafiyogi::yy_data::trie<char, int>;
using RadixTrie = yafiyogi::yy_data::radix_trie<char, int>;

using FlatTrie = yafiyogi::yy_data::flat_trie<char, int>;
using FlatRadixTrie = yafiyogi::yy_data::flat_radix_trie<char, int>;

using FMTrie = yafiyogi::yy_data::fm_trie<char, int>;
using FMRadixTrie = yafiyogi::yy_data::fm_radix_trie<char, int>;

using FMFlatTrieIdx = yafiyogi::yy_data::fm_flat_trie_idx<char, int>;
using FMFlatTriePtr = yafiyogi::yy_data::fm_flat_trie_ptr<char, int>;

template<typename LabelType>
using tokenizer_type_word = yafiyogi::yy_trie::label_word_tokenizer<LabelType, '/'>;

using FMFlatTrieIdxWord = yafiyogi::yy_data::fm_flat_trie_idx<std::string, int,
                                                              yafiyogi::yy_data::fm_flat_trie_idx_detail::Automaton,
                                                              tokenizer_type_word>;

using FMFlatTriePtrWord = yafiyogi::yy_data::fm_flat_trie_ptr<std::string, int,
                                                              yafiyogi::yy_data::fm_flat_trie_ptr_detail::Automaton,
                                                              tokenizer_type_word>;

using FlatMap = yafiyogi::yy_data::flat_map<std::string, int, yafiyogi::yy_data::ClearAction::Clear, yafiyogi::yy_data::ClearAction::Keep>;
using FlatMapSingle = yafiyogi::yy_data::flat_map<std::string, int, yafiyogi::yy_data::ClearAction::Clear, yafiyogi::yy_data::ClearAction::Keep, ((yafiyogi::yy_data::find_iter_pos_size_threshold_cache_line_size * 1) / sizeof(int)) + 1>;
using FlatMapDouble = yafiyogi::yy_data::flat_map<std::string, int, yafiyogi::yy_data::ClearAction::Clear, yafiyogi::yy_data::ClearAction::Keep, ((yafiyogi::yy_data::find_iter_pos_size_threshold_cache_line_size * 2) / sizeof(int)) + 1>;
using FlatMapBinary = yafiyogi::yy_data::flat_map<std::string, int, yafiyogi::yy_data::ClearAction::Clear, yafiyogi::yy_data::ClearAction::Keep, std::numeric_limits<std::size_t>::min()>;
using FlatMapLinear = yafiyogi::yy_data::flat_map<std::string, int, yafiyogi::yy_data::ClearAction::Clear, yafiyogi::yy_data::ClearAction::Keep, std::numeric_limits<std::size_t>::max()>;
using Map = std::map<std::string, int>;
using UOMap = std::unordered_map<std::string, int>;

namespace yafiyogi::benchmark {

struct TrieLookup:
      public ::benchmark::Fixture
{
  public:
    TrieLookup();
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

    static FMFlatTrieIdx::automaton_type fm_flat_trie_idx;
    static FMFlatTrieIdxWord::automaton_type fm_flat_trie_idx_word;

    static FMFlatTriePtr::automaton_type fm_flat_trie_ptr;
    static FMFlatTriePtrWord::automaton_type fm_flat_trie_ptr_word;

    static FlatMap flat_map;
    static FlatMapSingle flat_map_single;
    static FlatMapDouble flat_map_double;
    static FlatMapBinary flat_map_binary;
    static FlatMapLinear flat_map_linear;
    static Map map;
    static UOMap uo_map;
};



} // namespace yafiyogi::benchmark
