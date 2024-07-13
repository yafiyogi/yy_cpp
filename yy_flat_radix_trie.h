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

#include <algorithm>
#include <memory>
#include <vector>

#include "yy_assert.h"
#include "yy_span.h"
#include "yy_type_traits.h"
#include "yy_utility.h"

namespace yafiyogi::yy_data {
namespace flat_radix_trie_detail {

template<typename LabelElemType>
struct trie_node_edge;

template<typename LabelElemType>
class trie_node;

template<typename EdgesIterator>
struct found_value final
{
    EdgesIterator iter{};
    std::size_t common = 0;
    std::size_t remaining = 0;
};

template<typename LabelType>
struct trie_node_traits final
{
    using label_type = std::vector<yy_traits::remove_cvr_t<LabelType>>;
    using label_span_type = typename yy_quad::span_traits_helper<label_type>::const_span_type;
    using node_type = trie_node<LabelType>;
    using node_idx_type = std::size_t;
    using data_idx_type = std::size_t;
    using node_edge = trie_node_edge<LabelType>;
    using edges_type = std::vector<node_edge>;
    using edges_iterator = typename edges_type::iterator;
    using found_value_type = found_value<edges_iterator>;

    static constexpr node_idx_type root_idx{};
    static constexpr node_idx_type empty_idx = std::numeric_limits<node_idx_type>::max();
};

template<typename LabelType,
         typename ValueType>
struct trie_traits final
{
    using traits = trie_node_traits<LabelType>;
    using label_type = typename traits::label_type;
    using label_span_type = typename traits::label_span_type;
    using node_type = typename traits::node_type;
    using node_idx_type = typename traits::node_idx_type;
    using data_idx_type = typename traits::data_idx_type;
    using node_edge = typename traits::node_edge;
    using found_value_type = typename traits::found_value_type;

    using trie_container_type = std::vector<node_type>;
    using trie_ptr = std::shared_ptr<trie_container_type>;
    using value_type = ValueType;
    using data_container_type = std::vector<value_type>;
    using data_ptr = std::shared_ptr<data_container_type>;

    static constexpr node_idx_type root_idx = traits::root_idx;
    static constexpr node_idx_type empty_idx = traits::empty_idx;
};

template<typename LabelElemType>
struct trie_node_edge final
{
  public:
    using traits = trie_node_traits<LabelElemType>;
    using label_type = typename traits::label_type;
    using node_idx_type = typename traits::node_idx_type;

    constexpr explicit trie_node_edge(const label_type & p_label,
                                      node_idx_type p_idx) noexcept:
      m_label(p_label),
      m_idx(p_idx)
    {
    }

    constexpr explicit trie_node_edge(label_type && p_label,
                                      node_idx_type p_idx) noexcept:
      m_label(std::move(p_label)),
      m_idx(p_idx)
    {
    }

    constexpr bool operator==(const trie_node_edge & other) const noexcept
    {
      return m_label == other.m_label;
    }

    friend constexpr bool operator==(const typename label_type::value_type & a,
                                     const trie_node_edge & b) noexcept
    {
      return a == b.m_label[0];
    }

    friend constexpr bool operator==(const trie_node_edge & a,
                                     const typename label_type::value_type & b) noexcept
    {
      return a.m_label[0] == b;
    }

    constexpr bool operator<(const trie_node_edge & other) const noexcept
    {
      return m_label < other.m_label;
    }

    friend constexpr bool operator<(const typename label_type::value_type & a,
                                    const trie_node_edge & b) noexcept
    {
      return a < b.m_label[0];
    }

    friend constexpr bool operator<(const trie_node_edge & a,
                                    const typename label_type::value_type & b) noexcept
    {
      return a.m_label[0] < b;
    }

    trie_node_edge() = delete;
    constexpr trie_node_edge(const trie_node_edge & edge) noexcept = default;
    constexpr trie_node_edge(trie_node_edge && edge) noexcept = default;
    constexpr ~trie_node_edge() noexcept = default;

    constexpr trie_node_edge & operator=(const trie_node_edge & node) noexcept = default;
    constexpr trie_node_edge & operator=(trie_node_edge && node) noexcept = default;

    constexpr explicit operator bool() const noexcept
    {
      return traits::empty_idx != m_idx;
    }

    label_type m_label{};
    node_idx_type m_idx = traits::empty_idx;
};

template<typename LabelElemType>
class trie_node final
{
  public:
    using traits = trie_node_traits<LabelElemType>;
    using label_type = typename traits::label_type;
    using node_idx_type = typename traits::node_idx_type;
    using data_idx_type = typename traits::data_idx_type;
    using node_edge = typename traits::node_edge;
    using edges_type = std::vector<node_edge>;
    using edges_iterator = typename edges_type::iterator;
    using found_value_type = typename traits::found_value_type;

    static constexpr data_idx_type no_data = std::numeric_limits<data_idx_type>::max();

    constexpr explicit trie_node(const data_idx_type p_data_idx) noexcept:
      m_data_idx(p_data_idx)
    {
    }

    constexpr trie_node() noexcept = default;
    constexpr trie_node(const trie_node & node) noexcept = default;
    constexpr trie_node(trie_node && node) noexcept = default;
    constexpr ~trie_node() noexcept = default;

    constexpr trie_node & operator=(const trie_node & node) noexcept = default;
    constexpr trie_node & operator=(trie_node && node) noexcept = default;

    constexpr void add_edge(label_type && label,
                            node_idx_type node_idx)
    {
      auto iter = std::lower_bound(m_edges.begin(),
                                   m_edges.end(),
                                   label[0]);

      m_edges.emplace(iter, std::move(label), node_idx);
    }

    constexpr void add_edge(edges_iterator iter,
                            label_type && label,
                            node_idx_type node_idx)
    {
      m_edges.emplace(iter, std::move(label), node_idx);
    }

    template<typename InputLabelType>
    [[nodiscard]]
    constexpr found_value_type find(InputLabelType label_target) noexcept
    {
      static_assert(yy_traits::is_span_v<InputLabelType>,
                    "trie_node::find(): InputLabelType is not a yy_quad::span<>");

      auto edge_iter = std::lower_bound(m_edges.begin(),
                                        m_edges.end(),
                                        label_target[0]);

      if(m_edges.end() != edge_iter)
      {
        node_edge & edge = *edge_iter;
        auto label_edge = yy_quad::make_const_span(edge.m_label);

        auto [target_first, edge_first] = std::mismatch(label_target.begin(),
                                                        label_target.end(),
                                                        label_edge.begin(),
                                                        label_edge.end());

        const auto common_size = static_cast<std::size_t>(std::distance(label_target.begin(), target_first));

        return found_value_type{std::move(edge_iter),
                                common_size,
                                label_edge.size() - common_size};
      }

      return found_value_type{m_edges.end(), 0, label_target.size()};
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor)
    {
      for(auto & [label, node_idx]: m_edges)
      {
        visitor(label, node_idx);
      }
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return no_data == m_data_idx;;
    }

    constexpr void data_idx(data_idx_type p_data_idx) noexcept
    {
      m_data_idx = p_data_idx;
    }

    [[nodiscard]]
    constexpr data_idx_type data_idx() const noexcept
    {
      return m_data_idx;
    }

  private:
    edges_type m_edges{};
    data_idx_type m_data_idx = no_data;
};

template<typename LabelElemType,
         typename ValueType>
class Automaton final
{
  public:
    using traits = trie_traits<LabelElemType, ValueType>;
    using label_type = typename traits::label_type;
    using node_type = typename traits::node_type;
    using value_type = typename traits::value_type;
    using node_idx_type = typename traits::node_idx_type;
    using data_idx_type = typename traits::data_idx_type;
    using node_edge = typename traits::node_edge;
    using trie_ptr = typename traits::trie_ptr;
    using trie_container_type = typename traits::trie_container_type;
    using data_ptr = typename traits::data_ptr;
    using data_container_type = typename traits::data_container_type;

    constexpr explicit Automaton(const trie_ptr p_nodes,
                                 const data_ptr p_data) noexcept:
      m_nodes(std::move(p_nodes)),
      m_data(std::move(p_data))
    {
      reset();
    }

    constexpr Automaton() noexcept = default;
    Automaton(const Automaton &) = delete;
    constexpr Automaton(Automaton &&) noexcept = default;
    constexpr ~Automaton() noexcept = default;

    Automaton & operator=(const Automaton & other) = delete;
    constexpr Automaton & operator=(Automaton && other) noexcept = default;

    template<typename InputSpanType>
    [[nodiscard]]
    constexpr bool find(InputSpanType && label)
    {
      return find_span(yy_quad::make_const_span(std::forward<InputSpanType>(label)));
    }

    constexpr void reset() noexcept
    {
      m_state = traits::root_idx;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return traits::empty_idx == m_state;
    }

    [[nodiscard]]
    constexpr bool has_payload() const noexcept
    {
      return !empty() && !get_node(m_state)->empty();
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) const
    {
      if(has_payload())
      {
        auto & data = *get_data(get_node(m_state)->data_idx());

        visitor(data);
      }
    }

  private:
    [[nodiscard]]
    constexpr node_type * get_node(const node_idx_type idx) noexcept
    {
      return get_node(m_nodes->data(), idx);
    }

    [[nodiscard]]
    constexpr const node_type * get_node(const node_idx_type idx) const noexcept
    {
      return get_node(m_nodes->data(), idx);
    }

    [[nodiscard]]
    static constexpr node_type * get_node(node_type * nodes,
                                          const node_idx_type idx) noexcept
    {
      return nodes + idx;
    }

    [[nodiscard]]
    static constexpr const node_type * get_node(const node_type * nodes,
                                                const node_idx_type idx) noexcept
    {
      return nodes + idx;
    }

    [[nodiscard]]
    static value_type * get_data(value_type * data, data_idx_type idx) noexcept
    {
      return data + idx;
    }

    [[nodiscard]]
    constexpr value_type * get_data(data_idx_type idx) noexcept
    {
      return get_data(m_data->data(), idx);
    }

    [[nodiscard]]
    static constexpr const value_type * get_data(const value_type * data, data_idx_type idx) noexcept
    {
      return data + idx;
    }

    [[nodiscard]]
    constexpr const value_type * get_data(data_idx_type idx) const noexcept
    {
      return get_data(m_data->data(), idx);
    }

    template<typename InputSpanType>
    [[nodiscard]]
    constexpr bool find_span(InputSpanType label_target) noexcept
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "Automaton::find_span(): InputScanType is not a yy_quad::span<>");
      reset();

      auto node_idx = m_state;
      auto nodes = m_nodes->data();

      while(!label_target.empty())
      {
        auto [edge_iter, common, remaining] = get_node(nodes, node_idx)->find(label_target);

        if((0 == common) || ((label_target.size() == common) && (remaining != 0)))
        {
          // Not found an edge...
          // ...or target is 'abcd' and found 'abcd', but edge is 'abcde'
          m_state = traits::empty_idx;
          return false;
        }

        node_idx = edge_iter->m_idx;
        label_target = label_target.subspan(common);
      }

      m_state = node_idx;
      return has_payload();
    }

    trie_ptr m_nodes{};
    data_ptr m_data{};
    node_idx_type m_state = traits::empty_idx;
};

} // namespace detail

template<typename LabelElemType,
         typename ValueType,
         typename Automaton = flat_radix_trie_detail::Automaton<LabelElemType, ValueType>>
class flat_radix_trie
{
  public:
    using traits = typename flat_radix_trie_detail::trie_traits<LabelElemType, ValueType>;
    using label_type = typename traits::label_type;
    using label_span_type = typename traits::label_span_type;
    using node_type = typename traits::node_type;
    using node_idx_type = typename traits::node_idx_type;
    using data_idx_type = typename traits::data_idx_type;
    using node_edge = typename traits::node_edge;
    using value_type = typename traits::value_type;
    using automaton = Automaton;
    using trie_ptr = typename traits::trie_ptr;
    using trie_container_type = typename traits::trie_container_type;
    using data_ptr = typename traits::data_ptr;
    using data_container_type = typename traits::data_container_type;

    constexpr flat_radix_trie() noexcept:
      m_nodes(std::make_shared<trie_container_type>(1, node_type{node_type::no_data})), // add root node
      m_data(std::make_shared<data_container_type>())
    {
    }

    flat_radix_trie(const flat_radix_trie &) = delete;
    constexpr flat_radix_trie(flat_radix_trie &&) noexcept = default;
    constexpr ~flat_radix_trie() noexcept = default;

    flat_radix_trie & operator=(const flat_radix_trie &) = delete;
    constexpr flat_radix_trie & operator=(flat_radix_trie &&) noexcept = default;

    template<typename InputSpanType,
             typename InputValueType>
    constexpr void add(InputSpanType && label,
                       InputValueType && value)
    {
      static_assert(std::is_same_v<yy_traits::remove_cvr_t<InputValueType>,
                    yy_traits::remove_cvr_t<value_type>>,
                    "The value provided is not the correct type.");
      add_span(yy_quad::make_const_span(std::forward<InputSpanType>(label)), std::forward<InputValueType>(value));
    }

    [[nodiscard]]
    constexpr automaton create_automaton() noexcept
    {
      return automaton{m_nodes, m_data};
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor)
    {
      get_node(traits::root_idx)->visit(visitor);
    }

  private:
    [[nodiscard]]
    constexpr node_type * get_node(const node_idx_type idx) noexcept
    {
      return get_node(m_nodes->data(), idx);
    }

    [[nodiscard]]
    static constexpr node_type * get_node(node_type * nodes,
                                          const node_idx_type idx) noexcept
    {
      return nodes + idx;
    }

    template<typename InputValueType>
    [[nodiscard]]
    static constexpr data_idx_type add_data(data_container_type & data,
                                            InputValueType && value)
    {
      auto data_idx = data.size();

      data.emplace_back(std::forward<InputValueType>(value));

      return data_idx;
    }

    [[nodiscard]]
    static constexpr node_idx_type add_node(trie_container_type & nodes,
                                            data_idx_type data_idx)
    {
      auto node_idx = nodes.size();

      nodes.emplace_back(data_idx);

      return node_idx;
    }

    template<typename InputSpanType,
             typename InputValueType>
    constexpr void add_span(InputSpanType label,
                            InputValueType && value)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "flat_radix_trie::add_span(): InputSpanType is not a yy_quad::span<>");

      auto nodes = m_nodes->data();
      auto & data = *m_data;
      node_idx_type node_idx{traits::root_idx};

      while(!label.empty())
      {
        auto [edge_iter, common, remaining] = get_node(nodes, node_idx)->find(label);
        auto & edge = *edge_iter;

        if((0 == remaining) && (label.size() != common) && (0 != common))
        {
          // Searching for 'abcd' found 'ab'.
          // Go round again.
          label = label.subspan(common);
          node_idx = edge.m_idx;
        }
        else
        {
          if(0 == common)
          {
            // * Searching for 'abcd', but found nothing.
            auto abcd_data_idx = add_data(data, std::forward<InputValueType>(value));
            auto abcd_node_idx = add_node(*m_nodes, abcd_data_idx);
            get_node(node_idx)->add_edge(edge_iter,
                                         label_type{label.begin(), label.end()},
                                         abcd_node_idx);

            // Added everything!
            return;
          }

          if((0 != remaining) && (label.size() != common))
          {
            YY_ASSERT(0 != common);
            // * Searching for 'abef' found 'abcd'.
            // * Create new node for 'cd'
            auto & abcd_label = edge.m_label;
            auto abcd_idx = edge.m_idx;

            label_span_type cd_span(yy_quad::make_const_span(abcd_label).subspan(common));
            label_type cd_label{cd_span.begin(), cd_span.end()};
            data_idx_type cd_data_idx = get_node(abcd_idx)->data_idx();

            // Repurpose 'abcd' edge and node for 'ab'.
            label_span_type ab_span{yy_quad::make_const_span(abcd_label).subspan(0, common)};
            edge.m_label = label_type{ab_span.begin(), ab_span.end()};
            *get_node(abcd_idx) = node_type{node_type::no_data};

            // Add 'cd' node to 'ab' node.
            // N.B this invatidates edge_iter & edge!
            auto cd_node_idx = add_node(*m_nodes, cd_data_idx);
            get_node(abcd_idx)->add_edge(std::move(cd_label),
                                         cd_node_idx);

            // Create new node for 'ef'
            data_idx_type ef_data_idx = add_data(data, std::forward<InputValueType>(value));
            node_idx_type ef_node_idx = add_node(*m_nodes, ef_data_idx);
            auto ef_span{label.subspan(common)};

            get_node(abcd_idx)->add_edge(label_type{ef_span.begin(), ef_span.end()},
                                         ef_node_idx);

            // Added everything!
            return;
          }

          if((0 == remaining) && (label.size() == common))
          {
            YY_ASSERT(0 != common);

            // Searching for 'abcd', found 'abcd'.
            auto & edge_node = *get_node(edge.m_idx);

            if(!edge_node.empty())
            {
              // Overwrite value.
              data[edge_node.data_idx()] = std::forward<InputValueType>(value);

              return;
            }

            // Node has no data.
            // Update data idx & add data.
            auto data_idx = add_data(data, std::forward<InputValueType>(value));
            edge_node.data_idx(data_idx);

            // Added everything!
            return;
          }

          YY_ASSERT(0 != common);

          // * Searching for 'ab' found 'abcd'.
          // * Create 'cd' edge and add to 'ab' node.
          // * Use 'abcd' node for 'ab' data.
          // * Add new node for 'cd' node.
          label_span_type cd_span{yy_quad::make_const_span(edge.m_label).subspan(common)};
          label_type cd_label{cd_span.begin(), cd_span.end()};
          node_idx_type abcd_node_idx = edge.m_idx;
          data_idx_type cd_data_idx = get_node(abcd_node_idx)->data_idx();

          // * Add 'ab' data.
          // * Update 'abcd edge to be 'ab'
          auto ab_span{label.subspan(0, common)};
          auto ab_data_idx = add_data(data, std::forward<InputValueType>(value));
          edge.m_label = label_type{ab_span.begin(), ab_span.end()};
          *get_node(abcd_node_idx) = node_type{ab_data_idx};

          node_idx_type cd_node_idx = add_node(*m_nodes, cd_data_idx);
          get_node(abcd_node_idx)->add_edge(std::move(cd_label),
                                            cd_node_idx);

          // Added everything!
          return;
        }
      }
    }

    trie_ptr m_nodes{};
    data_ptr m_data{};
};

} // namespace yafiyogi::yy_data
