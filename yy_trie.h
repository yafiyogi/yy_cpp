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
#include <stdexcept>
#include <vector>

#include "yy_find_util.h"
#include "yy_span.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_utility.h"

namespace yafiyogi::yy_data {
namespace trie_detail {

template<typename LabelType,
         typename ValueType>
struct trie_node_edge;

template<typename LabelType,
         typename ValueType>
class trie_node;

template<typename LabelType,
         typename ValueType>
struct trie_node_traits final
{
    using label_type = yy_traits::remove_cvr_t<LabelType>;
    using label_l_value_ref = typename yy_traits::ref_traits<label_type>::l_value_ref;
    using label_const_l_value_ref = typename yy_traits::ref_traits<label_type>::const_l_value_ref;
    using label_r_value_ref = typename yy_traits::ref_traits<label_type>::r_value_ref;
    using value_type = yy_traits::remove_cvr_t<ValueType>;
    using node_type = trie_node<label_type, value_type>;
    using node_ptr = std::unique_ptr<node_type>;
    using root_node_ptr = std::shared_ptr<node_type>;
    using node_edge = trie_node_edge<label_type, value_type>;
    using edges_type = std::vector<node_edge>;
    using edge_traits = find_util_detail::traits_type<node_edge>;
    using edge_ptr = typename edge_traits::key_ptr;
    using found_value_type = typename edge_traits::iter_found_type;
};

template<typename LabelType,
         typename ValueType>
struct trie_node_edge final
{
  public:
    using traits = trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_const_l_value_ref = typename traits::label_const_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_ptr = typename traits::node_ptr;

    constexpr explicit trie_node_edge(label_const_l_value_ref p_label,
                                      node_ptr p_node) noexcept:
      m_label(p_label),
      m_node(std::move(p_node))
    {
    }

    constexpr trie_node_edge() noexcept = default;
    constexpr trie_node_edge(const trie_node_edge & edge) noexcept = default;
    constexpr trie_node_edge(trie_node_edge && edge) noexcept = default;
    constexpr ~trie_node_edge() noexcept = default;

    constexpr trie_node_edge & operator=(const trie_node_edge & node) noexcept = default;
    constexpr trie_node_edge & operator=(trie_node_edge && node) noexcept = default;

    [[nodiscard]]
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
    node_ptr m_node = nullptr;
};

template<typename LabelType,
         typename ValueType>
constexpr bool operator==(typename trie_node_edge<LabelType, ValueType>::label_const_l_value_ref lhs,
                          const trie_node_edge<LabelType, ValueType> & rhs) noexcept
{
  return lhs == rhs.m_label;
}

template<typename LabelType,
         typename ValueType>
constexpr bool operator<(typename trie_node_edge<LabelType, ValueType>::label_const_l_value_ref lhs,
                         const trie_node_edge<LabelType, ValueType> & rhs) noexcept
{
  return lhs < rhs.m_label;
}

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
    using node_edge = typename traits::node_edge;
    using value_type = typename traits::value_type;
    using found_value_type = typename traits::found_value_type;
    using edges_type = typename traits::edges_type;
    using edge_ptr = typename traits::edge_ptr;

    constexpr trie_node() noexcept = default;
    constexpr trie_node(const trie_node & node) noexcept = default;
    constexpr trie_node(trie_node && node) noexcept = default;
    constexpr virtual ~trie_node() noexcept = default;

    constexpr trie_node & operator=(const trie_node & node) noexcept = default;
    constexpr trie_node & operator=(trie_node && node) noexcept = default;

    constexpr void add_edge(edge_ptr iter,
                            node_edge edge)
    {
      auto pos = iter - m_edges.data();

      m_edges.emplace(m_edges.begin() + pos, std::move(edge));
    }

    [[nodiscard]]
    constexpr found_value_type find_edge(label_const_l_value_ref label) noexcept
    {
      return yy_data::do_find_raw(m_edges, label);
    }

    [[nodiscard]]
    constexpr virtual bool empty() const noexcept
    {
      return true;
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor)
    {
      for(auto & [label, node]: m_edges)
      {
        visitor(label, *node);
      }
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

    constexpr void swap(trie_node & other) noexcept
    {
      m_edges.swap(other.m_edges);
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
    using node_edge = typename traits::node_edge;

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

      auto node = m_state;

      for(label_const_l_value_ref label_part : label)
      {
        auto [edge_iter, found] = node->find_edge(label_part);

        if(!found)
        {
          m_state = nullptr;
          return false;
        }

        node= edge_iter->m_node.get();
      }

      m_state = node;

      return has_payload();
    }

  private:
    root_node_ptr m_root{};
    node_type * m_state = nullptr;
};

} // namespace detail

template<typename LabelType,
         typename ValueType,
         typename Automaton = trie_detail::Automaton<LabelType, ValueType>>
class trie final
{
  public:
    using traits = typename trie_detail::trie_node_traits<LabelType, ValueType>;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_const_l_value_ref = typename traits::label_const_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using node_edge = typename traits::node_edge;
    using edge_ptr = typename node_type::edge_ptr;
    using value_type = typename traits::value_type;
    using value_node = typename trie_detail::Payload<label_type, value_type>;
    using automaton = Automaton;

    constexpr trie() noexcept:
      m_root(std::make_shared<node_type>())
    {
    }

    trie(const trie &) = delete;
    constexpr trie(trie &&) noexcept = default;
    constexpr ~trie() noexcept = default;

    trie & operator=(const trie &) = delete;
    constexpr trie & operator=(trie &&) noexcept = default;

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
    constexpr node_type * add_node(node_type * node,
                                   edge_ptr edge_iter,
                                   const label_type & label)
    {
      node_ptr new_node{std::make_unique<node_type>()};

      auto new_node_ptr = new_node.get();

      node->add_edge(edge_iter, node_edge{label, std::move(new_node)});

      return new_node_ptr;
    }

    template<typename InputSpanType>
    constexpr node_type * add_empty_nodes(const InputSpanType label)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "trie::add_empty_nodes(): InputSpanType is not a yy_quad::span<>");

      node_type * node{m_root.get()};

      for(auto label_range{yy_util::make_range(label.begin(), std::prev(label.end()))};
          label_const_l_value_ref label_part: label_range)
      {
        auto [edge_iter, found] = node->find_edge(label_part);

        if(found)
        {
          node = edge_iter->m_node.get();
        }
        else
        {
          node = add_node(node, edge_iter, label_part);
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
        node_type * parent{add_empty_nodes(label)};

        auto last = label.back();
        auto [edge_iter, found] = parent->find_edge(last);
        if(found)
        {
          auto payload_node = edge_iter->m_node.get();
          if(!payload_node->empty())
          {
            payload_node->value() = std::forward<InputValueType>(value);
            return;
          }

          auto new_node = yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value)));
          new_node->swap(*payload_node);
          edge_iter->m_node = std::move(new_node);

          return;
        }

        node_edge payload{
          std::move(last),
          yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value)))};
        parent->add_edge(edge_iter, std::move(payload));
      }
    }

    root_node_ptr m_root{};
};

} // namespace yafiyogi::yy_data
