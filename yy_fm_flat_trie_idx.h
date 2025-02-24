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
#include "yy_vector.h"

namespace yafiyogi::yy_data {
namespace fm_flat_trie_idx_detail {

template<typename LabelType,
         typename ValueType>
class trie_node_idx;

template<typename LabelType,
         typename ValueType>
class trie_node_ptr;

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

    using node_type = trie_node_idx<LabelType, ValueType>;
    using node_idx_type = std::size_t;
    using data_idx_type = std::size_t;
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
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_idx_type = typename traits::node_idx_type;
    using node_type = typename traits::node_type;
    using value_type = typename traits::value_type;
    using data_idx_type = typename traits::data_idx_type;
    using edges_type = typename traits::edges_type;

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
                            node_idx_type node_idx)
    {
      std::ignore = m_edges.emplace(pos, std::move(label), node_idx);
    }

    template<typename Visitor,
             typename InputLabelType>
    [[nodiscard]]
    constexpr auto find_edge(Visitor && visitor,
                             const InputLabelType & label) noexcept
    {
      return m_edges.find_value(std::forward<Visitor>(visitor), label);
    }

    template<typename InputLabelType>
    [[nodiscard]]
    constexpr auto find_edge_pos(const InputLabelType & label) const noexcept
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

template<typename LabelType,
         typename ValueType>
struct trie_idx_traits final
{
    using traits = trie_node_idx_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_idx_type = typename traits::node_idx_type;
    using value_type = typename traits::value_type;
    using data_idx_type = typename traits::data_idx_type;
    using edges_type = typename traits::edges_type;

    using trie_vector = yy_quad::simple_vector<node_type>;
    using data_vector = yy_quad::simple_vector<value_type>;
};

template<typename LabelType,
         typename ValueType,
         typename TokenizerType>
class Automaton final
{
  public:
    using traits = trie_idx_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_idx_type = typename traits::node_idx_type;
    using value_type = typename traits::value_type;
    using data_idx_type = typename traits::data_idx_type;
    using edges_type = typename traits::edges_type;

    using trie_vector = typename traits::trie_vector;
    using data_vector = typename traits::data_vector;
    using tokenizer_type = TokenizerType;
    using label_span_type = typename tokenizer_type::label_span_type;

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
      m_state = node_type::root_idx;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return node_type::root_idx == m_state;
    }

    [[nodiscard]]
    constexpr bool has_payload() const noexcept
    {
      return !empty() && !get_node(m_state)->empty();
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) const noexcept
    {
      if(has_payload())
      {
        visitor(get_data(get_node(m_state)->data()));
      }
    }

  private:
    [[nodiscard]]
    constexpr node_type * get_node(const node_idx_type idx) noexcept
    {
      return get_node(m_nodes.data(), idx);
    }

    [[nodiscard]]
    constexpr const node_type * get_node(const node_idx_type idx) const noexcept
    {
      return get_node(m_nodes.data(), idx);
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
    constexpr value_type & get_data(const data_idx_type idx) noexcept
    {
      return get_data(m_data, idx);
    }

    [[nodiscard]]
    constexpr const value_type & get_data(const data_idx_type idx) const noexcept
    {
      return get_data(m_data, idx);
    }

    [[nodiscard]]
    static constexpr value_type & get_data(data_vector & data,
                                           const data_idx_type idx) noexcept
    {
      return *get_data_ptr(data, idx);
    }

    [[nodiscard]]
    static constexpr const value_type & get_data(const data_vector & data,
                                                 const data_idx_type idx) noexcept
    {
      YY_ASSERT(idx < data.size());

      return *(data.data() + idx);
    }

    [[nodiscard]]
    constexpr bool find_span(const label_span_type label) noexcept
    {
      reset();

      auto node_idx = m_state;
      auto next_node_do = [&node_idx](auto edge_idx, size_type)
      {
        node_idx = *edge_idx;
      };

      tokenizer_type tokenizer{label};

      while(!tokenizer.empty())
      {
        if(const auto label_part{tokenizer.scan()};
           !get_node(node_idx)->find_edge(next_node_do, label_part).found)
        {
          m_state = node_type::root_idx;
          return false;
        }
      }

      m_state = node_idx;

      return has_payload();
    }

    trie_vector m_nodes{};
    data_vector m_data{};
    node_idx_type m_state = node_type::root_idx;
};

} // namespace detail

template<typename LabelType,
         typename ValueType,
         template<typename L, typename V, typename T> class Automaton = fm_flat_trie_idx_detail::Automaton,
         template<typename L> class Tokenizer = yy_trie::default_label_tokenizer>
class fm_flat_trie_idx final
{
  public:
    using traits = typename fm_flat_trie_idx_detail::trie_idx_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_idx_type = typename traits::node_idx_type;
    using value_type = typename traits::value_type;
    using data_idx_type = typename traits::data_idx_type;
    using edges_type = typename traits::edges_type;

    using trie_vector = typename traits::trie_vector;
    using data_vector = typename traits::data_vector;

    using tokenizer_type = Tokenizer<label_type>;
    using label_span_type = tokenizer_type::label_span_type;
    using source_type = tokenizer_type::source_type;
    using token_type = tokenizer_type::token_type;
    using automaton_type = Automaton<label_type, value_type, tokenizer_type>;

    constexpr fm_flat_trie_idx() noexcept:
      m_nodes(1), // add root node
      m_data()
    {
    }

    fm_flat_trie_idx(const fm_flat_trie_idx &) = delete;
    constexpr fm_flat_trie_idx(fm_flat_trie_idx &&) noexcept = default;
    constexpr ~fm_flat_trie_idx() noexcept = default;

    fm_flat_trie_idx & operator=(const fm_flat_trie_idx &) = delete;
    constexpr fm_flat_trie_idx & operator=(fm_flat_trie_idx &&) noexcept = default;

    struct data_added_type final
    {
        value_type * data = nullptr;
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
      typename automaton_type::trie_vector new_nodes{m_nodes};
      typename automaton_type::data_vector new_data{m_data};

      return automaton_type{std::move(new_nodes), std::move(new_data)};
    }

  private:
    [[nodiscard]]
    constexpr node_type * get_node(const node_idx_type idx) noexcept
    {
      return get_node(m_nodes.data(), idx);
    }

    [[nodiscard]]
    constexpr const node_type * get_node(const node_idx_type idx) const noexcept
    {
      return get_node(m_nodes.data(), idx);
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
    constexpr value_type & get_data(const data_idx_type idx) noexcept
    {
      return get_data(m_data, idx);
    }

    [[nodiscard]]
    constexpr const value_type & get_data(const data_idx_type idx) const noexcept
    {
      return get_data(m_data, idx);
    }

    [[nodiscard]]
    static constexpr value_type & get_data(data_vector & data,
                                           const data_idx_type idx) noexcept
    {
      return *get_data_ptr(data, idx);
    }

    [[nodiscard]]
    static constexpr const value_type & get_data(const data_vector & data,
                                                 const data_idx_type idx) noexcept
    {
      YY_ASSERT(idx < data.size());

      return *(data.data() + idx);
    }

    [[nodiscard]]
    constexpr value_type * get_data_ptr(const data_idx_type idx) noexcept
    {
      return get_data_ptr(m_data, idx);
    }

    [[nodiscard]]
    static constexpr value_type * get_data_ptr(data_vector & data,
                                               const data_idx_type idx) noexcept
    {
      YY_ASSERT(idx < data.size());

      return data.data() + idx;
    }

    template<typename InternalValueType>
    [[nodiscard]]
    static constexpr data_idx_type add_data(data_vector & data,
                                            InternalValueType && value)
    {
      static_assert(std::is_same_v<value_type, yy_traits::remove_cvr_t<InternalValueType>>,
                    "The data type of value is not 'value_type'");
      data_idx_type data_idx = data.size();

      data.emplace_back(std::forward<InternalValueType>(value));

      return data_idx;
    }

    [[nodiscard]]
    static constexpr node_idx_type add_node(trie_vector & nodes,
                                            node_type * node,
                                            size_type pos,
                                            label_type label,
                                            const data_idx_type data_idx)
    {
      node_idx_type node_idx{static_cast<node_idx_type>(nodes.size())};
      node->add_edge(pos, std::move(label), node_idx);

      nodes.emplace_back(data_idx);

      return node_idx;
    }

    [[nodiscard]]
    constexpr node_idx_type add_empty_nodes(trie_vector & nodes,
                                            tokenizer_type & p_tokenizer)
    {
      node_idx_type node_idx = node_type::root_idx;
      auto next_node_do = [&node_idx](auto edge_node_idx,
                                      size_type /* pos */) {
        node_idx = *edge_node_idx;
      };

      token_type token{p_tokenizer.scan()};
      // Skip exising nodes.
      while(!p_tokenizer.empty())
      {
        if(!get_node(nodes.data(), node_idx)->find_edge(next_node_do, token).found)
        {
          break;
        }
        token = p_tokenizer.scan();
      }

      // Add new nodes;
      while(!p_tokenizer.empty())
      {
        auto node = get_node(nodes.data(), node_idx);
        auto [edge_pos, ignore] = node->find_edge_pos(token);

        node_idx = add_node(nodes, node, edge_pos, tokenizer_type::create(token), no_data);

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
                            (auto idx, size_type) {
          edge_node_idx = idx.get();
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
          auto data_idx = add_data(m_data, std::forward<InputValueType>(value));
          edge_node.data(data_idx);

          return data_added_type{get_data_ptr(edge_node.data()), true};
        }

        // No data node exists.
        // Add data node.
        auto data_idx = add_data(m_data, std::forward<InputValueType>(value));
        label_type node_label{tokenizer_type::create(payload_label)};
        std::ignore = add_node(m_nodes, node, edge_pos, std::move(node_label), data_idx);

        return data_added_type{get_data_ptr(data_idx), true};
      }

      return data_added_type{};
    }

    trie_vector m_nodes;
    data_vector m_data;
};

} // namespace yafiyogi::yy_data
