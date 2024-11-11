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

#include <cstddef>

#include <memory>
#include <stdexcept>

#include "yy_assert.h"
#include "yy_span.h"
#include "yy_flat_map.h"
#include "yy_ref_traits.h"
#include "yy_type_traits.h"
#include "yy_utility.h"
#include "yy_vector.h"


namespace yafiyogi::yy_data {
namespace fm_radix_trie_detail {

template<typename LabelElemType,
         typename ValueType>
class trie_node;

template<typename LabelElemType,
         typename ValueType>
struct edge_found;

template<typename LabelElemType,
         typename ValueType>
struct edge_part_found;

template<typename LabelElemType,
         typename ValueType>
struct trie_node_traits final
{
    using label_type = yy_quad::simple_vector<yy_traits::remove_cvr_t<LabelElemType>>;
    using label_l_value_ref = typename yy_traits::ref_traits<label_type>::l_value_ref;
    using label_r_value_ref = typename yy_traits::ref_traits<label_type>::r_value_ref;
    using label_span_type = typename yy_quad::span_traits_helper<label_type>::const_span_type;
    using label_span_l_value_ref = typename yy_traits::ref_traits<label_span_type>::l_value_ref;
    using label_span_const_l_value_ref = typename yy_traits::ref_traits<label_span_type>::const_l_value_ref;
    using label_span_r_value_ref = typename yy_traits::ref_traits<label_span_type>::r_value_ref;
    using value_type = yy_traits::remove_cvr_t<ValueType>;
    using node_type = trie_node<LabelElemType, value_type>;
    using node_ptr = std::unique_ptr<node_type>;
    using root_node_ptr = std::shared_ptr<node_type>;
    using edges_type = flat_map<label_type, node_ptr>;
    using size_type = typename edges_type::size_type;
    using key_value_pos_type = typename edges_type::key_value_pos_type;
    using edge_found_type = edge_found<LabelElemType, ValueType>;
    using edge_part_found_type = edge_part_found<LabelElemType, ValueType>;
};

template<typename LabelElemType,
         typename ValueType>
struct edge_found final
{
    using traits = trie_node_traits<LabelElemType, ValueType>;
    using size_type = typename traits::size_type;
    using label_type = typename traits::label_type;
    using node_ptr = typename traits::node_ptr;

    label_type * edge_label = nullptr;
    node_ptr * edge_node = nullptr;
    size_type edge_pos = 0;
    size_type common = 0;
    size_type remaining =  0;
};

template<typename LabelElemType,
         typename ValueType>
struct edge_part_found final
{
    using traits = trie_node_traits<LabelElemType, ValueType>;
    using size_type = typename traits::size_type;
    using node_type = typename traits::node_type;

    node_type * edge_node = nullptr;
    size_type common = 0;
    size_type remaining = 0;
};

template<typename LabelElemType,
         typename ValueType>
class trie_node
{
  public:
    using traits = trie_node_traits<LabelElemType, ValueType>;
    using node_type = typename traits::node_type;
    using label_type = typename traits::label_type;
    using label_l_value_ref = typename traits::label_l_value_ref;
    using label_r_value_ref = typename traits::label_r_value_ref;
    using label_span_type = typename traits::label_span_type;
    using label_span_l_value_ref = typename traits::label_span_l_value_ref;
    using label_span_const_l_value_ref = typename traits::label_span_const_l_value_ref;
    using label_span_r_value_ref = typename traits::label_span_r_value_ref;
    using node_ptr = typename traits::node_ptr;
    using value_type = typename traits::value_type;
    using edges_type = typename traits::edges_type;
    using size_type = typename traits::size_type;
    using edge_found_type = typename traits::edge_found_type;
    using edge_part_found_type = typename traits::edge_part_found_type;

    constexpr trie_node() noexcept = default;
    constexpr trie_node(const trie_node & node) noexcept = default;
    constexpr trie_node(trie_node && node) noexcept = default;
    constexpr virtual ~trie_node() noexcept = default;

    constexpr trie_node & operator=(const trie_node & node) noexcept = default;
    constexpr trie_node & operator=(trie_node && node) noexcept = default;

    constexpr void add_edge(size_type pos,
                            label_r_value_ref label,
                            node_ptr && node)
    {
      std::ignore = m_edges.emplace(pos, std::move(label), std::move(node));
    }

    constexpr void add_edge(const label_r_value_ref label,
                            node_ptr && node)
    {
      auto label_span = yy_quad::make_const_span(label).subspan(0, 1);
      size_type pos = m_edges.lower_bound_pos(label_span).pos;

      std::ignore = m_edges.emplace(pos,
                                    std::move(label),
                                    std::move(node));
    }

    [[nodiscard]]
    constexpr edge_found_type find_edge(label_span_const_l_value_ref label) noexcept
    {
      edge_found_type edge{nullptr, nullptr, m_edges.size(), 0, label.size()};

      auto egde_getter = [label, &edge](label_type * key, node_ptr * node, size_type pos) {
        edge.edge_label = key;
        edge.edge_node = node;
        edge.edge_pos = pos;

        auto [target_first, edge_first] = std::mismatch(label.begin(),
                                                        label.end(),
                                                        key->begin(),
                                                        key->end());

        edge.common = static_cast<std::size_t>(target_first - label.begin());
        edge.remaining = edge.edge_label->size() - edge.common;
      };

      std::ignore = m_edges.lower_bound(egde_getter, label.subspan(0, 1));

      return edge;
    }

    [[nodiscard]]
    constexpr edge_part_found_type find_edge_part(label_span_const_l_value_ref label) noexcept
    {
      edge_part_found_type edge_part{nullptr, 0, label.size()};

      auto edge_part_getter = [label, &edge_part]
                              (const label_type * label_found,
                               node_ptr * node_found,
                               size_type /* pos */) {
        auto edge_label = yy_quad::make_const_span(*label_found);
        auto [target_first, edge_first] = std::mismatch(label.begin(),
                                                        label.end(),
                                                        edge_label.begin(),
                                                        edge_label.end());

        edge_part.edge_node = node_found->get();
        edge_part.common = static_cast<std::size_t>(target_first - label.begin());
        edge_part.remaining = edge_label.size() - edge_part.common;
      };

      std::ignore = m_edges.lower_bound(edge_part_getter,
                                        label.subspan(0, 1));

      return edge_part;
    }

    template<typename Visitor>
    constexpr void visit(Visitor && visitor)
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

    constexpr void swap(trie_node & other) noexcept
    {
      m_edges.swap(other.m_edges);
    }

  private:
    edges_type m_edges;
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
    value_type m_payload;
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

    constexpr explicit Automaton(root_node_ptr p_root) noexcept:
      m_root(std::move(p_root)),
      m_state()
    {
      reset();
    }

    constexpr Automaton() noexcept = default;
    Automaton(const Automaton &) = delete;
    constexpr Automaton(Automaton &&) noexcept = default;

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
        auto [edge_node, common, remaining] = node->find_edge_part(label_target);

        if((0 == common) || ((label_target.size() == common) && (remaining != 0)))
        {
          // Not found an edge...
          // ...or target is 'abcd' and found 'abcd', but edge is 'abcde'
          m_state = nullptr;
          return false;
        }

        node = edge_node;
        label_target.inc_begin(common);
      }

      m_state = node;
      return has_payload();
    }

    root_node_ptr m_root;
    node_type * m_state = nullptr;
};

} // namespace detail

template<typename LabelElemType,
         typename ValueType,
         typename Automaton = fm_radix_trie_detail::Automaton<LabelElemType, ValueType>>
class fm_radix_trie final
{
  public:
    using traits = typename fm_radix_trie_detail::trie_node_traits<LabelElemType, ValueType>;
    using label_type = typename traits::label_type;
    using label_span_type = typename traits::label_span_type;
    using value_type = typename traits::value_type;
    using node_type = typename traits::node_type;
    using node_ptr = typename traits::node_ptr;
    using root_node_ptr = typename traits::root_node_ptr;
    using value_node = typename fm_radix_trie_detail::Payload<LabelElemType, value_type>;
    using automaton = Automaton;

    constexpr fm_radix_trie() noexcept:
      m_root(std::make_shared<node_type>())
    {
    }

    fm_radix_trie(const fm_radix_trie &) = delete;
    constexpr fm_radix_trie(fm_radix_trie &&) noexcept = default;

    fm_radix_trie & operator=(const fm_radix_trie &) = delete;
    constexpr fm_radix_trie & operator=(fm_radix_trie &&) noexcept = default;

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
                    "fm_radix_trie::add_span(): InputSpanType is not a yy_quad::span<>");

      node_type * node = m_root.get();
      while(!label.empty())
      {
        auto [edge_label, edge_node, edge_pos, common, remaining] = node->find_edge(label);

        if((0 == remaining) && (label.size() != common) && (0 != common))
        {
          // Searching for 'abcd' found 'ab'.
          // Go round again.
          label = label.subspan(common);
          node = edge_node->get();
        }
        else
        {
          if(0 == common)
          {
            // * Searching for 'abcd', but found nothing.
            node->add_edge(edge_pos,
                           label_type{label.begin(), label.end()},
                           yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(value)));

            // Added everything!
            return;
          }

          if((0 != remaining) && (label.size() != common))
          {
            YY_ASSERT(0 != common);
            // * Searching for 'abef' found 'abcd'.
            // * Create new node for 'cd'
            auto & abcd_label = *edge_label;

            label_span_type cd_span{yy_quad::make_const_span(abcd_label).subspan(common)};
            label_type cd_label{cd_span.begin(), cd_span.end()};
            auto cd_node{std::move(*edge_node)};

            // Repurpose 'abcd' edge and node for 'ab'.
            label_span_type ab_span{yy_quad::make_const_span(abcd_label).subspan(0, common)};
            *edge_label = label_type{ab_span.begin(), ab_span.end()};
            *edge_node = std::make_unique<node_type>();
            node_type * ab_node = edge_node->get();

            // Add 'cd' node to 'ab' node.
            // N.B this invatidates edge_iter & edge!
            ab_node->add_edge(std::move(cd_label),
                              std::move(cd_node));

            // Create new node for 'ef'
            auto ef_span{label.subspan(common)};

            ab_node->add_edge(label_type{ef_span.begin(), ef_span.end()},
                              yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(value)));

            // Added everything!
            return;
          }

          if((0 == remaining) && (label.size() == common))
          {
            YY_ASSERT(0 != common);

            // Searching for 'abcd', found 'abcd'.
            auto payload_node = edge_node->get();

            if(!payload_node->empty())
            {
              // Overwrite value.
              payload_node->value() = value;

              return;
            }

            // Add new payload node.
            auto new_node = yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(value));
            new_node->swap(*payload_node);
            *edge_node = std::move(new_node);

            return;
          }

          YY_ASSERT(0 != common);

          // * Searching for 'ab' found 'abcd'.
          // * Create 'cd' edge and add to 'ab' node.
          // * Use 'abcd' node for 'ab' data.
          // * Add new node for 'cd' node.
          label_span_type cd_span{yy_quad::make_const_span(*edge_label).subspan(common)};
          label_type cd_label{cd_span.begin(), cd_span.end()};
          node_ptr cd_node{std::move(*edge_node)};

          // * Add 'ab' data.
          // * Update 'abcd edge to be 'ab'
          auto ab_span{label.subspan(0, common)};
          label_type ab_label{ab_span.begin(), ab_span.end()};
          node_ptr ab_node{yy_util::static_unique_cast<node_type>(std::make_unique<value_node>(value))};

          // Add 'cd' node to 'ab' node
          ab_node->add_edge(std::move(cd_label),
                            std::move(cd_node));

          // Repurpose existing edge for 'ab' node.
          *edge_label = std::move(ab_label);
          *edge_node = std::move(ab_node);

          // Added everything!
          return;
        }
      }
    }

    root_node_ptr m_root;
};

} // namespace yafiyogi::yy_data
