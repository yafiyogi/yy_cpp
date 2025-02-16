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

#include <cstddef>

#include "yy_assert.h"
#include "yy_constants.hpp"
#include "yy_span.h"
#include "yy_flat_map.h"
#include "yy_ref_traits.h"
#include "yy_trie_common.h"
#include "yy_type_traits.h"
#include "yy_types.hpp"
#include "yy_vector.h"

namespace yafiyogi::yy_data {
namespace fm_flat_trie_ptr_detail {

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
    using label_const_l_value_ref = typename label_traits::label_const_l_value_ref;
    using label_r_value_ref = typename label_traits::label_r_value_ref;

    using value_type = typename value_traits::value_type;
    using value_ptr = typename value_traits::value_ptr;
    using const_value_ptr = typename value_traits::const_value_ptr;
    using value_idx_type = std::size_t;

    using node_type = trie_node_idx<LabelType, ValueType>;
    using node_ptr = std::add_pointer_t<node_type>;
    using const_node_ptr = std::add_pointer_t<std::add_const_t<node_type>>;
    using node_idx_type = std::size_t;
    using edges_type = flat_map<label_type, node_idx_type>;
};

template<typename LabelType,
         typename ValueType>
class trie_node_idx final
{
  public:
    using traits = trie_node_idx_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_const_l_value_ref = typename traits::label_const_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;

    using value_type = typename traits::value_type;
    using value_idx_type = typename traits::value_idx_type;

    using node_idx_type = typename traits::node_idx_type;
    using node_type = typename traits::node_type;
    using edges_type = typename traits::edges_type;

    static constexpr node_idx_type root_idx = 0;
    static constexpr node_idx_type empty_idx = end_idx;

    constexpr explicit trie_node_idx(const value_idx_type p_data) noexcept:
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
                            node_idx_type node_idx)
    {
      std::ignore = m_edges.emplace(pos, std::move(label), node_idx);
    }

    using pos_found_type = yy_data::pos_found_type;

    template<typename Visitor,
             typename InputLabelType>
    [[nodiscard]]
    constexpr pos_found_type find_edge(Visitor && visitor,
                                       const InputLabelType & label) noexcept
    {
      return m_edges.find_value(std::forward<Visitor>(visitor), label);
    }

    template<typename InputLabelType>
    [[nodiscard]]
    constexpr pos_found_type find_edge_pos(const InputLabelType & label) const noexcept
    {
      return m_edges.find_pos(label);
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) noexcept
    {
      m_edges.visit(std::forward<Visitor>(visitor));
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

    constexpr void data(const value_idx_type p_data) noexcept
    {
      m_data = p_data;
    }

    [[nodiscard]]
    constexpr value_idx_type data() const noexcept
    {
      return m_data;
    }

  private:
    value_idx_type m_data = no_data;
    edges_type m_edges{};
};

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
    using label_const_l_value_ref = typename label_traits::label_const_l_value_ref;
    using label_r_value_ref = typename label_traits::label_r_value_ref;

    using value_type = typename value_traits::value_type;
    using value_ptr = typename value_traits::value_ptr;
    using const_value_ptr = typename value_traits::const_value_ptr;

    using node_type = trie_node_ptr<LabelType, ValueType>;
    using node_ptr = std::add_pointer_t<node_type>;
    using const_node_ptr = std::add_pointer_t<std::add_const_t<node_type>>;
    using edges_type = flat_map<label_type, node_ptr>;
};

template<typename LabelType,
         typename ValueType>
class trie_node_ptr final
{
  public:
    using traits = trie_node_ptr_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_const_l_value_ref = typename traits::label_const_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;

    using value_type = typename traits::value_type;
    using value_ptr = typename traits::value_ptr;
    using const_value_ptr = typename traits::const_value_ptr;

    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using const_node_ptr = typename traits::const_node_ptr;
    using edges_type = typename traits::edges_type;

    constexpr explicit trie_node_ptr(const_value_ptr p_data) noexcept:
      m_data(p_data)
    {
    }

    constexpr trie_node_ptr() noexcept = default;
    constexpr trie_node_ptr(const trie_node_ptr & node) noexcept = default;
    constexpr trie_node_ptr(trie_node_ptr && node) noexcept = default;
    constexpr ~trie_node_ptr() noexcept = default;

    constexpr trie_node_ptr & operator=(const trie_node_ptr & node) noexcept = default;
    constexpr trie_node_ptr & operator=(trie_node_ptr && node) noexcept = default;

    constexpr void add_edge(label_r_value_ref label,
                            node_ptr node)
    {
      std::ignore = m_edges.emplace(m_edges.size(), std::move(label), node);
    }

    struct found_edge_type final
    {
        node_ptr edge_node = nullptr;
        bool found = false;
    };

    template<typename Visitor,
             typename InputLabelType>
    [[nodiscard]]
    constexpr bool find_edge(Visitor && visitor,
                             const InputLabelType & label) noexcept
    {
      return m_edges.find_value(std::forward<Visitor>(visitor), label).found;
    }

    template<typename Visitor,
             typename InputLabelType>
    [[nodiscard]]
    constexpr bool find_edge(Visitor && visitor,
                             const InputLabelType & label) const noexcept
    {
      return m_edges.find_value(std::forward<Visitor>(visitor), label).found;
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) noexcept
    {
      m_edges.visit(std::forward<Visitor>(visitor));
    }

    constexpr void reserve(size_t size)
    {
      m_edges.reserve(size);
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
      return m_data;
    }

  private:
    value_ptr m_data = nullptr;
    edges_type m_edges{};
};

template<typename LabelType,
         typename ValueType,
         template<typename L> class TokenizerType>
struct trie_traits final
{
    using idx_traits = trie_node_idx_traits<LabelType, ValueType>;
    using label_type = typename idx_traits::label_type;
    using value_type = typename idx_traits::value_type;
    using value_ptr = typename idx_traits::value_ptr;

    using data_vector = yy_quad::simple_vector<value_type>;

    using idx_node_type = typename idx_traits::node_type;
    using idx_node_ptr = typename idx_traits::node_ptr;
    using idx_const_node_ptr = typename idx_traits::const_node_ptr;

    using idx_trie_vector = yy_quad::simple_vector<idx_node_type>;

    using ptr_traits = trie_node_ptr_traits<LabelType, ValueType>;
    using ptr_label_type = typename ptr_traits::label_type;
    using ptr_node_type = typename ptr_traits::node_type;
    using ptr_node_ptr = typename ptr_traits::node_ptr;
    using ptr_const_node_ptr = typename ptr_traits::const_node_ptr;

    using ptr_trie_vector = yy_quad::simple_vector<ptr_node_type>;

    static_assert(std::is_same_v<label_type, typename ptr_traits::label_type>, "trie_traits: value types different!");
    static_assert(std::is_same_v<value_type, typename ptr_traits::value_type>, "trie_traits: value types different!");

    using label_l_value_ref = typename idx_traits::label_l_value_ref;
    using label_const_l_value_ref = typename idx_traits::label_const_l_value_ref;
    using label_r_value_ref = typename idx_traits::label_r_value_ref;

    using value_idx_type = typename idx_traits::value_idx_type;
    using node_idx_type = typename idx_traits::node_idx_type;

    using tokenizer_type = TokenizerType<label_type>;
    using token_type = typename tokenizer_type::token_type;
};

template<typename TrieTraitsType>
class Automaton final
{
  public:
    using traits = TrieTraitsType;
    using tokenizer_type = typename traits::tokenizer_type;
    using token_type = typename traits::token_type;

    using label_const_l_value_ref = typename traits::label_const_l_value_ref;
    using label_span_type = typename tokenizer_type::label_span_type;

    using node_ptr = typename traits::ptr_node_ptr;

    using trie_vector = typename traits::ptr_trie_vector;
    using data_vector = typename traits::data_vector;

    constexpr explicit Automaton(trie_vector && p_nodes,
                                 data_vector && p_data) noexcept:
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
    constexpr bool find(InputSpanType && label) noexcept
    {
      return find_span(yy_quad::make_const_span(label));
    }

    constexpr void reset() noexcept
    {
      m_state = m_nodes.data();
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return nullptr == m_state;
    }

    [[nodiscard]]
    constexpr bool has_payload() const noexcept
    {
      return !empty() && !m_state->empty();
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) const noexcept
    {
      if(has_payload())
      {
        visitor(*m_state->data());
      }
    }

  private:
    [[nodiscard]]
    constexpr bool find_span(const label_span_type label) noexcept
    {
      reset();

      auto next_node_do = [this](node_ptr * edge_node, size_type)
      {
        m_state = *edge_node;
      };

      tokenizer_type tokenizer{label};

      while(!tokenizer.empty())
      {
        if(token_type label_part{tokenizer.scan()};
           !m_state->find_edge(next_node_do, label_part))
        {
          m_state = nullptr;
          return false;
        }
      }

      return has_payload();
    }

    trie_vector m_nodes{};
    data_vector m_data{};
    node_ptr m_state = nullptr;
};

} // namespace fm_flat_trie_ptr_detail

template<typename LabelType,
         typename ValueType,
         template<typename Traits> class Automaton = fm_flat_trie_ptr_detail::Automaton,
         template<typename L> class TokenizerType =  yafiyogi::yy_trie::default_label_tokenizer>
class fm_flat_trie_ptr final
{
  public:
    using trie_traits = fm_flat_trie_ptr_detail::trie_traits<LabelType,
                                                             ValueType,
                                                             TokenizerType>;

    using automaton_type = Automaton<trie_traits>;

    using label_type = typename trie_traits::label_type;
    using value_type = typename trie_traits::value_type;
    using tokenizer_type = typename trie_traits::tokenizer_type;

    using label_l_value_ref = typename trie_traits::label_l_value_ref;
    using label_const_l_value_ref = typename trie_traits::label_const_l_value_ref;
    using label_r_value_ref = typename trie_traits::label_r_value_ref;
    using label_span_type = tokenizer_type::label_span_type;

    using value_ptr = typename trie_traits::value_ptr;

    using idx_trie_vector = typename trie_traits::idx_trie_vector;
    using ptr_trie_vector = typename trie_traits::ptr_trie_vector;
    using data_vector = typename trie_traits::data_vector;

    using source_type = tokenizer_type::source_type;
    using token_type = tokenizer_type::token_type;

    constexpr fm_flat_trie_ptr() noexcept:
      m_nodes(1), // add root node
      m_data()
    {
    }

    fm_flat_trie_ptr(const fm_flat_trie_ptr &) = delete;
    constexpr fm_flat_trie_ptr(fm_flat_trie_ptr &&) noexcept = default;
    constexpr ~fm_flat_trie_ptr() noexcept = default;

    fm_flat_trie_ptr & operator=(const fm_flat_trie_ptr &) = delete;
    constexpr fm_flat_trie_ptr & operator=(fm_flat_trie_ptr &&) noexcept = default;

    struct data_added_type final
    {
        value_ptr data = nullptr;
        bool added = false;
    };

    template<typename InputType,
             typename InputValueType>
    constexpr data_added_type add(InputType && label,
                                  InputValueType && value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>,
                    yy_traits::remove_cvr_t<value_type>>,
                    "The value provided is not the correct type.");

      return add_span(yy_quad::make_const_span(label), std::forward<InputValueType>(value));
    }

    [[nodiscard]]
    constexpr automaton_type create_automaton() noexcept
    {
      // Copy nodes & data.
      ptr_trie_vector ptr_nodes{m_nodes.size()};
      data_vector ptr_data{m_data};

      // Transform node_idx_type to node_type *,
      // and transform value_idx_type to value_type *.
      auto ptr_nodes_begin{ptr_nodes.data()};
      auto ptr_data_begin{ptr_data.data()};

      for(size_type idx = 0; idx < m_nodes.size(); ++idx)
      {
        idx_node_type & idx_node = m_nodes[idx];
        ptr_node_type & ptr_node = ptr_nodes[idx];

        // Transform edges.
        ptr_node.reserve(idx_node.edges());

        idx_node.visit([ptr_nodes_begin, &ptr_node](label_l_value_ref label, node_idx_type node_idx) {
          // BUG! leave 'std::move(label)' as it causes performance reduction.
          ptr_node.add_edge(std::move(label), ptr_nodes_begin + node_idx);
        });

        // Transform data.
        if(value_idx_type value_idx = idx_node.data();
           no_data != value_idx)
        {
          ptr_node.data(ptr_data_begin + value_idx);
        }
      }

      return automaton_type{std::move(ptr_nodes), std::move(ptr_data)};
    }

  private:
    using value_idx_type = typename trie_traits::value_idx_type;
    using node_idx_type = typename trie_traits::node_idx_type;
    using idx_node_type = typename trie_traits::idx_node_type;
    using idx_node_ptr = typename trie_traits::idx_node_ptr;
    using idx_const_node_ptr = typename trie_traits::idx_const_node_ptr;

    using ptr_node_type = trie_traits::ptr_node_type;
    using ptr_node_ptr = trie_traits::ptr_node_ptr;

    [[nodiscard]]
    constexpr idx_node_ptr get_node(const node_idx_type idx) noexcept
    {
      return get_node(m_nodes.data(), idx);
    }

    [[nodiscard]]
    constexpr idx_const_node_ptr get_node(const node_idx_type idx) const noexcept
    {
      return get_node(m_nodes.data(), idx);
    }

    [[nodiscard]]
    static constexpr idx_node_ptr get_node(idx_node_ptr raw_nodes,
                                           const node_idx_type idx) noexcept
    {
      return raw_nodes + idx;
    }

    [[nodiscard]]
    static constexpr idx_const_node_ptr get_node(idx_const_node_ptr raw_nodes,
                                                 const node_idx_type idx) noexcept
    {
      return raw_nodes + idx;
    }

    [[nodiscard]]
    constexpr value_type & get_data(const value_idx_type idx) noexcept
    {
      return get_data(m_data, idx);
    }

    [[nodiscard]]
    constexpr const value_type & get_data(const value_idx_type idx) const noexcept
    {
      return get_data(m_data, idx);
    }

    [[nodiscard]]
    static constexpr value_type & get_data(data_vector & data,
                                           const value_idx_type idx) noexcept
    {
      return *get_data_ptr(data, idx);
    }

    [[nodiscard]]
    static constexpr const value_type & get_data(const data_vector & data,
                                                 const value_idx_type idx) noexcept
    {
      YY_ASSERT(idx < data.size());

      return *(data.data() + idx);
    }

    [[nodiscard]]
    constexpr value_ptr get_data_ptr(const value_idx_type idx) noexcept
    {
      return get_data_ptr(m_data, idx);
    }

    [[nodiscard]]
    static constexpr value_ptr get_data_ptr(data_vector & data,
                                            const value_idx_type idx) noexcept
    {
      YY_ASSERT(idx < data.size());

      return data.data() + idx;
    }

    template<typename InternalValueType>
    [[nodiscard]]
    static constexpr value_idx_type add_data(data_vector & data,
                                             InternalValueType && value)
    {
      static_assert(std::is_same_v<value_type, yy_traits::remove_cvr_t<InternalValueType>>,
                    "The data type of value is not 'value_type'");
      value_idx_type value_idx = data.size();

      data.emplace_back(std::forward<InternalValueType>(value));

      return value_idx;
    }

    [[nodiscard]]
    static constexpr node_idx_type add_node(idx_trie_vector & nodes,
                                            idx_node_ptr node,
                                            size_type pos,
                                            label_type && label,
                                            const value_idx_type value_idx)
    {
      node_idx_type node_idx{static_cast<node_idx_type>(nodes.size())};
      node->add_edge(pos, std::move(label), node_idx);

      nodes.emplace_back(value_idx);

      return node_idx;
    }

    [[nodiscard]]
    constexpr node_idx_type add_empty_nodes(idx_trie_vector & p_nodes,
                                            tokenizer_type & p_tokenizer)
    {
      node_idx_type node_idx = idx_node_type::root_idx;
      auto next_node_do = [&node_idx](node_idx_type *edge_node_idx,
                                      size_type /* pos */) {
        node_idx = *edge_node_idx;
      };

      token_type token{p_tokenizer.scan()};
      // Skip exising nodes.
      while(!p_tokenizer.empty())
      {
        if(!get_node(p_nodes.data(), node_idx)->find_edge(next_node_do, token).found)
        {
          break;
        }
        token = p_tokenizer.scan();
      }

      // Add new nodes;
      while(!p_tokenizer.empty())
      {
        auto node = get_node(p_nodes.data(), node_idx);
        auto [edge_pos, ignore] = node->find_edge_pos(token);

        node_idx = add_node(p_nodes, node, edge_pos, tokenizer_type::create(token), no_data);

        token = p_tokenizer.scan();
      }

      return node_idx;
    }

    template<typename InputValueType>
    constexpr data_added_type add_span(label_span_type label,
                                       InputValueType && value)
    {
      if(!label.empty())
      {
        tokenizer_type l_tokenizer{label};

        node_idx_type node_idx = add_empty_nodes(m_nodes, l_tokenizer);
        auto node = get_node(m_nodes.data(), node_idx);

        node_idx_type * edge_node_idx = nullptr;;
        auto do_find_edge = [&edge_node_idx]
                            (node_idx_type * idx, size_type) {
          edge_node_idx = idx;
        };

        auto payload_label{l_tokenizer.token()};
        auto [edge_pos, found] = node->find_edge(do_find_edge, payload_label);

        if(found)
        {
          // Overwrite an existing node.
          auto & edge_node = *get_node(m_nodes.data(), *edge_node_idx);
          if(!edge_node.empty())
          {
            // Data node exists.
            // Keep existing data.
            return data_added_type{get_data_ptr(edge_node.data()), false};
          }

          // Add new data item, & update node data idx
          auto value_idx = add_data(m_data, std::forward<InputValueType>(value));
          edge_node.data(value_idx);

          return data_added_type{get_data_ptr(edge_node.data()), true};
        }

        // No data node exists.
        // Add data node.
        auto value_idx = add_data(m_data, std::forward<InputValueType>(value));
        label_type node_label{tokenizer_type::create(payload_label)};
        std::ignore = add_node(m_nodes, node, edge_pos, std::move(node_label), value_idx);

        return data_added_type{get_data_ptr(value_idx), true};
      }

      return data_added_type{};
    }

    idx_trie_vector m_nodes{};
    data_vector m_data{};
};

} // namespace yafiyogi::yy_data
