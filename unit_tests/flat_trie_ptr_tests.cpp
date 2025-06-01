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

#include <cstdio>
#include <string>

#include <gtest/gtest.h>
#include <fmt/format.h>

#include "yy_flat_trie_ptr.h"

namespace yafiyogi::yy_data::tests {

class TestFlatTriePtr:
      public testing::Test
{
  public:
    using flat_trie_type = yafiyogi::yy_data::flat_trie_ptr<char, int>;
    using traits = flat_trie_type::traits;

    using ptr_node_type = traits::ptr_node_type;
    using ptr_node_ptr = traits::ptr_node_ptr;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TestFlatTriePtr, TrieNodeGetBeforeAdd)
{
  // No child nodes added.
  ptr_node_type ptr_node{nullptr};

  EXPECT_FALSE(ptr_node.find_edge('a').found);
  EXPECT_FALSE(ptr_node.find_edge('b').found);
}

TEST_F(TestFlatTriePtr, TrieNodeGetAfterAdd)
{
  //auto edge_getter = [](auto &){};

  // Add 'a' node, but not 'b' node.
  ptr_node_type ptr_node{nullptr};
  ptr_node.add_edge(ptr_node.find_edge('a').edge, 'a', ptr_node_ptr{});

  EXPECT_TRUE(ptr_node.find_edge('a').found);
  EXPECT_FALSE(ptr_node.find_edge('b').found);
}

TEST_F(TestFlatTriePtr, TrieNodeAddDuplicate)
{
  // Add 2 'b' nodes.
  ptr_node_type node{nullptr};
  int d1 = 668;
  ptr_node_type node_1{&d1};
  int d2 = 777;
  ptr_node_type node_2{&d2};

  node.add_edge(node.find_edge('b').edge, 'b', ptr_node_ptr{&node_1});
  node.add_edge(node.find_edge('b').edge, 'b', ptr_node_ptr{&node_2});

  ASSERT_TRUE(node.find_edge('b').found);
  EXPECT_EQ(777, *((*node.find_edge('b').edge)->data()));
}

TEST_F(TestFlatTriePtr, TestNodeChildOrder)
{
  // Add 'a', 'b', 'c', & 'd' nodes out of order.
  // Check node has children in order.
  ptr_node_type node{nullptr};

  int a = 2;
  ptr_node_type node_a(&a);
  int b = 1;
  ptr_node_type node_b(&b);
  int c = 4;
  ptr_node_type node_c(&c);
  int d = 3;
  ptr_node_type node_d(&d);

  const char * result = "abcd";
  const int data[] = {a, b, c, d};
  int idx = 0;

  node.add_edge(node.find_edge('b').edge, 'b', ptr_node_ptr{&node_b});
  node.add_edge(node.find_edge('a').edge, 'a', ptr_node_ptr{&node_a});
  node.add_edge(node.find_edge('d').edge, 'd', ptr_node_ptr{&node_d});
  node.add_edge(node.find_edge('c').edge, 'c', ptr_node_ptr{&node_c});

  node.visit([&result, &data, &idx](const auto & k, const auto & edge) {
    EXPECT_TRUE(k == *result);
    EXPECT_EQ(data[idx], *(edge->data()));
    ++result;
    ++idx;
  });
}

TEST_F(TestFlatTriePtr, TrieAddThreeThirdOverlap_R_Value)
{
  // Add 'abcd', 'abfg'
  // Test add 'ab'
  flat_trie_type trie{};

  // Check 'abcd', 'abef' & 'ab' don't exist.
  auto automaton = trie.create_automaton();
  ASSERT_FALSE(automaton.find("abcd"));
  ASSERT_FALSE(automaton.find("abef"));
  ASSERT_FALSE(automaton.find("ab"));

  // Add 'abcd' with value 668.
  trie.add("abcd", 668);

  // Check 'abcd' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("abcd"));
  automaton.visit([](auto & payload) {
    EXPECT_EQ(668, payload);
  });

  // Add 'abef' with value 777.
  trie.add("abef", 777);

  // Check 'abef' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("abef"));
  automaton.visit([](const auto & payload) {
    EXPECT_EQ(777, payload);
  });

  // Add 'ab' with value 888.
  trie.add("ab", 888);

  // Check 'ab' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("ab"));
  automaton.visit([](const auto & payload)
  {
    EXPECT_EQ(888, payload);
  });

  // Check 'abcd' exists & value correct.
  ASSERT_TRUE(automaton.find("abcd"));
  automaton.visit([](const auto & payload) {
    EXPECT_EQ(668, payload);
  });

  // Check 'abef' exists & value correct.
  ASSERT_TRUE(automaton.find("abef"));
  automaton.visit([](const auto & payload) {
    EXPECT_EQ(777, payload);
  });
}

TEST_F(TestFlatTriePtr, Trie_R_Value)
{
  flat_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  ASSERT_FALSE(automaton.find("1234"));

  // Add '1234' with value 668.
  trie.add("1234", 668);

  // Check '1234' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("1234"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Check '123' doesn't exist.
  ASSERT_FALSE(automaton.find("123"));

  // Add '12345' with value 2
  trie.add("12345", 777);

  // Check '12345' exists & value correct.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("12345"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});

  // Check '1234' still exists & value still correct.
  ASSERT_TRUE(automaton.find("1234"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Add duplicate '1234'
  trie.add("1234", 888);
  // Check '1234' still exists & value unchanged.
  automaton = trie.create_automaton();
  ASSERT_TRUE(automaton.find("1234"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});
}

TEST_F(TestFlatTriePtr, TrieAddThreeThirdOverlap_L_Value)
{
  // Add 'abcd', 'abfg'
  // Test add 'ab'
  flat_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcd', 'abef' & 'ab' don't exist.
  ASSERT_FALSE(automaton.find("abcd"));
  ASSERT_FALSE(automaton.find("abef"));
  ASSERT_FALSE(automaton.find("ab"));

  // Add 'abcd' with value 668.
  auto value_668 = 668;
  trie.add("abcd", value_668);

  // Check 'abcd' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("abcd"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add 'abef' with value 777.
  auto value_777 = 777;
  trie.add("abef", value_777);

  // Check 'abef' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("abef"));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Add 'ab' with value 888.
  auto value_888 = 888;
  trie.add("ab", value_888);

  // Check 'ab' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("ab"));
  automaton.visit([&value_888](const auto & payload) { EXPECT_EQ(value_888, payload);});

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});
}

TEST_F(TestFlatTriePtr, Trie_L_Value)
{
  flat_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  ASSERT_FALSE(automaton.find("1234"));

  // Add '1234' with value 668.
  auto value_668 = 668;
  trie.add("1234", value_668);

  // Check '1234' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check '123' doesn't exist.
  ASSERT_FALSE(automaton.find("123"));

  // Add '12345' with value 2
  auto value_777 = 777;
  trie.add("12345", value_777);

  // Check '12345' exists & value correct.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("12345"));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Check '1234' still exists & value still correct.
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add duplicate '1234'
  auto value_888 = 888;
  trie.add("1234", value_888);
  // Check '1234' still exists & value unchanged.
  automaton = trie.create_automaton();
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});
}

} // namespace yafiyogi::yy_data::tests
