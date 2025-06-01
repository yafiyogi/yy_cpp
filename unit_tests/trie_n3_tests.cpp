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

#include <string>
#include <string_view>

#include "fmt/format.h"
#include "gtest/gtest.h"

#include "yy_trie_n3.h"

namespace yafiyogi::yy_data::tests {

class TestTrieN3:
      public testing::Test
{
  public:
    using trie_type = yafiyogi::yy_data::trie_n3<char, int>;

    using trie_node = trie_type::node_type;
    using value_node = trie_type::value_node;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TestTrieN3, TrieNodeGetBeforeAdd)
{
  // No child nodes added.
  trie_node node{};
  EXPECT_FALSE(node.find_edge('a').found);
  EXPECT_FALSE(node.find_edge('b').found);
}

TEST_F(TestTrieN3, TrieNodeGetAfterAdd)
{
  // Add 'a' node, but not 'b' node.
  trie_node node{};
  node.add_edge(node.find_edge('a').edge, 'a', std::make_unique<trie_node>());

  EXPECT_TRUE(node.find_edge('a').found);
  EXPECT_FALSE(node.find_edge('b').found);
}

TEST_F(TestTrieN3, TestNodeChildOrder)
{
  // Add 'a', 'b', 'c', & 'd' nodes out of order.
  // Check node has children in order.
  trie_node node{};
  const char * result = "abcd";

  node.add_edge(node.find_edge('b').edge, 'b', std::make_unique<trie_node>());
  node.add_edge(node.find_edge('a').edge, 'a', std::make_unique<trie_node>());
  node.add_edge(node.find_edge('d').edge, 'd', std::make_unique<trie_node>());
  node.add_edge(node.find_edge('c').edge, 'c', std::make_unique<trie_node>());

  node.visit([&result](const auto & k, const auto & /* node */) {
    EXPECT_TRUE(k == *result);
    ++result;
  });
}

TEST_F(TestTrieN3, TestNodeValue)
{
  trie_node node{};
  const trie_node const_node{};

  EXPECT_THROW(std::ignore = node.value(), std::runtime_error);
  EXPECT_THROW(std::ignore = const_node.value(), std::runtime_error);
}

TEST_F(TestTrieN3, TestNodeAddNodeEdge)
{
  // Add 2 'e' node edges.
  // Make sure first added is overwritten by second.
  trie_node node{};

  node.add_edge(node.find_edge('e').edge,
                'e', yy_util::static_unique_cast<trie_node>(std::make_unique<value_node>(668)));
  auto e1 = node.find_edge('e').edge->get()->value();

  node.add_edge(node.find_edge('e').edge,
           'e', yy_util::static_unique_cast<trie_node>(std::make_unique<value_node>(777)));
  auto e2 = node.find_edge('e').edge->get()->value();

  EXPECT_NE(e1, e2);
}

TEST_F(TestTrieN3, TrieAddThreeThirdOverlap_R_Value)
{
  // Add 'abcd', 'abfg'
  // Test add 'ab'
  trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcd', 'abef' & 'ab' don't exist.
  EXPECT_FALSE(automaton.find("abcd"));
  EXPECT_FALSE(automaton.find("abef"));
  EXPECT_FALSE(automaton.find("ab"));

  // Add 'abcd' with value 668.
  trie.add("abcd", 668);

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Add 'abef' with value 777.
  trie.add("abef", 777);

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});
  ASSERT_TRUE(automaton.find("abcd"));

  // Add 'ab' with value 888.
  trie.add("ab", 888);

  // Check 'ab' exists & value correct.
  EXPECT_TRUE(automaton.find("ab"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(888, payload);});

  // Check 'abcd' exists & value correct.
  ASSERT_TRUE(automaton.find("abcd"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Check 'abef' exists & value correct.
  ASSERT_TRUE(automaton.find("abef"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});
}

TEST_F(TestTrieN3, Trie_R_Value)
{
  trie_type trie;
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  EXPECT_FALSE(automaton.find("1234"));

  // Add '1234' with value 668.
  trie.add("1234", 668);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Check '123' doesn't exist.
  EXPECT_FALSE(automaton.find("123"));

  // Add '12345' with value 2
  trie.add("12345", 777);

  // Check '12345' exists & value correct.
  EXPECT_TRUE(automaton.find("12345"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});

  // Check '1234' still exists & value still correct.
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Add duplicate '1234'
  trie.add("1234", 888);
  // Check '1234' still exists & value changed.
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([](const auto & payload) { EXPECT_EQ(888, payload);});
}

TEST_F(TestTrieN3, TrieAddThreeThirdOverlap_L_Value)
{
  // Add 'abcd', 'abfg'
  // Test add 'ab'
  trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcd', 'abef' & 'ab' don't exist.
  EXPECT_FALSE(automaton.find("abcd"));
  EXPECT_FALSE(automaton.find("abef"));
  EXPECT_FALSE(automaton.find("ab"));

  // Add 'abcd' with value 668.
  auto value_668 = 668;
  trie.add("abcd", value_668);

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add 'abef' with value 777.
  auto value_777 = 777;
  trie.add("abef", value_777);

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Add 'ab' with value 888.
  auto value_888 = 888;
  trie.add("ab", value_888);

  // Check 'ab' exists & value correct.
  EXPECT_TRUE(automaton.find("ab"));
  automaton.visit([&value_888](const auto & payload) { EXPECT_EQ(value_888, payload);});

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});
}

TEST_F(TestTrieN3, Trie_L_Value)
{
  trie_type trie;
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  EXPECT_FALSE(automaton.find("1234"));

  // Add '1234' with value 668.
  auto value_668 = 668;
  trie.add("1234", value_668);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check '123' doesn't exist.
  EXPECT_FALSE(automaton.find("123"));

  // Add '12345' with value 2
  auto value_777 = 777;
  trie.add("12345", value_777);

  // Check '12345' exists & value correct.
  EXPECT_TRUE(automaton.find("12345"));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Check '1234' still exists & value still correct.
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add duplicate '1234'
  auto value_888 = 888;
  trie.add("1234", value_888);
  // Check '1234' still exists & value changed.
  EXPECT_TRUE(automaton.find("1234"));
  automaton.visit([&value_888](const auto & payload) { EXPECT_EQ(value_888, payload);});
}

} // namespace yafiyogi::yy_data::tests
