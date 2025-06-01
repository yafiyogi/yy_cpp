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

#pragma once

#include "yy_find_raw_util.hpp"

namespace yafiyogi::yy_data::trie_node_ptr_detail {

template<typename LabelType,
         typename ValueType>
class trie_node_ptr;

template<typename LabelType,
         typename ValueType>
struct trie_node_ptr_traits final
{
    using label_traits = yy_trie::label_traits<LabelType>;
    using value_traits = yy_trie::value_traits<ValueType>;

    using label_type = typename label_traits::label_type;
    using label_l_value_ref = typename label_traits::label_l_value_ref;
    using label_r_value_ref = typename label_traits::label_r_value_ref;

    using value_type = typename value_traits::value_type;
    using value_ptr = typename value_traits::value_ptr;
    using const_value_ptr = typename value_traits::const_value_ptr;

    using node_type = trie_node_ptr<LabelType, ValueType>;
    using node_ptr = std::add_pointer_t<node_type>;
    using const_node_ptr = std::add_pointer_t<std::add_const_t<node_type>>;

    using edge_ptr = std::add_pointer_t<node_ptr>;
};

template<typename LabelType,
         typename ValueType>
class trie_node_ptr final
{
  public:
    using traits = trie_node_ptr_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;

    using value_type = typename traits::value_type;
    using value_ptr = typename traits::value_ptr;
    using const_value_ptr = typename traits::const_value_ptr;

    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using const_node_ptr = typename traits::const_node_ptr;
    using edge_ptr = typename traits::edge_ptr;

    constexpr explicit trie_node_ptr(value_ptr p_data) noexcept:
      m_data(p_data)
    {
    }

    constexpr trie_node_ptr() noexcept = default;
    constexpr trie_node_ptr(const trie_node_ptr & node) noexcept = default;
    constexpr trie_node_ptr(trie_node_ptr && node) noexcept = default;
    constexpr ~trie_node_ptr() noexcept = default;

    constexpr trie_node_ptr & operator=(const trie_node_ptr & node) noexcept = default;
    constexpr trie_node_ptr & operator=(trie_node_ptr && node) noexcept = default;

    constexpr void add_edge(edge_ptr p_edge,
                            label_r_value_ref p_label,
                            node_ptr p_node_ptr)
    {
      auto pos = p_edge - m_edges.data();

      std::ignore = m_labels.emplace(m_labels.begin() + pos, std::move(p_label));
      std::ignore = m_edges.emplace(m_edges.begin() + pos, p_node_ptr);
    }

    constexpr void add_edge(label_r_value_ref p_label,
                            node_ptr p_node_ptr)
    {
      add_edge(m_edges.data() + m_edges.size(),
               std::move(p_label),
               p_node_ptr);
    }

    struct found_edge_type final
    {
        edge_ptr edge = nullptr;
        bool found = false;
    };

    template<typename InputLabelType>
    [[nodiscard]]
    constexpr found_edge_type find_edge(const InputLabelType & p_label) noexcept
    {
      auto [label, found] = find_raw(m_labels, p_label);

      auto pos = label - m_labels.data();

      return found_edge_type{m_edges.data() + pos, found};
    }

    struct const_found_edge_type final
    {
        const_node_ptr edge = nullptr;
        bool found = false;
    };

    template<typename InputLabelType>
    [[nodiscard]]
    constexpr const_found_edge_type find_edge(const InputLabelType & p_label) const noexcept
    {
      auto [label, found] = find_raw(m_labels, p_label);

      auto pos = label - m_labels.data();

      return const_found_edge_type{m_edges.data() + pos, found};
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) noexcept
    {
      auto edge = m_edges.data();
      for(auto & label : m_labels)
      {
        visitor(label, *edge);
        ++edge;
      }
    }

    constexpr void reserve(size_type capacity) noexcept
    {
      m_labels.reserve(capacity);
      m_edges.reserve(capacity);
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return nullptr == m_data;
    }

    constexpr void data(value_ptr p_data) noexcept
    {
      m_data = p_data;
    }

    [[nodiscard]]
    constexpr value_ptr data() noexcept
    {
      return m_data;
    }

    [[nodiscard]]
    constexpr const_value_ptr data() const noexcept
    {
      return const_value_ptr{m_data};
    }

  private:
    value_ptr m_data{};
    yy_quad::simple_vector<label_type> m_labels;
    yy_quad::simple_vector<node_ptr> m_edges;
};

} // yafiyogi::yy_data::trie_node_idx_detail::trie_node_ptr_detail
