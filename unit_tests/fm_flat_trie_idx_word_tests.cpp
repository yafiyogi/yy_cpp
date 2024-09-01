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

#include <cstdio>
#include <string>

#include <gtest/gtest.h>
#include <fmt/format.h>

#include "yy_fm_flat_trie_idx.h"

namespace yafiyogi::yy_data::tests {

using namespace std::string_literals;
using namespace std::string_view_literals;

class TestFMFlatTrieIdxWord:
      public testing::Test
{
  public:
    template<typename LabelType>
    using fm_flat_idx_word_tokenizer_type = yafiyogi::yy_trie::label_word_tokenizer<LabelType, '/'>;

    using flat_trie_type = yafiyogi::yy_data::fm_flat_trie_idx<std::string, int,
                                                               yafiyogi::yy_data::fm_flat_trie_idx_detail::Automaton,
                                                               fm_flat_idx_word_tokenizer_type>;
    using traits = flat_trie_type::traits;
    using trie_node = flat_trie_type::node_type;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TestFMFlatTrieIdxWord, TrieNodeGetBeforeAdd)
{
  //auto edge_getter = [](auto &){};

  // No child nodes added.
  trie_node node{trie_node::no_data};

  EXPECT_FALSE(node.find_edge([](auto, auto){}, "a"sv).found);
  EXPECT_FALSE(node.find_edge([](auto, auto){}, "b"sv).found);
}

TEST_F(TestFMFlatTrieIdxWord, TrieNodeGetAfterAdd)
{
  //auto edge_getter = [](auto &){};

  // Add 'a' node, but not 'b' node.
  trie_node node{trie_node::no_data};
  node.add_edge(node.find_edge_pos("a"sv).pos, "a"s, 1);

  EXPECT_TRUE(node.find_edge([](auto, auto){}, "a"sv).found);
  EXPECT_FALSE(node.find_edge([](auto, auto){}, "b"sv).found);
}

TEST_F(TestFMFlatTrieIdxWord, TrieNodeAddDuplicate)
{
  // Add 2 'b' nodes.
  trie_node node{trie_node::no_data};
  node.add_edge(node.find_edge_pos("b"sv).pos, "b"s, 668);
  node.add_edge(node.find_edge_pos("b"sv).pos, "b"s, 777);

  ASSERT_TRUE(node.find_edge([](auto, auto){}, "b"s).found);
  // EXPECT_EQ(777, edge_idx);
}

TEST_F(TestFMFlatTrieIdxWord, TestNodeChildOrder)
{
  // Add 'a', 'b', 'c', & 'd' nodes out of order.
  // Check node has children in order.
  trie_node node{trie_node::no_data};
  const char * result[] = {"a", "b", "c", "d"};
  int r_idx = 0;
  const size_t idxs[] = {2, 1, 4, 3};
  const size_t * idx = idxs;

  node.add_edge(node.find_edge_pos("b"sv).pos, "b"s, 1);
  node.add_edge(node.find_edge_pos("a"sv).pos, "a"s, 2);
  node.add_edge(node.find_edge_pos("d"sv).pos, "d"s, 3);
  node.add_edge(node.find_edge_pos("c"sv).pos, "c"s, 4);

  node.visit([&result, &r_idx, &idx](const auto & k, const auto & node_idx) {
    EXPECT_TRUE(k == result[r_idx]);
    EXPECT_EQ(node_idx, *idx);
    ++r_idx;
    ++idx;
  });
}

TEST_F(TestFMFlatTrieIdxWord, TrieAddThreeThirdOverlap_R_Value)
{
  // Add 'ab/cd', 'ab/fg'
  // Test add 'ab'
  flat_trie_type trie{};

  // Check 'ab/cd', 'abef' & 'ab' don't exist.
  auto automaton = trie.create_automaton();
  ASSERT_FALSE(automaton.find("ab/cd"sv));
  ASSERT_FALSE(automaton.find("ab/ef"sv));
  ASSERT_FALSE(automaton.find("ab"sv));

  // Add 'ab/cd' with value 668.
  trie.add("ab/cd", 668);

  // Check 'ab/cd' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("ab/cd"sv));
  automaton.visit([](auto & payload) {
    EXPECT_EQ(668, payload);
  });

  // Add 'ab/ef' with value 777.
  trie.add("ab/ef", 777);

  // Check 'ab/ef' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("ab/cd"sv));
  ASSERT_TRUE(automaton.find("ab/ef"sv));
  automaton.visit([](const auto & payload) {
    EXPECT_EQ(777, payload);
  });

  // Add 'ab' with value 888.
  trie.add("ab"sv, 888);

  // Check 'ab' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("ab"sv));
  automaton.visit([](const auto & payload)
  {
    EXPECT_EQ(888, payload);
  });

  // Check 'ab/cd' exists & value correct.
  ASSERT_TRUE(automaton.find("ab/cd"sv));
  automaton.visit([](const auto & payload) {
    EXPECT_EQ(668, payload);
  });

  // Check 'ab/ef' exists & value correct.
  ASSERT_TRUE(automaton.find("ab/ef"sv));
  automaton.visit([](const auto & payload) {
    EXPECT_EQ(777, payload);
  });
}

TEST_F(TestFMFlatTrieIdxWord, Trie_R_Value)
{
  flat_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  ASSERT_FALSE(automaton.find("1234"sv));

  // Add '1234' with value 668.
  trie.add("1234"sv, 668);

  // Check '1234' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("1234"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Check '123' doesn't exist.
  ASSERT_FALSE(automaton.find("123"sv));

  // Add '12345' with value 2
  trie.add("12345", 777);

  // Check '12345' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("12345"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});

  // Check '1234' still exists & value still correct.
  ASSERT_TRUE(automaton.find("1234"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Add duplicate '1234'
  trie.add("1234"sv, 888);
  // Check '1234' still exists & value unchanged.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("1234"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});
}

TEST_F(TestFMFlatTrieIdxWord, TrieAddThreeThirdOverlap_L_Value)
{
  // Add 'abcd', 'abfg'
  // Test add 'ab'
  flat_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcd', 'abef' & 'ab' don't exist.
  ASSERT_FALSE(automaton.find("abcd"sv));
  ASSERT_FALSE(automaton.find("abef"sv));
  ASSERT_FALSE(automaton.find("ab"sv));

  // Add 'abcd' with value 668.
  auto value_668 = 668;
  trie.add("abcd", value_668);

  // Check 'abcd' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("abcd"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add 'abef' with value 777.
  auto value_777 = 777;
  trie.add("abef", value_777);

  // Check 'abef' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("abef"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Add 'ab' with value 888.
  auto value_888 = 888;
  trie.add("ab"sv, value_888);

  // Check 'ab' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("ab"sv));
  automaton.visit([&value_888](const auto & payload) { EXPECT_EQ(value_888, payload);});

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});
}

TEST_F(TestFMFlatTrieIdxWord, Trie_L_Value)
{
  flat_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  ASSERT_FALSE(automaton.find("1234"sv));

  // Add '1234' with value 668.
  auto value_668 = 668;
  trie.add("1234"sv, value_668);

  // Check '1234' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check '123' doesn't exist.
  ASSERT_FALSE(automaton.find("123"sv));

  // Add '12345' with value 2
  auto value_777 = 777;
  trie.add("12345"sv, value_777);

  // Check '12345' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("12345"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Check '1234' still exists & value still correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add duplicate '1234'
  auto value_888 = 888;
  trie.add("1234"sv, value_888);
  // Check '1234' still exists & value unchanged.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});
}

} // namespace yafiyogi::yy_data::tests
