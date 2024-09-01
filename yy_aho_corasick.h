/*

  MIT License

  Copyright (c) 2021-2024 Yafiyogi

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

#include <deque>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "yy_find_util.h"
#include "yy_span.h"
#include "yy_utility.h"

namespace yafiyogi::yy_data {
namespace ac_trie_detail {

template<typename LabelType,
         typename ValueType>
class trie_node;

template<typename LabelType,
         typename ValueType>
struct trie_node_edge;

template<typename LabelType,
         typename ValueType>
struct trie_node_traits final
{
    using label_type = LabelType;
    using value_type = ValueType;
    using node_type = trie_node<label_type, value_type>;
    using node_ptr = std::unique_ptr<node_type>;
    using root_node_ptr = std::shared_ptr<node_type>;
    using node_edge = trie_node_edge<label_type, value_type>;
    using node_queue = std::deque<node_type *>;
};

template<typename LabelType,
         typename ValueType>
struct trie_node_edge final
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using node_ptr = typename traits::node_ptr;

    constexpr explicit trie_node_edge(label_type p_label,
                                      node_ptr p_node) noexcept:
      m_label(std::move(p_label)),
      m_node(std::move(p_node))
    {
    }

    constexpr trie_node_edge() noexcept = default;
    constexpr trie_node_edge(const trie_node_edge & edge) noexcept = default;
    constexpr trie_node_edge(trie_node_edge && edge) noexcept = default;

    constexpr trie_node_edge & operator=(const trie_node_edge & node) noexcept = default;
    constexpr trie_node_edge & operator=(trie_node_edge && node) noexcept = default;

    constexpr explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_node);
    }

    constexpr bool operator<(const trie_node_edge & other) const noexcept
    {
      return m_label < other.m_label;
    }

    friend bool operator<(const trie_node_edge & lhs,
                          const label_type & rhs) noexcept
    {
      return lhs.m_label < rhs;
    }

    constexpr bool operator==(const trie_node_edge & other) const noexcept
    {
      return m_label == other.m_label;
    }

    friend bool operator==(const trie_node_edge & lhs,
                           const label_type & rhs) noexcept
    {
      return lhs.m_label == rhs;
    }

    label_type m_label = label_type{};
    node_ptr m_node{};
};

template<typename LabelType,
         typename ValueType>
class trie_node
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using value_type = typename traits::value_type;
    using node_type = typename traits::node_type;
    using node_edge = typename traits::node_edge;

    constexpr explicit trie_node(node_type * fail) noexcept:
      m_fail(fail)
    {
    }

    constexpr trie_node() noexcept = default;
    constexpr trie_node(const trie_node & node) noexcept = default;
    constexpr trie_node(trie_node && node) noexcept = default;
    constexpr virtual ~trie_node() noexcept = default;

    constexpr trie_node & operator=(const trie_node & node) noexcept = default;
    constexpr trie_node & operator=(trie_node && node) noexcept = default;

    constexpr node_type * add(label_type label,
                              node_type * fail)
    {
      auto [iter, found] = yy_data::do_find_raw(m_edges, label);

      if(found)
      {
        return iter->m_node.get();
      }

      auto node = std::make_unique<trie_node>(fail);
      trie_node * node_rv = node.get();

      auto pos = iter - m_edges.data();
      m_edges.emplace(m_edges.begin() + pos, node_edge{std::move(label), std::move(node)});

      return node_rv;
    }

    constexpr void add(node_edge edge)
    {
      auto [iter, found] = yy_data::do_find_raw(m_edges, edge.m_label);

      if(!found)
      {
        auto pos = iter - m_edges.data();
        m_edges.emplace(m_edges.begin() + pos, std::move(edge));
      }
      else
      {
        auto & found_edge = *iter;

        // Overwrite found edge with new edge.
        std::swap(found_edge.m_node->m_fail, edge.m_node->m_fail);
        std::swap(found_edge.m_node->m_edges, edge.m_node->m_edges);
        found_edge = std::move(edge);
      }
    }

    [[nodiscard]]
    constexpr trie_node * find(const label_type & label) const noexcept
    {
      auto [iter, found] = yy_data::do_find_raw(m_edges, label);

      if(!found)
      {
        return nullptr;
      }

      return iter->m_node.get();
    }

    [[nodiscard]]
    constexpr node_type * fail() const noexcept
    {
      return m_fail;
    }

    constexpr void fail(node_type * p_fail) noexcept
    {
      m_fail = p_fail;
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor)
    {
      for(auto & edge: m_edges)
      {
        visitor(edge);
      }
    }

    [[nodiscard]]
    constexpr virtual bool empty() const noexcept
    {
      return true;
    }

    [[nodiscard]]
    constexpr virtual const value_type & value() const
    {
      throw std::runtime_error("Invalid value");
    }

    [[nodiscard]]
    constexpr virtual value_type & value()
    {
      throw std::runtime_error("Invalid value");
    }

  private:
    node_type * m_fail = nullptr;
    std::vector<node_edge> m_edges{};
};

template<typename LabelType,
         typename ValueType>
class Payload final:
      public trie_node<LabelType,
                       ValueType>
{
  public:
    using traits = typename trie_node<LabelType, ValueType>::traits;
    using node_type = typename traits::node_type;
    using label_type = typename traits::label_type;
    using value_type = typename traits::value_type;
    using node_ptr = typename traits::node_ptr;

    constexpr explicit Payload(node_type * fail,
                               const value_type & payload) noexcept:
      trie_node<LabelType, ValueType>(fail),
      m_payload(payload)
    {
    }

    constexpr explicit Payload(node_type * fail,
                               value_type && payload) noexcept:
      trie_node<LabelType, ValueType>(fail),
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
    constexpr const value_type & value() const override
    {
      return m_payload;
    }

    [[nodiscard]]
    constexpr value_type & value() override
    {
      return m_payload;
    }

  private:
    value_type m_payload{};
};

template<typename LabelType,
         typename ValueType>
class add_edges_visitor final
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using node_ptr = typename traits::node_ptr;
    using node_edge = typename traits::node_edge;
    using node_queue = typename traits::node_queue;

    constexpr explicit add_edges_visitor(node_queue & queue) noexcept :
      m_queue(queue)
    {
    }

    add_edges_visitor() = delete;
    add_edges_visitor(const add_edges_visitor &) = delete;
    add_edges_visitor(add_edges_visitor &&) = delete;
    constexpr ~add_edges_visitor() noexcept = default;

    add_edges_visitor & operator=(const add_edges_visitor &) = delete;
    add_edges_visitor & operator=(add_edges_visitor &&) = delete;

    constexpr void operator()(const node_edge & edge)
    {
      m_queue.emplace_back(edge.m_node.get());
    }

  private:
    node_queue & m_queue;
};

template<typename LabelType,
         typename ValueType>
class compile_visitor final
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using node_edge = typename traits::node_edge;
    using node_type = typename traits::node_type;
    using node_queue = typename traits::node_queue;

    constexpr explicit compile_visitor(node_queue & p_queue,
                                       node_type * p_fail,
                                       root_node_ptr p_root) :
      m_queue(p_queue),
      m_fail(p_fail),
      m_root(std::move(p_root))
    {
    }

    compile_visitor() = delete;
    compile_visitor(const compile_visitor &) = delete;
    compile_visitor(compile_visitor &&) = delete;
    constexpr ~compile_visitor() noexcept = default;

    compile_visitor & operator=(const compile_visitor &) = delete;
    compile_visitor & operator=(compile_visitor &&) = delete;

    constexpr void operator()(node_edge & edge)
    {
      m_queue.emplace_back(edge.m_node.get());

      auto state = m_fail;
      auto fail = m_root.get();

      while(fail == m_root.get())
      {
        fail = state->find(edge.m_label);
        if(!fail)
        {
          fail = m_root.get();
        }
        state = state->fail();

        if(state == m_root.get())
        {
          break;
        }
      }

      edge.m_node->fail(fail);
    }

  private:
    node_queue & m_queue;
    node_type * m_fail = nullptr;
    root_node_ptr m_root{};
};

template<typename LabelType,
         typename ValueType>
class Automaton final
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using node_type = typename traits::node_type;
    using value_type = typename traits::value_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using node_edge = typename traits::node_edge;

    constexpr explicit Automaton(root_node_ptr p_root) :
      m_root(std::move(p_root)),
      m_state(m_root.get())
    {
    }

    Automaton() = delete;
    Automaton(const Automaton &) = delete;
    constexpr Automaton(Automaton &&) noexcept = default;
    constexpr ~Automaton() noexcept = default;

    Automaton & operator=(const Automaton &) = delete;
    constexpr Automaton & operator=(Automaton &&) noexcept = default;

    template<typename T>
    [[nodiscard]]
    constexpr bool find(const T & label)
    {
      return find_span(yy_quad::make_const_span(label));
    }

    constexpr void next(const label_type p_label_part) noexcept
    {
      auto node{m_state};

      while(true)
      {
        const auto edge = node->find(p_label_part);

        if(edge)
        {
          node = edge;
          break;
        }

        if(node == m_root.get())
        {
          break;
        }
        node = node->fail();
      }

      m_state = node;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
      return (nullptr == m_state) || m_state->empty();
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor) const
    {
      if(m_state != m_root.get())
      {
        if(!m_state->empty())
        {
          visitor(m_state->value());
        }
      }
    }

    template<typename Visitor>
    constexpr void visit_all(Visitor && visitor) const
    {
      auto node = m_state;

      while(node != m_root.get())
      {
        if(node)
        {
          visitor(node->value());
        }

        node = node->fail();
      }
    }

    constexpr void reset() noexcept
    {
      m_state = m_root.get();
    }

  private:
    template<typename InputSpanType>
    [[nodiscard]]
    constexpr bool find_span(const InputSpanType label) noexcept
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "ac_trie::find_span(): InputSpanType is not a yy_quad::span<>");

      if(!label.empty())
      {
        reset();

        for(const auto & label_part: label)
        {
          next(label_part);

          if(!m_state)
          {
            return false;
          }

          if(m_root.get() == m_state)
          {
            return false;
          }

          if(!empty())
          {
            return true;
          }
        }
      }

      return false;
    }

    root_node_ptr m_root{};
    node_type * m_state = nullptr;
};

} // namespace detail

template<typename LabelType,
         typename ValueType>
class ac_trie final
{
  public:
    using traits = typename ac_trie_detail::trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using value_type = typename traits::value_type;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using node_edge = typename traits::node_edge;
    using node_queue = typename traits::node_queue;
    using value_node = ac_trie_detail::Payload<label_type, value_type>;
    using Automaton = ac_trie_detail::Automaton<label_type, value_type>;

    constexpr ac_trie() noexcept:
      m_root(std::make_shared<node_type>())
    {
      m_root->fail(m_root.get());
    }

    ac_trie(const ac_trie &) = delete;
    constexpr ac_trie(ac_trie &&) noexcept = default;
    constexpr ~ac_trie() noexcept = default;

    ac_trie & operator=(const ac_trie &) = delete;
    ac_trie & operator=(ac_trie &&) noexcept = delete;

    template<typename Container,
             typename Value>
    constexpr void add(Container && label,
                       Value && value)
    {
      add_span(yy_quad::make_const_span(label), std::forward<Value>(value));
    }

    constexpr void compile()
    {
      node_queue queue;
      m_root->visit(
        ac_trie_detail::add_edges_visitor<label_type, value_type>(queue));

      while(!queue.empty())
      {
        node_type * node = queue.front();
        queue.pop_front();

        node->visit(
          ac_trie_detail::compile_visitor<label_type, value_type>(queue,
                                                                  node->fail(),
                                                                  m_root));
      }
    }

    [[nodiscard]]
    constexpr Automaton create_automaton()
    {
      return Automaton{m_root};
    }

  private:
    [[nodiscard]]
    constexpr node_ptr add_node(node_ptr & node,
                                const label_type & label)
    {
      node_ptr new_node{std::make_shared<node_type>()};
      node->add(node_edge{label, new_node});

      return new_node;
    }

    template<typename InputSpanType>
    [[nodiscard]]
    constexpr node_type * add_empty_nodes(const InputSpanType label)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "ac_trie::add_empty_nodes(): InputSpanType is not a yy_quad::span<>");

      node_type * node = m_root.get();

      for(auto label_range{yy_util::make_range(label.begin(), std::prev(label.end()))};
          const auto & label_part: label_range)
      {
        node = node->add(label_part, m_root.get());
      }

      return node;
    }

    template<typename InputSpanType,
             typename InputValueType>
    constexpr void add_span(InputSpanType label,
                            InputValueType && value)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "ac_trie::add_span(): InputSpanType is not a yy_quad::span<>");

      if(!label.empty())
      {
        node_type * parent = add_empty_nodes(label);

        node_edge payload{
          label.back(),
          std::make_unique<value_node>(m_root.get(), std::forward<InputValueType>(value))};

        parent->add(std::move(payload));
      }
    }

    root_node_ptr m_root{};
};

} // namespace yafiyogi::yy_data
