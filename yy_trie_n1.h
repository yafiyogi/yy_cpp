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

#include <memory>
#include <stdexcept>

#include "yy_flat_map.h"
#include "yy_ref_traits.h"
#include "yy_span.h"
#include "yy_trie_common.h"
#include "yy_type_traits.h"
#include "yy_utility.h"

namespace yafiyogi::yy_data {
namespace trie_n1_detail {

template<typename LabelType,
         typename ValueType>
class trie_node;

template<typename LabelType,
         typename ValueType>
struct trie_node_traits final
{
    using label_traits = yy_trie::label_traits<LabelType>;
    using value_traits = yy_trie::value_traits<ValueType>;

    using label_type = typename label_traits::label_type;
    using label_l_value_ref = typename label_traits::label_l_value_ref;
    using label_const_l_value_ref = typename label_traits::label_const_l_value_ref;
    using label_r_value_ref = typename label_traits::label_r_value_ref;

    using value_type = typename value_traits::value_type;

    using node_type = trie_node<label_type, value_type>;
    using node_ptr = std::unique_ptr<node_type>;
    using root_node_ptr = std::shared_ptr<node_type>;

    using edges_type = yy_data::flat_map<label_type, node_ptr>;
};

template<typename LabelType,
         typename ValueType>
class trie_node
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_const_l_value_ref = typename traits::label_const_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_ptr = typename traits::node_ptr;
    using value_type = typename traits::value_type;
    using edges_type = typename traits::edges_type;

    constexpr trie_node() noexcept = default;
    constexpr trie_node(const trie_node & node) noexcept = default;
    constexpr trie_node(trie_node && node) noexcept = default;
    constexpr virtual ~trie_node() noexcept = default;

    constexpr trie_node & operator=(const trie_node & node) noexcept = default;
    constexpr trie_node & operator=(trie_node && node) noexcept = default;

    constexpr void add_edge(size_type pos,
                            label_r_value_ref label,
                            node_ptr node)
    {
      std::ignore = m_edges.emplace(pos, std::move(label), std::move(node));
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

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) noexcept
    {
      m_edges.visit(std::forward<Visitor>(visitor));
    }

    [[nodiscard]]
    constexpr virtual bool empty() const noexcept
    {
      return true;
    }

    [[nodiscard]]
    constexpr virtual value_type & value()
    {
      throw std::runtime_error("Invalid value");
    }

    [[nodiscard]]
    constexpr virtual const value_type & value() const
    {
      throw std::runtime_error("Invalid value");
    }

    constexpr void swap_edge(size_type p_edge_pos, node_ptr p_new) noexcept
    {
      auto [label, edge] = m_edges[p_edge_pos];

      static_assert(std::is_reference_v<decltype(edge)>);

      edge->m_edges.swap(p_new->m_edges);
      edge = std::move(p_new);
    }

  private:
    edges_type m_edges{};
};

template<typename LabelType,
         typename ValueType>
class Payload final:
      public trie_node<LabelType, ValueType>
{
  public:
    using traits = typename trie_node<LabelType, ValueType>::traits;
    using node_type = typename traits::node_type;
    using value_type = typename traits::value_type;

    constexpr explicit Payload(const value_type & payload) noexcept:
      m_payload(payload)
    {
    }

    constexpr explicit Payload(value_type && payload) noexcept:
      m_payload(std::move(payload))
    {
    }

    Payload() = delete;
    constexpr Payload(const Payload &) noexcept = default;
    constexpr Payload(Payload &&) noexcept = default;
    constexpr ~Payload() noexcept override = default;

    constexpr Payload & operator=(const Payload &) noexcept = default;
    constexpr Payload & operator=(Payload &&) noexcept = default;

    [[nodiscard]]
    constexpr bool empty() const noexcept override
    {
      return false;
    }

    [[nodiscard]]
    constexpr value_type & value() override
    {
      return m_payload;
    }

    [[nodiscard]]
    constexpr const value_type & value() const override
    {
      return m_payload;
    }

  private:
    value_type m_payload{};
};

template<typename LabelType,
         typename ValueType>
class Automaton final
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_const_l_value_ref = typename traits::label_const_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;

    constexpr explicit Automaton(root_node_ptr p_root) noexcept:
      m_root(std::move(p_root)),
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
      m_state = m_root.get();
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
    constexpr void visit(Visitor && visitor) const
    {
      if(has_payload())
      {
        visitor(m_state->value());
      }
    }

  protected:
    template<typename InputSpanType>
    [[nodiscard]]
    constexpr bool find_span(const InputSpanType label) noexcept
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "trie::find_span(): InputSpanType is not a yy_quad::span<>");

      reset();

      node_type * node = m_state;

      auto do_find_edge = [&node](auto p_edge, auto) {
        node = p_edge->get();
      };

      for(label_const_l_value_ref label_part : label)
      {
        if(!node->find_edge(do_find_edge, label_part).found)
        {
          m_state = nullptr;
          return false;
        }
      }

      m_state = node;

      return has_payload();
    }

  private:
    root_node_ptr m_root{};
    node_type * m_state = nullptr;
};

} // namespace trie_n1_detail

template<typename LabelType,
         typename ValueType,
         typename Automaton = trie_n1_detail::Automaton<LabelType, ValueType>>
class trie_n1 final
{
  public:
    using traits = typename trie_n1_detail::trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_const_l_value_ref = typename traits::label_const_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using value_type = typename traits::value_type;
    using value_node = typename trie_n1_detail::Payload<label_type, value_type>;
    using automaton = Automaton;

    constexpr trie_n1() noexcept:
      m_root(std::make_shared<node_type>())
    {
    }

    trie_n1(const trie_n1 &) = delete;
    constexpr trie_n1(trie_n1 &&) noexcept = default;
    constexpr ~trie_n1() noexcept = default;

    trie_n1 & operator=(const trie_n1 &) = delete;
    constexpr trie_n1 & operator=(trie_n1 &&) noexcept = default;

    template<typename Container,
             typename Value>
    constexpr void add(Container && label,
                       Value && value)
    {
      static_assert(std::is_same_v<yy_traits::remove_cvr_t<Value>,
                    yy_traits::remove_cvr_t<value_type>>,
                    "The value provided is not the correct type.");
      add_span(yy_quad::make_const_span(label), std::forward<Value>(value));
    }

    [[nodiscard]]
    constexpr automaton create_automaton() noexcept
    {
      return automaton{m_root};
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor)
    {
      if(m_root)
      {
        m_root->visit(visitor);
      }
    }

  private:
    constexpr node_type * add_node(node_type * p_parent,
                                   size_type p_edge_pos,
                                   const label_type & p_label)
    {
      node_ptr new_node{std::make_unique<node_type>()};

      auto new_node_ptr = new_node.get();

      p_parent->add_edge(p_edge_pos, p_label, std::move(new_node));

      return new_node_ptr;
    }

    template<typename InputSpanType>
    constexpr node_type * add_empty_nodes(const InputSpanType label)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "trie::add_empty_nodes(): InputSpanType is not a yy_quad::span<>");

      node_type * node{m_root.get()};
      size_type node_pos = 0;
      auto next_node_do = [&node, &node_pos](auto p_edge_node,
                                             size_type p_pos ) {
        node = p_edge_node->get();
        node_pos = p_pos;
      };

      for(auto label_range{yy_util::make_range(label.begin(), std::prev(label.end()))};
          label_const_l_value_ref label_part: label_range)
      {
        pos_found_type child{node->find_edge(next_node_do, label_part)};
        if(!child.found)
        {
          node = add_node(node, child.pos, label_part);
        }
      }

      return node;
    }

    template<typename InputSpanType,
             typename InputValueType>
    constexpr void add_span(InputSpanType label,
                            InputValueType && value)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "trie::add_span(): InputSpanType is not a yy_quad::span<>");

      if(!label.empty())
      {
        node_type * parent_node{add_empty_nodes(label)};
        node_type * child_node = nullptr;

        auto do_find_edge = [&child_node]
                            (auto p_edge_node, size_type) {
          child_node = p_edge_node->get();
        };

        auto last = label.back();

        pos_found_type child{parent_node->find_edge(do_find_edge, last)};
        if(child.found)
        {
          if(!child_node->empty())
          {
            child_node->value() = std::forward<InputValueType>(value);
            return;
          }

          auto new_node = yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value)));
          parent_node->swap_edge(child.pos, std::move(new_node));

          return;
        }

        node_ptr payload{
          yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value)))};
        parent_node->add_edge(child.pos, last, std::move(payload));
      }
    }

    root_node_ptr m_root{};
};

} // namespace yafiyogi::yy_data
