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

#include <gtest/gtest.h>
#include "fmt/format.h"
#include "yy_fm_radix_trie.h"

namespace yafiyogi::yy_data::tests {

using namespace std::string_literals;
using namespace std::string_view_literals;

class TestFMRadixTrie:
      public testing::Test
{
  public:
    using radix_trie_type = yafiyogi::yy_data::fm_radix_trie<char, int>;

    using trie_node = radix_trie_type::node_type;
    using value_node = radix_trie_type::value_node;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TestFMRadixTrie, TrieNodeGetBeforeAdd)
{
  // No child nodes added.
  trie_node node{};

  EXPECT_EQ(0, node.find_edge(yy_quad::make_const_span("a"sv)).common);
  EXPECT_EQ(0, node.find_edge(yy_quad::make_const_span("b"sv)).common);
}

TEST_F(TestFMRadixTrie, TrieNodeGetAfterAdd)
{
  // Add 'ab' node, but not 'b' node.
  auto source_a = yy_quad::make_const_span("ab"sv);
  trie_node node{};
  node.add_edge(trie_node::label_type{source_a.begin(), source_a.end()},
                std::make_unique<trie_node>());

  EXPECT_NE(0, node.find_edge(yy_quad::make_const_span("ab"sv)).common);
  EXPECT_NE(0, node.find_edge(yy_quad::make_const_span("a"sv)).common);
  EXPECT_EQ(1, node.find_edge(yy_quad::make_const_span("a"sv)).common);
  EXPECT_EQ(1, node.find_edge(yy_quad::make_const_span("a"sv)).remaining);
  EXPECT_EQ(0, node.find_edge(yy_quad::make_const_span("b"sv)).common);
}

TEST_F(TestFMRadixTrie, TestNodeChildOrder)
{
  // Add 'a', 'b', 'c', & 'd' nodes out of order.
  // Check node has children in order.
  trie_node node{};
  const char * result = "abcd";

  auto source_a = yy_quad::make_const_span("a"sv);
  auto source_b = yy_quad::make_const_span("b"sv);
  auto source_c = yy_quad::make_const_span("c"sv);
  auto source_d = yy_quad::make_const_span("d"sv);


  node.add_edge(trie_node::label_type{source_b.begin(), source_b.end()}, std::make_unique<trie_node>());
  node.add_edge(trie_node::label_type{source_a.begin(), source_a.end()}, std::make_unique<trie_node>());
  node.add_edge(trie_node::label_type{source_d.begin(), source_d.end()}, std::make_unique<trie_node>());
  node.add_edge(trie_node::label_type{source_c.begin(), source_c.end()}, std::make_unique<trie_node>());

  node.visit([&result](const auto & k, const auto & /* node */) {
    EXPECT_TRUE(k[0] == *result);
    ++result;
  });
}


TEST_F(TestFMRadixTrie, TestNodeValue)
{
  trie_node node{};
  const trie_node const_node{};

  EXPECT_THROW(std::ignore = node.value(), std::runtime_error);
  EXPECT_THROW(std::ignore = const_node.value(), std::runtime_error);
}

TEST_F(TestFMRadixTrie, TrieAddToEmptyTrie_R_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  EXPECT_FALSE(automaton.find("1234"sv));

  // Add '1234' with value 1.
  trie.add("1234"sv, 1);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(1, payload);});

  // Check '123' doesn't exist.
  EXPECT_FALSE(automaton.find("123"sv));

  // Check '12345' doesn't exist.
  EXPECT_FALSE(automaton.find("12345"sv));
}

TEST_F(TestFMRadixTrie, TrieAddTwoWithNoInteresct_R_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' & 'abcd' don't exist.
  EXPECT_FALSE(automaton.find("1234"sv));
  EXPECT_FALSE(automaton.find("abcd"sv));

  // Add '1234' with value 1.
  trie.add("1234"sv, 668);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Add 'abcd' with value 2.
  trie.add("abcd"sv, 777);

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});

  // Check '123' & 'abc' don't exist.
  EXPECT_FALSE(automaton.find("123"sv));
  EXPECT_FALSE(automaton.find("abc"sv));

  // Check '12345' & 'abcde' don't exist.
  EXPECT_FALSE(automaton.find("12345"sv));
  EXPECT_FALSE(automaton.find("abcde"sv));
}

TEST_F(TestFMRadixTrie, TrieAddTwoWithInteresct_R_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcde' & 'abxyz' don't exist.
  EXPECT_FALSE(automaton.find("abcde"sv));
  EXPECT_FALSE(automaton.find("abxyz"sv));

  // Add 'abcde' with value 1.
  trie.add("abcde"sv, 1);

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(1, payload);});

  // Add 'abwxy' with value 2.
  trie.add("abwxy"sv, 2);

  // Check 'abwxy' exists & value correct.
  EXPECT_TRUE(automaton.find("abwxy"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(2, payload);});

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(1, payload);});

  // Check 'a' does exist.
  EXPECT_FALSE(automaton.find("a"sv));

  // Check 'ab' does exist.
  EXPECT_FALSE(automaton.find("ab"sv));

  // Check 'abc' & 'abw' don't exist.
  EXPECT_FALSE(automaton.find("abc"sv));
  EXPECT_FALSE(automaton.find("abw"sv));

  // Check 'abcdef' & 'abwxyz' don't exist.
  EXPECT_FALSE(automaton.find("abcdef"sv));
  EXPECT_FALSE(automaton.find("abwxyz"sv));
}

TEST_F(TestFMRadixTrie, TrieAddTwoFirstOverlap_R_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcde' & 'abc' don't exist.
  EXPECT_FALSE(automaton.find("abcde"sv));
  EXPECT_FALSE(automaton.find("abc"sv));

  // Add 'abcde' with value 668.
  trie.add("abcde"sv, 668);

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Add 'abc' with value 777.
  trie.add("abc"sv, 777);

  // Check 'abc' exists & value correct.
  EXPECT_TRUE(automaton.find("abc"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Check 'a' does exist.
  EXPECT_FALSE(automaton.find("a"sv));

  // Check 'ab' does exist.
  EXPECT_FALSE(automaton.find("ab"sv));

  // Check 'abw' doesn't exist.
  EXPECT_FALSE(automaton.find("abw"sv));

  // Check 'abcd', 'abcdef', & 'abwxyz' don't exist.
  EXPECT_FALSE(automaton.find("abcd"sv));
  EXPECT_FALSE(automaton.find("abcdef"sv));
  EXPECT_FALSE(automaton.find("abwxyz"sv));
}

TEST_F(TestFMRadixTrie, TrieAddTwoSecondOverlap_R_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcde' & 'abc' don't exist.
  EXPECT_FALSE(automaton.find("abcde"sv));
  EXPECT_FALSE(automaton.find("abc"sv));

  // Add 'abc' with value 1.
  trie.add("abc"sv, 1);

  // Check 'abc' exists & value correct.
  EXPECT_TRUE(automaton.find("abc"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(1, payload);});

  // Add 'abcde' with value 2.
  trie.add("abcde"sv, 2);

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(2, payload);});

  // Check 'abc' exists & value correct.
  EXPECT_TRUE(automaton.find("abc"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(1, payload);});

  // Check 'ab' doesn't exist.
  EXPECT_FALSE(automaton.find("ab"sv));

  // Check 'abw' doesn't exist.
  EXPECT_FALSE(automaton.find("abw"sv));

  // Check 'abcdef' & 'abwxyz' don't exist.
  EXPECT_FALSE(automaton.find("abcd"sv));
  EXPECT_FALSE(automaton.find("abwxyz"sv));
}

TEST_F(TestFMRadixTrie, TrieAddThreeThirdOverlap_R_Value)
{
  // Add 'abcd', 'abfg'
  // Test add 'ab'
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcd', 'abef' & 'ab' don't exist.
  EXPECT_FALSE(automaton.find("abcd"sv));
  EXPECT_FALSE(automaton.find("abef"sv));
  EXPECT_FALSE(automaton.find("ab"sv));

  // Add 'abcd' with value 668.
  trie.add("abcd"sv, 668);

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Add 'abef' with value 777.
  trie.add("abef"sv, 777);

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});

  // Add 'ab' with value 888.
  trie.add("ab"sv, 888);

  // Check 'ab' exists & value correct.
  EXPECT_TRUE(automaton.find("ab"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(888, payload);});

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});
}

TEST_F(TestFMRadixTrie, TrieAddDuplicate_R_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  EXPECT_FALSE(automaton.find("1234"sv));

  // Add '1234' with value 668.
  trie.add("1234"sv, 668);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(668, payload);});

  // Add duplicate '1234' with value 777.
  trie.add("1234"sv, 777);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([](const auto & payload) { EXPECT_EQ(777, payload);});
}

TEST_F(TestFMRadixTrie, TrieAddToEmptyTrie_L_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  EXPECT_FALSE(automaton.find("1234"sv));

  // Add '1234' with value 668.
  auto value_668 = 668;
  trie.add("1234"sv, value_668);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check '123' doesn't exist.
  EXPECT_FALSE(automaton.find("123"sv));

  // Check '12345' doesn't exist.
  EXPECT_FALSE(automaton.find("12345"sv));
}

TEST_F(TestFMRadixTrie, TrieAddTwoWithNoInteresct_L_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' & 'abcd' don't exist.
  EXPECT_FALSE(automaton.find("1234"sv));
  EXPECT_FALSE(automaton.find("abcd"sv));

  // Add '1234' with value 668.
  auto value_668 = 668;
  trie.add("1234"sv, value_668);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add 'abcd' with value 777.
  auto value_777 = 777;
  trie.add("abcd"sv, value_777);

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Check '123' & 'abc' don't exist.
  EXPECT_FALSE(automaton.find("123"sv));
  EXPECT_FALSE(automaton.find("abc"sv));

  // Check '12345' & 'abcde' don't exist.
  EXPECT_FALSE(automaton.find("12345"sv));
  EXPECT_FALSE(automaton.find("abcde"sv));
}

TEST_F(TestFMRadixTrie, TrieAddTwoWithInteresct_L_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcde' & 'abxyz' don't exist.
  EXPECT_FALSE(automaton.find("abcde"sv));
  EXPECT_FALSE(automaton.find("abxyz"sv));

  // Add 'abcde' with value 668.
  auto value_668 = 668;

  trie.add("abcde"sv, value_668);

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add 'abwxy' with value 777.
  auto value_777 = 777;
  trie.add("abwxy"sv, value_777);

  // Check 'abwxy' exists & value correct.
  EXPECT_TRUE(automaton.find("abwxy"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check 'a' does exist.
  EXPECT_FALSE(automaton.find("a"sv));

  // Check 'ab' does exist.
  EXPECT_FALSE(automaton.find("ab"sv));

  // Check 'abc' & 'abw' don't exist.
  EXPECT_FALSE(automaton.find("abc"sv));
  EXPECT_FALSE(automaton.find("abw"sv));

  // Check 'abcdef' & 'abwxyz' don't exist.
  EXPECT_FALSE(automaton.find("abcdef"sv));
  EXPECT_FALSE(automaton.find("abwxyz"sv));
}

TEST_F(TestFMRadixTrie, TrieAddTwoFirstOverlap_L_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcde' & 'abc' don't exist.
  EXPECT_FALSE(automaton.find("abcde"sv));
  EXPECT_FALSE(automaton.find("abc"sv));

  // Add 'abcde' with value 668.
  auto value_668 = 668;
  trie.add("abcde"sv, value_668);

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add 'abc' with value 777.
  auto value_777 = 777;
  trie.add("abc"sv, value_777);

  // Check 'abc' exists & value correct.
  EXPECT_TRUE(automaton.find("abc"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check 'a' does exist.
  EXPECT_FALSE(automaton.find("a"sv));

  // Check 'ab' does exist.
  EXPECT_FALSE(automaton.find("ab"sv));

  // Check 'abw' doesn't exist.
  EXPECT_FALSE(automaton.find("abw"sv));

  // Check 'abcdef' & 'abwxyz' don't exist.
  EXPECT_FALSE(automaton.find("abcd"sv));
  EXPECT_FALSE(automaton.find("abwxyz"sv));
}

TEST_F(TestFMRadixTrie, TrieAddTwoSecondOverlap_L_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcde' & 'abc' don't exist.
  EXPECT_FALSE(automaton.find("abcde"sv));
  EXPECT_FALSE(automaton.find("abc"sv));

  // Add 'abc' with value 668.
  auto value_668 = 668;
  trie.add("abc"sv, value_668);

  // Check 'abc' exists & value correct.
  EXPECT_TRUE(automaton.find("abc"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add 'abcde' with value 777.
  auto value_777 = 777;
  trie.add("abcde"sv, value_777);

  // Check 'abcde' exists & value correct.
  EXPECT_TRUE(automaton.find("abcde"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Check 'abc' exists & value correct.
  EXPECT_TRUE(automaton.find("abc"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check 'ab' doesn't exist.
  EXPECT_FALSE(automaton.find("ab"sv));

  // Check 'abw' doesn't exist.
  EXPECT_FALSE(automaton.find("abw"sv));

  // Check 'abcdef' & 'abwxyz' don't exist.
  EXPECT_FALSE(automaton.find("abcd"sv));
  EXPECT_FALSE(automaton.find("abwxyz"sv));
}

TEST_F(TestFMRadixTrie, TrieAddThreeThirdOverlap_L_Value)
{
  // Add 'abcd', 'abfg'
  // Test add 'ab'
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check 'abcd', 'abef' & 'ab' don't exist.
  EXPECT_FALSE(automaton.find("abcd"sv));
  EXPECT_FALSE(automaton.find("abef"sv));
  EXPECT_FALSE(automaton.find("ab"sv));

  // Add 'abcd' with value 668.
  auto value_668 = 668;
  trie.add("abcd"sv, value_668);

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add 'abef' with value 777.
  auto value_777 = 777;
  trie.add("abef"sv, value_777);

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});

  // Add 'ab' with value 888.
  auto value_888 = 888;
  trie.add("ab"sv, value_888);

  // Check 'ab' exists & value correct.
  EXPECT_TRUE(automaton.find("ab"sv));
  automaton.visit([&value_888](const auto & payload) { EXPECT_EQ(value_888, payload);});

  // Check 'abcd' exists & value correct.
  EXPECT_TRUE(automaton.find("abcd"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Check 'abef' exists & value correct.
  EXPECT_TRUE(automaton.find("abef"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});
}

TEST_F(TestFMRadixTrie, TrieAddDuplicate_L_Value)
{
  radix_trie_type trie{};
  auto automaton = trie.create_automaton();

  // Check '1234' doesn't exist.
  EXPECT_FALSE(automaton.find("1234"sv));

  // Add '1234' with value 668.
  auto value_668 = 668;
  trie.add("1234"sv, value_668);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([&value_668](const auto & payload) { EXPECT_EQ(value_668, payload);});

  // Add duplicate '1234' with value 777.
  auto value_777 = 777;
  trie.add("1234"sv, value_777);

  // Check '1234' exists & value correct.
  EXPECT_TRUE(automaton.find("1234"sv));
  automaton.visit([&value_777](const auto & payload) { EXPECT_EQ(value_777, payload);});
}

} // namespace yafiyogi::yy_data::tests
