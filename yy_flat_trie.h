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

#include <limits>
#include <memory>
#include <vector>

#include "yy_span.h"
#include "yy_utility.h"
#include "yy_vector_util.h"

#include "yy_type_traits.h"

namespace yafiyogi::yy_data {
namespace flat_trie_detail {

template<typename LabelType>
struct trie_node_edge;

template<typename LabelType>
class trie_node;

template<typename EdgesIterator>
struct found_value final
{
    EdgesIterator iter{};
    bool found = false;
};

template<typename LabelType>
struct trie_node_traits final
{
    using label_type = yy_traits::remove_rcv_t<LabelType>;
    using label_l_value_ref = typename yy_traits::ref_traits<label_type>::l_value_ref;
    using label_r_value_ref = typename yy_traits::ref_traits<label_type>::r_value_ref;
    using node_type = trie_node<label_type>;
    using node_idx_type = std::size_t;
    using data_idx_type = std::size_t;
    using node_edge = trie_node_edge<label_type>;
    using edges_type = std::vector<node_edge>;
    using edges_iterator = typename edges_type::iterator;
    using found_value_type = found_value<edges_iterator>;

    static inline constexpr node_idx_type root_idx{};
    static inline constexpr node_idx_type empty_idx = std::numeric_limits<node_idx_type>::max();
};

template<typename LabelType,
         typename ValueType>
struct trie_traits final
{
    using traits = trie_node_traits<LabelType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
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

    static inline constexpr node_idx_type root_idx = traits::root_idx;
    static inline constexpr node_idx_type empty_idx = traits::empty_idx;
};

template<typename LabelType>
struct trie_node_edge final
{
  public:
    using traits = trie_node_traits<LabelType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_idx_type = typename traits::node_idx_type;

    constexpr explicit trie_node_edge(label_type p_label,
                                      node_idx_type p_idx) noexcept:
      m_label(std::move(p_label)),
      m_idx(p_idx)
    {
    }

    constexpr trie_node_edge() noexcept = default;
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

template<typename LabelType>
constexpr bool operator==(const typename trie_node_edge<LabelType>::label_type & lhs,
                          const trie_node_edge<LabelType> & rhs) noexcept
{
  return lhs == rhs.m_label;
}

template<typename LabelType>
constexpr bool operator<(const typename trie_node_edge<LabelType>::label_type & lhs,
                         const trie_node_edge<LabelType> & rhs) noexcept
{
  return lhs < rhs.m_label;
}

template<typename LabelType>
class trie_node final
{
  public:
    using traits = trie_node_traits<LabelType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_idx_type = typename traits::node_idx_type;
    using data_idx_type = typename traits::data_idx_type;
    using node_edge = typename traits::node_edge;
    using found_value_type = typename traits::found_value_type;
    using edges_type = typename traits::edges_type;
    using edges_iterator = typename traits::edges_iterator;

    static inline constexpr data_idx_type no_data = std::numeric_limits<data_idx_type>::max();

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

    constexpr void add_edge(edges_iterator iter,
                            node_edge edge)
    {
      m_edges.emplace(iter, std::move(edge));
    }

    [[nodiscard]]
    constexpr found_value_type find(const label_type & label) noexcept
    {
      auto [iter, found] = yy_util::find(m_edges, label);

      return found_value_type{std::move(iter), found};
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor)
    {
      for(auto & [label, idx]: m_edges)
      {
        visitor(label, idx);
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
    edges_type m_edges;
    data_idx_type m_data_idx = no_data;
};

template<typename LabelType,
         typename ValueType>
class Automaton final
{
  public:
    using traits = trie_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_idx_type = typename traits::node_idx_type;
    using data_idx_type = typename traits::data_idx_type;
    using node_edge = typename traits::node_edge;
    using value_type = typename traits::value_type;
    using trie_ptr = typename traits::trie_ptr;
    using trie_container_type = typename traits::trie_container_type;
    using data_ptr = typename traits::data_ptr;
    using data_container_type = typename traits::data_container_type;

    constexpr explicit Automaton(const trie_ptr p_nodes,
                                 const data_ptr p_data) noexcept:
      m_nodes(std::move(p_nodes)),
      m_data(std::move(p_data)),
      m_state()
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
        visitor(*get_data(get_node(m_state)->data_idx()));
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
    static constexpr node_type * get_node(node_type * raw_nodes,
                                          const node_idx_type idx) noexcept
    {
      return raw_nodes + idx;
    }

    [[nodiscard]]
    static constexpr const node_type * get_node(const node_type * raw_nodes,
                                                const node_idx_type idx) noexcept
    {
      return raw_nodes + idx;
    }

    [[nodiscard]]
    static constexpr value_type * get_data(value_type * raw_data,
                                           const data_idx_type idx) noexcept
    {
      return raw_data + idx;
    }

    [[nodiscard]]
    static constexpr const value_type * get_data(const value_type * raw_data,
                                                 const data_idx_type idx) noexcept
    {
      return raw_data + idx;
    }

    [[nodiscard]]
    constexpr value_type * get_data(const data_idx_type idx) noexcept
    {
      return get_data(m_data->data(), idx);
    }

    [[nodiscard]]
    constexpr const value_type * get_data(const data_idx_type idx) const noexcept
    {
      return get_data(m_data->data(), idx);
    }

    template<typename InputSpanType>
    [[nodiscard]]
    constexpr bool find_span(const InputSpanType label) noexcept
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "flat_trie::find_span(): InputSpanType is not a yy_quad::span<>");

      reset();

      auto node_idx = m_state;
      auto raw_nodes = m_nodes->data();

      for(const label_l_value_ref ch : label)
      {
        auto [edge_iter, found] = get_node(raw_nodes, node_idx)->find(ch);

        if(!found)
        {
          m_state = traits::empty_idx;
          return false;
        }

        node_idx = edge_iter->m_idx;
      }

      m_state = node_idx;

      return has_payload();
    }

    trie_ptr m_nodes;
    data_ptr m_data;
    node_idx_type m_state = traits::empty_idx;
};

} // namespace detail

template<typename LabelType,
         typename ValueType,
         typename Automaton = flat_trie_detail::Automaton<LabelType, ValueType>>
class flat_trie
{
  public:
    using traits = typename flat_trie_detail::trie_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_idx_type = typename traits::node_idx_type;
    using data_idx_type = typename traits::data_idx_type;
    using node_edge = typename traits::node_edge;
    using edges_iterator = typename node_type::edges_iterator;
    using value_type = typename traits::value_type;
    using automaton = Automaton;
    using trie_ptr = typename traits::trie_ptr;
    using trie_container_type = typename traits::trie_container_type;
    using data_ptr = typename traits::data_ptr;
    using data_container_type = typename traits::data_container_type;

    constexpr flat_trie() noexcept:
      m_nodes(std::make_shared<trie_container_type>(1, node_type{node_type::no_data})), // add root node
      m_data(std::make_shared<data_container_type>())
    {
    }

    flat_trie(const flat_trie &) = delete;
    constexpr flat_trie(flat_trie &&) noexcept = default;
    constexpr ~flat_trie() noexcept = default;

    flat_trie & operator=(const flat_trie &) = delete;
    constexpr flat_trie & operator=(flat_trie &&) noexcept = default;

    template<typename InputSpanType,
             typename InputValueType>
    constexpr void add(InputSpanType && label,
                       InputValueType && value)
    {
      static_assert(std::is_same_v<yy_traits::remove_rcv_t<InputValueType>,
                    yy_traits::remove_rcv_t<value_type>>,
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
    static constexpr node_type * get_node(node_type * raw_nodes,
                                          const node_idx_type idx) noexcept
    {
      return raw_nodes + idx;
    }

    [[nodiscard]]
    static constexpr value_type * get_data(value_type * raw_data,
                                           const data_idx_type idx) noexcept
    {
      return raw_data + idx;
    }

    [[nodiscard]]
    constexpr value_type * get_data(const data_idx_type idx) noexcept
    {
      return get_data(m_data->data(), idx);
    }

    template<typename InternalValueType>
    [[nodiscard]]
    constexpr data_idx_type add_data(InternalValueType && value)
    {
      auto & data = *m_data;

      data_idx_type data_idx = data.size();

      data.emplace_back(std::forward<InternalValueType>(value));

      return data_idx;
    }

    constexpr node_idx_type add_node(node_type * node,
                                     edges_iterator edge_iter,
                                     const label_type & label,
                                     const data_idx_type data_idx)
    {
      auto & nodes = *m_nodes;
      node_idx_type node_idx{static_cast<node_idx_type>(nodes.size())};
      node->add_edge(edge_iter, node_edge{label, node_idx});

      nodes.emplace_back(node_type{data_idx});

      return node_idx;
    }

    template<typename InputSpanType>
    [[nodiscard]]
    constexpr node_idx_type add_empty_nodes(trie_container_type & nodes,
                                            InputSpanType label)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "fm_flat_trie::find_span(): InputSpanType is not a yy_quad::span<>");

      node_idx_type node_idx{traits::root_idx};

      label.dec_end();

      auto raw_nodes = nodes.data();
      // Skip exising nodes.
      while(!label.empty())
      {
        auto node = get_node(raw_nodes, node_idx);
        auto [edge_iter, found] = node->find(label[0]);

        if(!found)
        {
          break;
        }

        label.inc_begin();
        node_idx = edge_iter->m_idx;
      }

      // Add new nodes;
      for(auto & label_part : label)
      {
        auto node = get_node(node_idx);
        auto [edge_iter, found] = node->find(label_part);

        node_idx = add_node(node, edge_iter, label_part, node_type::no_data);
      }

      return node_idx;
    }

    template<typename InputSpanType,
             typename InputValueType>
    constexpr void add_span(InputSpanType label,
                            InputValueType && value)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "flat_trie::add_span(): InputSpanType is not a yy_quad::span<>");

      if(!label.empty())
      {
        node_idx_type node_idx = add_empty_nodes(*m_nodes, label);
        auto node = get_node(node_idx);
        auto [edge_iter, found] = node->find(label.back());

        if(found)
        {
          // Overwrite an existing node.
          auto & edge_node = *get_node(edge_iter->m_idx);
          if(!edge_node.empty())
          {
            // Data node exists.
            // Overwrite existing value.
            auto * data_value = get_data(edge_node.data_idx());

            *data_value = std::forward<InputValueType>(value);
            return;
          }

          // Add new data item, & update node data idx
          auto data_idx = add_data(std::forward<InputValueType>(value));
          edge_node.data_idx(data_idx);

          return;
        }

        // No data node exists.
        // Add data node.
        auto data_idx = add_data(std::forward<InputValueType>(value));
        add_node(node, edge_iter, label.back(), data_idx);
      }
    }

    trie_ptr m_nodes;
    data_ptr m_data;
};

} // namespace yafiyogi::yy_data
