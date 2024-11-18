#pragma once

#include <cassert>
#include <cmath>
#include <limits>
#include <utility>

#include "GeometryTraits.hpp"
#include "Global.hpp"

namespace rtree {

// quadratic split algorithm
template <typename TreeType>
struct quadratic_split_t {
  using geometry_type = typename TreeType::geometry_type;
  using traits = geometry_traits<geometry_type>;
  using area_type = typename geometry_traits<geometry_type>::area_type;
  constexpr static area_type LOWEST_AREA
      = std::numeric_limits<area_type>::lowest();

  constexpr static unsigned int MIN_ENTRIES = TreeType::MIN_ENTRIES;
  constexpr static unsigned int MAX_ENTRIES = TreeType::MAX_ENTRIES;

  template <typename NodeType>
  void pickSeeds(NodeType* node,
                       typename NodeType::value_type new_child,
                       NodeType* node_pair) const
  {
    int n1 = 0, n2 = 0;
    area_type max_wasted_area = LOWEST_AREA;

    for (int i = 0; i < node->size() - 1; ++i) {
      for (int j = i + 1; j < node->size(); ++j) {
        const auto J = traits::merge(node->at(i).first, node->at(j).first);
        const area_type wasted_area = traits::area(J)
                                      - traits::area(node->at(i).first)
                                      - traits::area(node->at(j).first);
        if (wasted_area > max_wasted_area) {
          max_wasted_area = wasted_area;
          n1 = i;
          n2 = j;
        }
        // if same wasted area, choose pair with small intersection area
        else if (wasted_area == max_wasted_area) {
          if (traits::area(traits::intersection(node->at(i).first, node->at(j).first))
              < traits::area(traits::intersection(node->at(n1).first, node->at(n2).first))) {
            n1 = i;
            n2 = j;
          }
        }
      }
    }

    for (int j = 0; j < node->size(); ++j) {
      const auto J = traits::merge(new_child.first, node->at(j).first);
      const area_type wasted_area = traits::area(J)
                                    - traits::area(new_child.first)
                                    - traits::area(node->at(j).first);

      if (wasted_area > max_wasted_area) {
        max_wasted_area = wasted_area;
        n1 = -1;
        n2 = j;
      }
      // if same wasted area, choose pair with small intersection area
      else if (wasted_area == max_wasted_area) {
        if (n1 == -1) {
          if (traits::area(traits::intersection(new_child.first, node->at(j).first))
              < traits::area(traits::intersection(new_child.first, node->at(n2).first))) {
            n1 = -1;
            n2 = j;
          }
        }
        else
        {
          if (traits::area(traits::intersection(new_child.first, node->at(j).first))
              < traits::area(traits::intersection(node->at(n1).first, node->at(n2).first))) {
            n1 = -1;
            n2 = j;
          }
        }
      }
    }

    if (n1 == -1) {
      node_pair->insert(std::move(new_child));
      if (n2 != 0) {
        node->swap(0, n2);
      }
    }
    else {
      // n1 is still valid after erase() since n1 < n2
      auto n2_data = std::move(node->at(n2));
      node->erase(node->begin() + n2);
      node_pair->insert(std::move(n2_data));
      if (n1 != 0) {
        node->swap(0, n1);
      }
      node->insert(std::move(new_child));
    }
  }

  template <typename NodeType>
  NodeType* operator()(NodeType* node,
                       typename NodeType::value_type new_child,
                       NodeType* node_pair) const
  {
    assert(node->size() == MAX_ENTRIES);
    assert(node_pair);
    assert(node_pair->size() == 0);
    pickSeeds(node, std::move(new_child), node_pair);

    typename NodeType::size_type count1 = 1;
    geometry_type bound1 = node->at(0).first;
    geometry_type bound2 = node_pair->at(0).first;

    while (count1 + node_pair->size() < MAX_ENTRIES + 1)
    {
      /*
      PN1. [Determine cost of putting each entry in each group.]
      For each entry E not yet in a group,
      calculate d1 = the area increase required in the covering rectangle of
      Group 1 to include E.I. Calculate d2  similarly for Group 2.

      PN2. [Find entry with greatest preference for one group.]
      Choose any entry with the maximum difference between d1 and d2.
      */
      const typename NodeType::size_type node_left
          = MAX_ENTRIES + 1 - (count1 + node_pair->size());
      if (count1 + node_left == MIN_ENTRIES)
      {
        break;
      }
      else if (node_pair->size() + node_left == MIN_ENTRIES)
      {
        for (typename NodeType::size_type i = 0; i < node_left; ++i)
        {
          auto back_data = std::move(node->back());
          node->pop_back();
          node_pair->insert(std::move(back_data));
        }
        break;
      }
      else
      {
        int picked = 0;
        int picked_to = 0;
        area_type maximum_difference = LOWEST_AREA;

        for (typename NodeType::size_type i = count1; i < node->size(); ++i)
        {
          const area_type d1
              = traits::area(traits::merge(bound1, node->at(i).first))
                - traits::area(bound1);
          const area_type d2
              = traits::area(traits::merge(bound2, node->at(i).first))
                - traits::area(bound2);
          const auto diff = std::abs(d1 - d2);

          if (diff > maximum_difference)
          {
            picked = i;
            maximum_difference = diff;
            picked_to = d1 < d2 ? 0 : 1;
          }
        }

        if (picked_to == 0)
        {
          bound1 = traits::merge(bound1, node->at(picked).first);
          if (picked != count1)
          {
            node->swap(count1, picked);
          }
          ++count1;
        }
        else
        {
          bound2 = traits::merge(bound2, node->at(picked).first);
          auto picked_data = std::move(node->at(picked));
          node->erase(node->begin() + picked);
          node_pair->insert(std::move(picked_data));
        }
      }
    }
    return node_pair;
  }
};

}