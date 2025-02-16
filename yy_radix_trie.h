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

#include <algorithm>
#include <memory>
#include <stdexcept>

#include "yy_assert.h"
#include "yy_find_iter_util.hpp"
#include "yy_span.h"
#include "yy_type_traits.h"
#include "yy_utility.h"
#include "yy_vector.h"

namespace yafiyogi::yy_data {
namespace radix_trie_detail {

template<typename LabelElemType,
         typename ValueType>
struct trie_node_edge;

template<typename LabelElemType,
         typename ValueType>
class trie_node;

template<typename LabelElemType,
         typename ValueType>
struct found_value;

template<typename LabelElemType,
         typename ValueType>
struct trie_node_traits final
{
    using label_type = yy_quad::simple_vector<yy_traits::remove_cvr_t<LabelElemType>>;
    using label_span_type = typename yy_quad::span_traits_helper<label_type>::const_span_type;
    using value_type = ValueType;
    using node_type = trie_node<LabelElemType, value_type>;
    using node_ptr = std::unique_ptr<node_type>;
    using root_node_ptr = std::shared_ptr<node_type>;
    using node_edge = trie_node_edge<LabelElemType, value_type>;
    using edges_type = yy_quad::simple_vector<node_edge>;
    using edges_iterator = edges_type::iterator;
    using edge_traits = find_util_detail::iter_traits_type<typename edges_type::value_type, typename edges_type::iterator>;
    using edge_ptr = typename edge_traits::key_ptr;
    using found_value_type = found_value<LabelElemType, ValueType>;
};

template<typename LabelElemType,
         typename ValueType>
struct found_value final
{
    using traits = trie_node_traits<LabelElemType, ValueType>;
    using edges_iterator = typename traits::edges_iterator;

    edges_iterator iter{};
    size_type common = 0;
    size_type remaining = 0;
};

template<typename LabelElemType,
         typename ValueType>
struct trie_node_edge final
{
  public:
    using traits = trie_node_traits<LabelElemType, ValueType>;
    using label_type = typename traits::label_type;
    using node_ptr = typename traits::node_ptr;

    constexpr explicit trie_node_edge(const label_type & p_label,
                                      const node_ptr & p_node) noexcept:
      m_label(p_label),
      m_node(p_node)
    {
    }

    constexpr explicit trie_node_edge(label_type && p_label,
                                      node_ptr && p_node) noexcept:
      m_label(std::move(p_label)),
      m_node(std::move(p_node))
    {
    }

    constexpr trie_node_edge() noexcept = default;
    constexpr trie_node_edge(const trie_node_edge & edge) noexcept = default;
    constexpr trie_node_edge(trie_node_edge && edge) noexcept = default;

    constexpr trie_node_edge & operator=(const trie_node_edge & node) noexcept = default;
    constexpr trie_node_edge & operator=(trie_node_edge && node) noexcept = default;

    [[nodiscard]]
    constexpr explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_node);
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

    yy_quad::simple_vector<LabelElemType> m_label{};
    node_ptr m_node = nullptr;
};

template<typename LabelElemType,
         typename ValueType>
class trie_node
{
  public:
    using traits = trie_node_traits<LabelElemType, ValueType>;
    using label_type = typename traits::label_type;
    using node_ptr = typename traits::node_ptr;
    using node_edge = typename traits::node_edge;
    using value_type = typename traits::value_type;
    using edges_type = typename traits::edges_type;
    using edges_iterator = typename traits::edges_iterator;
    using edge_ptr = typename traits::edge_ptr;
    using found_value_type = typename traits::found_value_type;

    constexpr trie_node() noexcept = default;
    constexpr trie_node(const trie_node & node) noexcept = default;
    constexpr trie_node(trie_node && node) noexcept = default;
    constexpr virtual ~trie_node() noexcept = default;

    constexpr trie_node & operator=(const trie_node & node) noexcept = default;
    constexpr trie_node & operator=(trie_node && node) noexcept = default;

    constexpr void add_edge(label_type && label,
                            node_ptr && node)
    {
      auto iter = std::lower_bound(m_edges.begin(), m_edges.end(), label[0]);

      add_edge(iter,
               std::forward<label_type>(label),
               std::forward<node_ptr>(node));
    }

    constexpr void add_edge(edges_iterator iter,
                            label_type && label,
                            node_ptr && node)
    {
      m_edges.emplace(iter, std::move(label), std::move(node));
    }

    template<typename InputLabelType>
    [[nodiscard]]
    constexpr found_value_type find(InputLabelType label_target) noexcept
    {
      static_assert(yy_traits::is_span_v<InputLabelType>,
                    "trie_node::find(): InputLabelType is not a yy_quad::span<>");

      auto edges_end{m_edges.end()};
      auto edge_iter{std::lower_bound(m_edges.begin(),
                                      edges_end,
                                      label_target[0])};

      if(edges_end != edge_iter)
      {
        auto label_edge{yy_quad::make_const_span(edge_iter->m_label)};

        auto [target_first, edge_first] = std::mismatch(label_target.begin(),
                                                        label_target.end(),
                                                        label_edge.begin(),
                                                        label_edge.end());

        const auto common_size = static_cast<size_type>(std::distance(label_target.begin(),
                                                                      target_first));

        return found_value_type{edge_iter,
                                common_size,
                                label_edge.size() - common_size};
      }

      return found_value_type{edges_end, 0, label_target.size()};
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

    constexpr void swap(trie_node & other) noexcept
    {
      m_edges.swap(other.m_edges);
    }

  private:
    edges_type m_edges{};
};

template<typename LabelElemType,
         typename ValueType>
class Payload final:
      public trie_node<LabelElemType, ValueType>
{
  public:
    using traits = typename trie_node<LabelElemType, ValueType>::traits;
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

template<typename LabelElemType,
         typename ValueType>
class Automaton final
{
  public:
    using traits = trie_node_traits<LabelElemType, ValueType>;
    using label_type = typename traits::label_type;
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

  private:
    template<typename InputSpanType>
    [[nodiscard]]
    constexpr bool find_span(InputSpanType label_target) noexcept
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "Automaton::find_span(): InputScanType is not a yy_quad::span<>");
      reset();

      auto node = m_state;

      while(!label_target.empty())
      {
        auto [edge_iter, common, remaining] = node->find(label_target);

        if((0 == common) || ((label_target.size() == common) && (remaining != 0)))
        {
          // Not found an edge...
          // ...or target is 'abcd' and found 'abcd', but edge is 'abcde'
          m_state = nullptr;
          return false;
        }

        node = edge_iter->m_node.get();
        label_target = label_target.subspan(common);
      }

      m_state = node;
      return has_payload();
    }

    root_node_ptr m_root{};
    node_type * m_state = nullptr;
};

} // namespace detail

template<typename LabelElemType,
         typename ValueType,
         typename Automaton = radix_trie_detail::Automaton<LabelElemType, ValueType>>
class radix_trie final
{
  public:
    using traits = typename radix_trie_detail::trie_node_traits<LabelElemType, ValueType>;
    using label_type = typename traits::label_type;
    using label_span_type = typename traits::label_span_type;
    using value_type = typename traits::value_type;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using node_edge = typename traits::node_edge;
    using value_node = typename radix_trie_detail::Payload<LabelElemType, value_type>;
    using automaton = Automaton;

    constexpr radix_trie() noexcept:
      m_root(std::make_shared<node_type>())
    {
    }

    radix_trie(const radix_trie &) = delete;
    constexpr radix_trie(radix_trie &&) noexcept = default;
    constexpr ~radix_trie() noexcept = default;

    radix_trie & operator=(const radix_trie &) = delete;
    constexpr radix_trie & operator=(radix_trie &&) noexcept = default;

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
      return automaton{m_root};
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor)
    {
      m_root->visit(visitor);
    }

  private:
    template<typename InputSpanType,
             typename InputValueType>
    constexpr void add_span(InputSpanType label,
                            InputValueType && value)
    {
      static_assert(yy_traits::is_span_v<InputSpanType>,
                    "radix_trie::add_span(): InputSpanType is not a yy_quad::span<>");

      node_type * node = m_root.get();
      while(!label.empty())
      {
        auto [edge_iter, common, remaining] = node->find(label);
        auto & edge = *edge_iter;

        if((0 == remaining) && (label.size() != common) && (0 != common))
        {
          // Searching for 'abcd' found 'ab'.
          // Go round again.
          label = label.subspan(common);
          node = edge.m_node.get();
        }
        else
        {
          if(0 == common)
          {
            // * Searching for 'abcd', but found nothing.
            node->add_edge(edge_iter,
                           label_type{label.begin(), label.end()},
                           yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value))));

            // Added everything!
            return;
          }

          if((0 != remaining) && (label.size() != common))
          {
            YY_ASSERT(0 != common);
            // * Searching for 'abef' found 'abcd'.
            // * Create new node for 'cd'
            auto & abcd_label = edge.m_label;

            label_span_type cd_span{yy_quad::make_const_span(abcd_label).subspan(common)};
            label_type cd_label{cd_span.begin(), cd_span.end()};
            node_ptr cd_node{std::move(edge.m_node)};

            // Repurpose 'abcd' edge and node for 'ab'.
            label_span_type ab_span{yy_quad::make_const_span(abcd_label).subspan(0, common)};
            edge.m_label = label_type{ab_span.begin(), ab_span.end()};
            edge.m_node = std::make_unique<node_type>();
            node_type * ab_node = edge.m_node.get();

            // Add 'cd' node to 'ab' node.
            // N.B this invatidates edge_iter & edge!
            ab_node->add_edge(std::move(cd_label),
                              std::move(cd_node));

            // Create new node for 'ef'
            auto ef_span{label.subspan(common)};

            // Add 'ef' node to 'ab' node.
            ab_node->add_edge(label_type{ef_span.begin(), ef_span.end()},
                              yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value))));

            // Added everything!
            return;
          }

          if((0 == remaining) && (label.size() == common))
          {
            YY_ASSERT(0 != common);

            // Searching for 'abcd', found 'abcd'.
            auto payload_node = edge.m_node.get();

            if(!payload_node->empty())
            {
              // Overwrite value.
              payload_node->value() = std::forward<InputValueType>(value);

              return;
            }

            // Add new payload node.
            auto new_node = yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value)));
            new_node->swap(*payload_node);
            edge.m_node = std::move(new_node);

            return;
          }

          YY_ASSERT(0 != common);

          // * Searching for 'ab' found 'abcd'.
          // * Create 'cd' edge and add to 'ab' node.
          // * Use 'abcd' node for 'ab' data.
          // * Add new node for 'cd' node.
          label_span_type cd_span{yy_quad::make_const_span(edge.m_label).subspan(common)};
          label_type cd_label{cd_span.begin(), cd_span.end()};
          node_ptr cd_node{std::move(edge.m_node)};

          // * Add 'ab' data.
          // * Update 'abcd edge to be 'ab'
          auto ab_span{label.subspan(0, common)};
          node_ptr ab_node{yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(std::forward<InputValueType>(value)))};

          // Add 'cd' node to 'ab' node
          ab_node->add_edge(std::move(cd_label),
                            std::move(cd_node));


          // Repurpose existing edge for 'ab' node.
          edge.m_label = label_type{ab_span.begin(), ab_span.end()};
          edge.m_node = std::move(ab_node);

          // Added everything!
          return;
        }
      }
    }

    root_node_ptr m_root{};
};

} // namespace yafiyogi::yy_data
