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

#include <memory>
#include <vector>

#include "yy_assert.h"
#include "yy_span.h"
#include "yy_flat_map.h"
#include "yy_type_traits.h"
#include "yy_utility.h"

namespace yafiyogi::yy_data {
namespace fm_trie_detail {

template<typename LabelType,
         typename ValueType>
class trie_node;

template<typename LabelType,
         typename ValueType>
struct trie_node_traits final
{
    using label_type = yy_traits::remove_rcv_t<LabelType>;
    using label_l_value_ref = typename yy_traits::ref_traits<label_type>::l_value_ref;
    using label_r_value_ref = typename yy_traits::ref_traits<label_type>::r_value_ref;
    using value_type = yy_traits::remove_rcv_t<ValueType>;
    using node_type = trie_node<label_type, value_type>;
    using node_ptr = std::unique_ptr<node_type>;
    using root_node_ptr = std::shared_ptr<node_type>;
    using edges_type = flat_map<label_type, node_ptr>;
    using size_type = typename edges_type::size_type;
};

template<typename LabelType,
         typename ValueType>
class trie_node
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_ptr = typename traits::node_ptr;
    using value_type = typename traits::value_type;
    using edges_type = typename traits::edges_type;
    using size_type = typename traits::size_type;

    trie_node() noexcept = default;
    trie_node(const trie_node & node) noexcept = default;
    trie_node(trie_node && node) noexcept = default;
    virtual ~trie_node() noexcept = default;

    trie_node & operator=(const trie_node & node) noexcept = default;
    trie_node & operator=(trie_node && node) noexcept = default;

    void add_edge(size_type pos,
                  label_r_value_ref label,
                  node_ptr && node)
    {
      [[maybe_unused]]
      auto ignore = m_edges.emplace(pos, std::move(label), std::move(node));
    }

    template<typename Visitor>
    [[nodiscard]]
    auto find_edge(Visitor && visitor,
                   const label_l_value_ref label) noexcept
    {
      return m_edges.find_value(std::forward<Visitor>(visitor), label);
    }

    [[nodiscard]]
    auto find_edge_pos(const label_type & label) const noexcept
    {
      return m_edges.find_pos(label);
    }

    template<typename Visitor>
    void visit(Visitor && visitor)
    {
      for(size_t idx = 0; idx < m_edges.size(); ++idx)
      {
        auto [label, node] = m_edges[idx];

        visitor(label, node);
      }
    }

    [[nodiscard]]
    virtual bool empty() const noexcept
    {
      return true;
    }

    [[nodiscard]]
    virtual value_type & value()
    {
      throw std::runtime_error("Invalid value");
    }

    [[nodiscard]]
    virtual const value_type & value() const
    {
      throw std::runtime_error("Invalid value");
    }

    void swap(trie_node & other) noexcept
    {
      m_edges.swap(other.m_edges);
    }

  private:
    edges_type m_edges;
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

    explicit Payload(const value_type & payload) noexcept:
      m_payload(payload)
    {
    }

    explicit Payload(value_type && payload) noexcept:
      m_payload(std::move(payload))
    {
    }

    Payload() = delete;
    Payload(const Payload &) noexcept = default;
    Payload(Payload &&) noexcept = default;
    ~Payload() noexcept = default;

    Payload & operator=(const Payload &) noexcept = default;
    Payload & operator=(Payload &&) noexcept = default;

    [[nodiscard]]
    bool empty() const noexcept override
    {
      return false;
    }

    [[nodiscard]]
    value_type & value() override
    {
      return m_payload;
    }

    [[nodiscard]]
    const value_type & value() const override
    {
      return m_payload;
    }

  private:
    value_type m_payload;
};

template<typename LabelType,
         typename ValueType>
class Automaton final
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using size_type = typename traits::size_type;

    explicit Automaton(root_node_ptr p_root) noexcept:
      m_root(std::move(p_root)),
      m_state()
    {
      reset();
    }

    Automaton() noexcept = default;
    Automaton(const Automaton &) = delete;
    Automaton(Automaton &&) noexcept = default;
    ~Automaton() noexcept = default;

    Automaton & operator=(const Automaton & other) = delete;
    Automaton & operator=(Automaton && other) noexcept = default;

    template<typename InputSpanType>
    [[nodiscard]]
    bool find(InputSpanType && label)
    {
      return find_span(yy_quad::make_const_span(std::forward<InputSpanType>(label)));
    }

    void reset() noexcept
    {
      m_state = m_root.get();
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
      return nullptr == m_state;
    }

    [[nodiscard]]
    bool has_payload() const noexcept
    {
      return !empty() && !m_state->empty();
    }

    template<typename Visitor>
    void visit(Visitor && visitor) const
    {
      if(has_payload())
      {
        visitor(m_state->value());
      }
    }

  protected:
    template<typename InputSpanType>
    [[nodiscard]]
    bool find_span(const InputSpanType label) noexcept
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "fm_trie::find_span(): InputSpanType is not a yy_quad::span<>");

      reset();

      auto node = m_state;

      for(const label_l_value_ref ch : label)
      {
        if(!node->find_edge([&node](node_ptr * edge_node, size_type /* pos */) { node = edge_node->get();}, ch).found)
        {
          m_state = nullptr;
          return false;
        }
      }

      m_state = node;

      return has_payload();
    }

  private:
    root_node_ptr m_root;
    node_type * m_state = nullptr;
};

} // namespace detail

template<typename LabelType,
         typename ValueType,
         typename Automaton = fm_trie_detail::Automaton<LabelType, ValueType>>
class fm_trie
{
  public:
    using traits = typename fm_trie_detail::trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using value_type = typename traits::value_type;
    using size_type = typename traits::size_type;
    using value_node = typename fm_trie_detail::Payload<label_type, value_type>;
    using automaton = Automaton;

    fm_trie() noexcept:
      m_root(std::make_shared<node_type>())
    {
    }

    fm_trie(const fm_trie &) = delete;
    fm_trie(fm_trie &&) noexcept = default;
    ~fm_trie() noexcept = default;

    fm_trie & operator=(const fm_trie &) = delete;
    fm_trie & operator=(fm_trie &&) noexcept = default;

    template<typename InputSpanType,
             typename InputValueType>
    void add(InputSpanType && label,
             InputValueType && value)
    {
      static_assert(std::is_same_v<yy_traits::remove_rcv_t<InputValueType>,
                    yy_traits::remove_rcv_t<value_type>>,
                    "The value provided is not the correct type.");
      add_span(yy_quad::make_const_span(std::forward<InputSpanType>(label)), std::forward<InputValueType>(value));
    }

    [[nodiscard]]
    automaton create_automaton() noexcept
    {
      return automaton{m_root};
    }

    template<typename Visitor>
    void visit(Visitor && visitor)
    {
      if(m_root)
      {
        m_root->visit(visitor);
      }
    }

  private:
    [[nodiscard]]
    node_type * add_node(node_type * node,
                         size_type pos,
                         label_type label)
    {
      node_ptr new_node{std::make_unique<node_type>()};

      auto new_node_ptr = new_node.get();

      node->add_edge(pos, std::move(label), std::move(new_node));

      return new_node_ptr;
    }

    template<typename InputSpanType>
    [[nodiscard]]
    node_type * add_empty_nodes(InputSpanType label)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "fm_trie::add_empty_nodes(): InputSpanType is not a yy_quad::span<>");

      node_type * node{m_root.get()};

      label = label.subspan(0, label.size() - 1);

      // Skip exising nodes.
      while(!label.empty())
      {
        auto [ignore, found] = node->find_edge([&node]
                                               (node_ptr * edge_node, size_type /* pos */) {
          node = edge_node->get();
        }, label[0]);

        if(!found)
        {
          break;
        }

        label.inc_begin();
      }

      for(label_l_value_ref label_part : label)
      {
        auto edge_pos = node->find_edge_pos(label_part).pos;

        node = add_node(node, edge_pos, label_part);
      }

      return node;
    }

    template<typename InputSpanType,
             typename InputValueType>
    void add_span(InputSpanType label,
                  InputValueType && value)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "fm_trie::add_span(): InputSpanType is not a yy_quad::span<>");

      if(!label.empty())
      {
        node_type * parent = add_empty_nodes(label);

        auto last = label.back();
        node_ptr * edge_node = nullptr;
        auto [edge_pos, found] = parent->find_edge([&edge_node]
                                                   (node_ptr * node, size_type) {
          edge_node = node;
        }, last);

        if(found)
        {
          auto payload_node = edge_node->get();

          if(!payload_node->empty())
          {
            payload_node->value() = std::forward<InputValueType>(value);
            return;
          }

          auto new_node = yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value)));
          new_node->swap(*payload_node);
          *edge_node = std::move(new_node);

          return;
        }

        parent->add_edge(edge_pos,
                         std::move(last),
                         yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value))));
      }
    }

    root_node_ptr m_root;
};

} // namespace yafiyogi::yy_data
