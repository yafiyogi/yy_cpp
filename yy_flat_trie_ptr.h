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

#include "yy_assert.h"
#include "yy_observer_ptr.hpp"
#include "yy_span.h"
#include "yy_trie_common.h"
#include "yy_trie_node_idx.hpp"
#include "yy_trie_node_ptr.hpp"
#include "yy_type_traits.h"
#include "yy_vector.h"

namespace yafiyogi::yy_data {
namespace flat_trie_ptr_detail {

template<typename LabelType,
         typename ValueType,
         template<typename L> class TokenizerType>
struct trie_traits final
{
    using idx_traits = trie_node_idx_detail::trie_node_idx_traits<LabelType, ValueType>;
    using label_type = typename idx_traits::label_type;
    using idx_value_type = typename idx_traits::value_type;
    using idx_value_ptr = typename idx_traits::value_traits::value_ptr;

    using idx_node_type = typename idx_traits::node_type;
    using idx_node_ptr = typename idx_traits::node_ptr;
    using idx_const_node_ptr = typename idx_traits::const_node_ptr;
    using idx_node_idx_ptr = typename idx_traits::node_idx_ptr;
    using idx_data_vector = yy_quad::simple_vector<idx_value_type>;
    using idx_trie_vector = yy_quad::simple_vector<idx_node_type>;

    using ptr_traits = trie_node_ptr_detail::trie_node_ptr_traits<LabelType, ValueType>;
    using ptr_label_type = typename ptr_traits::label_type;
    using ptr_node_type = typename ptr_traits::node_type;
    using ptr_node_ptr = typename ptr_traits::node_ptr;
    using ptr_const_node_ptr = typename ptr_traits::const_node_ptr;
    using ptr_value_type = typename ptr_traits::value_type;
    using ptr_value_ptr = typename ptr_traits::value_ptr;
    using ptr_const_value_ptr = typename ptr_traits::const_value_ptr;

    using ptr_data_vector = yy_quad::simple_vector<ptr_value_type>;
    using ptr_trie_vector = yy_quad::simple_vector<ptr_node_type>;

    using value_type = ptr_value_type;
    using value_ptr = ptr_value_ptr;
    using data_vector = ptr_data_vector;

    static_assert(std::is_same_v<label_type, typename ptr_traits::label_type>, "trie_traits: value types different!");
    static_assert(std::is_same_v<ptr_value_type, typename ptr_traits::value_type>, "trie_traits: value types different!");

    using label_l_value_ref = typename idx_traits::label_l_value_ref;
    using label_r_value_ref = typename idx_traits::label_r_value_ref;

    using data_idx_type = typename idx_traits::data_idx_type;
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
    constexpr bool find(InputSpanType && p_label) noexcept
    {
      return find_span(yy_quad::make_const_span(p_label));
    }

    constexpr void reset() noexcept
    {
      m_state = m_nodes.data();
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return m_nodes.data() == m_state;
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
    constexpr bool find_span(const label_span_type p_label) noexcept
    {
      reset();

      tokenizer_type tokenizer{p_label};

      while(!tokenizer.empty())
      {
        auto [edge, found] = m_state->find_edge(tokenizer.scan());

        if(!found)
        {
          m_state = nullptr;
          return false;
        }

        m_state = *edge;
      }

      return has_payload();
    }

    trie_vector m_nodes{};
    data_vector m_data{};
    node_ptr m_state = nullptr;
};

} // namespace flat_trie_ptr_detail

template<typename LabelType,
         typename ValueType,
         template<typename Traits> class Automaton = flat_trie_ptr_detail::Automaton,
         template<typename L> class TokenizerType =  yafiyogi::yy_trie::default_label_tokenizer>
class flat_trie_ptr final
{
  public:
    using traits = flat_trie_ptr_detail::trie_traits<LabelType,
                                                     ValueType,
                                                     TokenizerType>;
    using automaton_type = Automaton<traits>;

    using label_type = typename traits::label_type;
    using value_type = typename traits::ptr_value_type;
    using tokenizer_type = typename traits::tokenizer_type;

    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using label_span_type = tokenizer_type::label_span_type;

    using value_ptr = typename traits::value_ptr;

    using idx_value_type =  typename traits::idx_value_type;
    using idx_value_ptr =  typename traits::idx_value_ptr;
    using idx_trie_vector = typename traits::idx_trie_vector;
    using idx_data_vector = typename traits::idx_data_vector;
    using ptr_trie_vector = typename traits::ptr_trie_vector;
    using ptr_data_vector = typename traits::ptr_data_vector;

    using ptr_node_type = traits::ptr_node_type;
    using ptr_node_ptr = traits::ptr_node_ptr;
    using ptr_value_ptr = traits::ptr_value_ptr;

    using source_type = tokenizer_type::source_type;
    using token_type = tokenizer_type::token_type;

    constexpr flat_trie_ptr() noexcept:
      m_nodes(1), // add root node
      m_data()
    {
    }

    flat_trie_ptr(const flat_trie_ptr &) = delete;
    constexpr flat_trie_ptr(flat_trie_ptr &&) noexcept = default;
    constexpr ~flat_trie_ptr() noexcept = default;

    flat_trie_ptr & operator=(const flat_trie_ptr &) = delete;
    constexpr flat_trie_ptr & operator=(flat_trie_ptr &&) noexcept = default;

    struct data_added_type final
    {
        idx_value_ptr data{};
        bool added = false;
    };

    template<typename InputLabelType,
             typename InputValueType>
    constexpr data_added_type add(InputLabelType && p_label,
                                  InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>, idx_value_type>,
                    "The value provided is not the correct type.");

      return add_span(yy_quad::make_const_span(p_label), std::forward<InputValueType>(p_value));
    }

    [[nodiscard]]
    constexpr automaton_type create_automaton() noexcept
    {
      // Copy nodes & data.
      ptr_trie_vector ptr_nodes{};
      ptr_nodes.reserve(m_nodes.size());

      ptr_data_vector ptr_data{};
      ptr_data.reserve(m_data.size());

      for(auto & data : m_data)
      {
        ptr_data.emplace_back(data);
      }

      // Transform node_idx_type to node_type *,
      // and transform data_idx_type to value_type *.
      ptr_node_ptr ptr_nodes_begin{ptr_nodes.data()};
      ptr_value_ptr ptr_data_begin{ptr_data.data()};

      for(size_type idx = 0; idx < m_nodes.size(); ++idx)
      {
        idx_node_type & idx_node = m_nodes[idx];
        ptr_node_type & ptr_node = ptr_nodes[idx];

        // Transform edges.
        ptr_node.reserve(idx_node.edges());

        idx_node.visit([ptr_nodes_begin, &ptr_node](label_l_value_ref label, node_idx_type node_idx) {
          // BUG! leave 'std::move(label)'! Removing 'std::move()' causes lookup performance reduction.
          ptr_node.add_edge(std::move(label), ptr_nodes_begin + node_idx);
        });

        // Transform data.
        if(data_idx_type value_idx = idx_node.data();
           no_data != value_idx)
        {
          ptr_node.data(ptr_data_begin + value_idx);
        }
      }

      return automaton_type{std::move(ptr_nodes), std::move(ptr_data)};
    }

  private:
    using data_idx_type = typename traits::data_idx_type;
    using node_idx_type = typename traits::node_idx_type;
    using idx_node_type = typename traits::idx_node_type;
    using idx_node_ptr = typename traits::idx_node_ptr;
    using idx_const_node_ptr = typename traits::idx_const_node_ptr;
    using idx_node_idx_ptr = typename traits::idx_node_idx_ptr;

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
      return idx_node_ptr{raw_nodes + idx};
    }

    [[nodiscard]]
    static constexpr idx_const_node_ptr get_node(idx_const_node_ptr raw_nodes,
                                                 const node_idx_type idx) noexcept
    {
      return idx_const_node_ptr{raw_nodes + idx};
    }

    [[nodiscard]]
    constexpr idx_value_type & get_data(const data_idx_type idx) noexcept
    {
      return get_data(m_data, idx);
    }

    [[nodiscard]]
    constexpr const idx_value_type & get_data(const data_idx_type idx) const noexcept
    {
      return get_data(m_data, idx);
    }

    [[nodiscard]]
    static constexpr idx_value_type & get_data(idx_data_vector & data,
                                               const data_idx_type idx) noexcept
    {
      return *get_data_ptr(data, idx);
    }

    [[nodiscard]]
    static constexpr const idx_value_type & get_data(const idx_data_vector & data,
                                                     const data_idx_type idx) noexcept
    {
      YY_ASSERT(idx < data.size());

      return *(data.data() + idx);
    }

    [[nodiscard]]
    constexpr idx_value_ptr get_data_ptr(const data_idx_type idx) noexcept
    {
      return get_data_ptr(m_data, idx);
    }

    [[nodiscard]]
    static constexpr idx_value_ptr get_data_ptr(idx_data_vector & data,
                                                const data_idx_type idx) noexcept
    {
      YY_ASSERT(idx < data.size());

      return idx_value_ptr{data.data() + idx};
    }

    template<typename InternalValueType>
    [[nodiscard]]
    static constexpr data_idx_type add_data(idx_data_vector & data,
                                             InternalValueType && value)
    {
      // static_assert(std::is_same_v<idx_value_type, yy_traits::remove_cvr_t<InternalValueType>>,
      //               "The data type of value is not 'value_type'");
      data_idx_type value_idx = data.size();

      data.emplace_back(std::forward<InternalValueType>(value));

      return value_idx;
    }

    [[nodiscard]]
    static constexpr node_idx_type add_node(idx_trie_vector & nodes,
                                            idx_node_ptr node,
                                            idx_node_idx_ptr edge,
                                            label_type && label,
                                            const data_idx_type value_idx)
    {
      node_idx_type node_idx{static_cast<node_idx_type>(nodes.size())};
      node->add_edge(edge, std::move(label), node_idx);

      nodes.emplace_back(idx_node_type{value_idx});

      return node_idx;
    }

    [[nodiscard]]
    constexpr node_idx_type add_empty_nodes(idx_trie_vector & p_nodes,
                                            tokenizer_type & p_tokenizer)
    {
      node_idx_type node_idx = idx_node_type::root_idx;
      token_type token{p_tokenizer.scan()};

      // Skip exising nodes.
      while(!p_tokenizer.empty())
      {
        auto [edge, found] = get_node(p_nodes.data(), node_idx)->find_edge(token);
        if(!found)
        {
          break;
        }
        node_idx = *edge;
        token = p_tokenizer.scan();
      }

      // Add new nodes;
      while(!p_tokenizer.empty())
      {
        auto node = get_node(p_nodes.data(), node_idx);
        auto [edge, ignore] = node->find_edge(token);

        node_idx = add_node(p_nodes, node, edge, tokenizer_type::create(token), no_data);

        token = p_tokenizer.scan();
      }

      return node_idx;
    }

    template<typename InputValueType>
    constexpr data_added_type add_span(label_span_type p_label,
                                       InputValueType && p_value)
    {
      if(!p_label.empty())
      {
        tokenizer_type l_tokenizer{p_label};

        node_idx_type node_idx = add_empty_nodes(m_nodes, l_tokenizer);
        idx_node_ptr node{get_node(m_nodes.data(), node_idx)};

        auto payload_label{l_tokenizer.token()};
        auto [edge, found] = node->find_edge(payload_label);

        if(found)
        {
          // Overwrite an existing node.
          auto & edge_node = *get_node(m_nodes.data(), *edge);
          if(!edge_node.empty())
          {
            // Data node exists.
            // Keep existing data.
            return data_added_type{get_data_ptr(edge_node.data()), false};
          }

          // Add new data item, & update node data idx
          auto value_idx = add_data(m_data, std::forward<InputValueType>(p_value));
          edge_node.data(value_idx);

          return data_added_type{get_data_ptr(edge_node.data()), true};
        }

        // No data node exists.
        // Add data node.
        auto value_idx = add_data(m_data, std::forward<InputValueType>(p_value));
        label_type node_label{tokenizer_type::create(payload_label)};
        std::ignore = add_node(m_nodes, node, edge, std::move(node_label), value_idx);

        return data_added_type{get_data_ptr(value_idx), true};
      }

      return data_added_type{};
    }

    idx_trie_vector m_nodes{};
    idx_data_vector m_data{};
};

} // namespace yafiyogi::yy_data
