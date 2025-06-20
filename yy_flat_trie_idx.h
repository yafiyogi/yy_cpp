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
#include "yy_trie_node_idx.hpp"

namespace yafiyogi::yy_data {
namespace flat_trie_idx_detail {

template<typename LabelType,
         typename ValueType>
struct trie_idx_traits final
{
    using traits = trie_node_idx_detail::trie_node_idx_traits<LabelType, ValueType>;
    using label_traits = typename traits::label_traits;
    using value_traits = typename traits::value_traits;

    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;

    using value_type = typename traits::value_type;

    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using const_node_ptr = typename traits::const_node_ptr;
    using node_idx_type = typename traits::node_idx_type;
    using node_idx_ptr = std::add_pointer_t<node_idx_type>;
    using data_idx_type = typename traits::data_idx_type;
    using data_idx_ptr = typename traits::data_idx_ptr;

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
    using node_ptr = typename traits::node_ptr;
    using const_node_ptr = typename traits::const_node_ptr;
    using node_idx_type = typename traits::node_idx_type;
    using value_type = typename traits::value_type;
    using data_idx_type = typename traits::data_idx_type;
    using data_idx_ptr = typename traits::data_idx_ptr;

    using trie_vector = typename traits::trie_vector;
    using data_vector = typename traits::data_vector;
    using tokenizer_type = TokenizerType;
    using token_type = tokenizer_type::token_type;
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
      return !empty() && !get_node(m_nodes.data(), m_state)->empty();
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) const noexcept
    {
      if(has_payload())
      {
        visitor(get_data(get_node(m_nodes.data(), m_state)->data()));
      }
    }

  private:
    [[nodiscard]]
    static inline constexpr node_ptr get_node(node_type * raw_nodes,
                                              const node_idx_type idx) noexcept
    {
      return raw_nodes + idx;
    }

    [[nodiscard]]
    static inline constexpr const_node_ptr get_node(const node_type * raw_nodes,
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

      tokenizer_type tokenizer{label};

      auto node = m_state;
      auto nodes = m_nodes.data();

      while(!tokenizer.empty())
      {
        auto [edge, found] = get_node(nodes, node)->find_edge(tokenizer.scan());

        if(!found)
        {
          m_state = node_type::root_idx;
          return false;
        }

        node = *edge;
      }

      m_state = node;

      return has_payload();
    }

    trie_vector m_nodes{};
    data_vector m_data{};
    node_idx_type m_state = node_type::root_idx;
};

} // namespace flat_trie_idx_detail

template<typename LabelType,
         typename ValueType,
         template<typename L, typename V, typename T> class Automaton = flat_trie_idx_detail::Automaton,
         template<typename L> class Tokenizer = yy_trie::default_label_tokenizer>
class flat_trie_idx final
{
  public:
    using traits = typename flat_trie_idx_detail::trie_idx_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using node_idx_type = typename traits::node_idx_type;
    using node_idx_ptr = typename traits::node_idx_ptr;
    using value_type = typename traits::value_type;
    using value_ptr = typename traits::value_traits::value_ptr;
    using data_idx_type = typename traits::data_idx_type;
    using data_idx_ptr = typename traits::data_idx_ptr;

    using trie_vector = typename traits::trie_vector;
    using data_vector = typename traits::data_vector;

    using tokenizer_type = Tokenizer<label_type>;
    using label_span_type = tokenizer_type::label_span_type;
    using source_type = tokenizer_type::source_type;
    using token_type = tokenizer_type::token_type;
    using automaton_type = Automaton<label_type, value_type, tokenizer_type>;

    constexpr flat_trie_idx() noexcept:
      m_nodes(1), // add root node
      m_data()
    {
    }

    flat_trie_idx(const flat_trie_idx &) = delete;
    constexpr flat_trie_idx(flat_trie_idx &&) noexcept = default;
    constexpr ~flat_trie_idx() noexcept = default;

    flat_trie_idx & operator=(const flat_trie_idx &) = delete;
    constexpr flat_trie_idx & operator=(flat_trie_idx &&) noexcept = default;

    struct data_added_type final
    {
        value_ptr data{};
        bool added = false;
    };

    template<typename InputType,
             typename InputValueType>
    constexpr data_added_type add(InputType && p_label,
                                  InputValueType && p_value)
    {
      static_assert(std::is_convertible_v<yy_traits::remove_cvr_t<InputValueType>,
                    yy_traits::remove_cvr_t<value_type>>,
                    "The value provided is not the correct type.");

      return add_span(yy_quad::make_const_span(p_label), std::forward<InputValueType>(p_value));
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
    constexpr value_ptr get_data_ptr(const data_idx_type idx) noexcept
    {
      return get_data_ptr(m_data, idx);
    }

    [[nodiscard]]
    static constexpr value_ptr get_data_ptr(data_vector & data,
                                               const data_idx_type idx) noexcept
    {
      YY_ASSERT(idx < data.size());

      return value_ptr{data.data() + idx};
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
    static constexpr node_idx_type add_node(trie_vector & p_nodes,
                                            node_type * p_parent_node,
                                            data_idx_ptr p_edge,
                                            label_type p_label,
                                            const data_idx_type p_data_idx)
    {
      node_idx_type node_idx{static_cast<node_idx_type>(p_nodes.size())};
      p_parent_node->add_edge(p_edge, std::move(p_label), node_idx);

      p_nodes.emplace_back(node_type{p_data_idx});

      return node_idx;
    }

    [[nodiscard]]
    constexpr node_idx_type add_empty_nodes(trie_vector & p_nodes,
                                            tokenizer_type & p_tokenizer)
    {
      node_idx_type node_idx = node_type::root_idx;
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
        node_ptr node = get_node(m_nodes.data(), node_idx);

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
          auto data_idx = add_data(m_data, std::forward<InputValueType>(p_value));
          edge_node.data(data_idx);

          return data_added_type{get_data_ptr(edge_node.data()), true};
        }

        // No data node exists.
        // Add data node.
        auto data_idx = add_data(m_data, std::forward<InputValueType>(p_value));
        label_type node_label{tokenizer_type::create(payload_label)};
        std::ignore = add_node(m_nodes, node, edge, std::move(node_label), data_idx);

        return data_added_type{get_data_ptr(data_idx), true};
      }

      return data_added_type{};
    }

    trie_vector m_nodes;
    data_vector m_data;
};

} // namespace yafiyogi::yy_data
