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

#include "yy_trie_common.h"
#include "yy_type_traits.h"
#include "yy_flat_map.h"

namespace yafiyogi::yy_data::trie_node_idx_detail {

template<typename LabelType,
         typename ValueType>
class trie_node_idx;

template<typename LabelType,
         typename ValueType>
struct trie_node_idx_traits final
{
    using label_traits = yy_trie::label_traits<LabelType>;
    using value_traits = yy_trie::value_traits<ValueType>;

    using label_type = typename label_traits::label_type;
    using label_l_value_ref = typename label_traits::label_l_value_ref;
    using label_r_value_ref = typename label_traits::label_r_value_ref;

    using value_type = typename value_traits::value_type;
    using value_ptr = typename value_traits::value_ptr;

    using node_type = trie_node_idx<LabelType, ValueType>;
    using node_ptr = std::add_pointer_t<node_type>;
    using const_node_ptr = std::add_pointer_t<std::add_const_t<node_type>>;
    using node_idx_type = size_type;
    using node_idx_ptr = std::add_pointer_t<node_idx_type>;
    using data_idx_type = size_type;
    using data_idx_ptr = std::add_pointer_t<data_idx_type>;
};

template<typename LabelType,
         typename ValueType>
class trie_node_idx final
{
  public:
    using traits = trie_node_idx_traits<LabelType, ValueType>;

    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;

    using value_type = typename traits::value_type;
    using value_ptr = typename traits::value_ptr;
    using data_idx_type = typename traits::data_idx_type;
    using data_idx_ptr = typename traits::data_idx_ptr;

    using node_idx_type = typename traits::node_idx_type;
    using node_idx_ptr = typename traits::node_idx_ptr;
    using node_type = typename traits::node_type;

    using edges_type = flat_map<label_type, node_idx_type>;
    using value_pos_type = edges_type::value_pos_type;
    using const_value_pos_type = edges_type::const_value_pos_type;

    static constexpr node_idx_type root_idx = 0;
    static constexpr node_idx_type empty_idx = end_idx;

    constexpr explicit trie_node_idx(data_idx_type p_data) noexcept:
      m_data(p_data)
    {
    }

    constexpr trie_node_idx() noexcept = default;
    constexpr trie_node_idx(const trie_node_idx & node) noexcept = default;
    constexpr trie_node_idx(trie_node_idx && node) noexcept = default;
    constexpr ~trie_node_idx() noexcept = default;

    constexpr trie_node_idx & operator=(const trie_node_idx & node) noexcept = default;
    constexpr trie_node_idx & operator=(trie_node_idx && node) noexcept = default;

    constexpr void add_edge(size_type pos,
                            label_r_value_ref label,
                            const node_idx_type node)
    {
      std::ignore = m_edges.emplace(pos,
                                    std::forward<label_type>(label),
                                    node);
    }

    constexpr void add_edge(label_r_value_ref label,
                            const node_idx_type node)
    {
      size_type pos = m_edges.lower_bound_pos(label).pos;

      add_edge(pos,
               std::forward<label_type>(label),
               node);
    }

    template<typename InputLabelType>
    [[nodiscard]]
    constexpr value_pos_type find_edge(const InputLabelType & p_label) noexcept
    {
      return m_edges.find(p_label);
    }

    template<typename InputLabelType>
    [[nodiscard]]
    constexpr const_value_pos_type find_edge(const InputLabelType & p_label) const noexcept
    {
      return m_edges.find(p_label);
    }

    using pos_found_type = yy_data::pos_found_type;

    template<typename InputLabelType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const InputLabelType & p_label) noexcept
    {
      return m_edges.find_value(std::forward<Visitor>(visitor), p_label);
    }

    template<typename InputLabelType,
             typename Visitor>
    [[nodiscard]]
    constexpr pos_found_type find_value(Visitor && visitor,
                                        const InputLabelType & p_label) const noexcept
    {
      return m_edges.find_value(std::forward<Visitor>(visitor), p_label);
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) noexcept
    {
      m_edges.visit(visitor);
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) const noexcept
    {
      m_edges.visit(visitor);
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return no_data == m_data;
    }

    constexpr size_type edges() const noexcept
    {
      return m_edges.size();
    }

    constexpr void data(const data_idx_type p_data) noexcept
    {
      m_data = p_data;
    }

    [[nodiscard]]
    constexpr data_idx_type data() const noexcept
    {
      return m_data;
    }

  private:
    data_idx_type m_data = no_data;
    edges_type m_edges{};
};

} // yafiyogi::yy_data::trie_node_idx_detail
