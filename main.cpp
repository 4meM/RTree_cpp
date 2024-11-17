#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <memory>
#include <random>

#include "rtree/RTree.hpp"
#include "rtree/AABB.hpp"

using namespace std;

void insertTest() {
  using point_type = rtree::point_t<double, 3>;
  using aabb_type = rtree::aabb_t<point_type>;
  using rtree_type = rtree::RTree<aabb_type, point_type, int, 2, 4>;

  std::mt19937 mt_engine { std::random_device {}() };

  // normal distribution random generator for each dimension
  // mu = 0, sigma = 5
  std::normal_distribution<double> normal_distribute(0, 5);

    /*
  if (argc < 2)
  {
    std::cerr << "Invalid Arguments:\n";
    std::cerr << argv[0] << " (Number of Points)\n";
    return 0;
  }
  */

  int N = 3;

  rtree_type rtree;
  for (int i = 0; i < N; ++i)
  {
    double x, y, z;
    x = normal_distribute(mt_engine);
    y = normal_distribute(mt_engine);
    z = normal_distribute(mt_engine);

    const double epsilon = 1e-6;
    point_type point = { x, y, z };

    rtree.insert({ point, i + 1 });
  }

  auto lamb = [](rtree_type& rtree) {
    // print tree structures to stdout
    std::ostream& output = std::cout;

    output << rtree.leaf_level() << "\n";

    for (int level = 0; level < rtree.leaf_level(); ++level)
    {
      int count = 0;
      for (auto ni = rtree.begin(level); ni != rtree.end(level); ++ni)
      {
        rtree_type::node_type* node = *ni;
        count += node->size();
      }
      output << count;

      for (auto ni = rtree.begin(level); ni != rtree.end(level); ++ni)
      {
        rtree_type::node_type* node = *ni;

        for (rtree_type::node_type::value_type& c : *node)
        {
          output << " " << c.first.min_[0] << " " << c.first.min_[1] << " "
                 << c.first.min_[2];
          output << " " << c.first.max_[0] << " " << c.first.max_[1] << " "
                 << c.first.max_[2];
        }
      }
      output << "\n";
    }
    int count = 0;
    for (auto ni = rtree.leaf_begin(); ni != rtree.leaf_end(); ++ni)
    {
      rtree_type::leaf_type* leaf = *ni;
      count += leaf->size();
    }
    output << count;
    for (auto ni = rtree.leaf_begin(); ni != rtree.leaf_end(); ++ni)
    {
      rtree_type::leaf_type* leaf = *ni;
      for (rtree_type::leaf_type::value_type& c : *leaf)
      {
        output << " " << c.first[0] << " " << c.first[1] << " " << c.first[2];
      }
    }
    output << "\n";
  };

  while(true) {
    //ask for a number input to call lamb, insert a point or finish
    int n;
    cout << "Enter a number to call lamb, insert a point or finish: ";
    cin >> n;
    if (n == 0) {
      break;
    } else if (n == 1) {
      double x, y, z;
      x = normal_distribute(mt_engine);
      y = normal_distribute(mt_engine);
      z = normal_distribute(mt_engine);

      const double epsilon = 1e-6;
      point_type point = { x, y, z };

      rtree.insert({ point, N + 1 });
      N++;
    } else {
      lamb(rtree);
    }
  }
}

void deleteTest() {
  using rtree_type = rtree::RTree<rtree::aabb_t<int>, rtree::aabb_t<int>, int>;
  using traits = rtree_type::traits;
  using bound_type = rtree_type::geometry_type;
  using node_type = rtree_type::node_type;

  std::mt19937 mt(std::random_device {}());
  std::uniform_int_distribution<int> dist(-1000, 1000);
  rtree_type rtree;
  rtree.reinsert_nodes(3);

  for (int i = 0; i < 1000; ++i)
  {
    int min_ = dist(mt);
    int max_ = dist(mt);
    if (max_ < min_)
    {
      std::swap(min_, max_);
    }
    rtree.insert({ { min_, max_ }, i });
  }

  std::vector<bool> data_inserted(1000, true);
  for (int i = 0; i < 1000; ++i)
  {
    const int cur_size = 1000 - i;
    int erase_index = std::uniform_int_distribution<int>(0, cur_size - 1)(mt);
    auto it = rtree.begin();
    std::advance(it, erase_index);
    data_inserted[it->second] = false;
    rtree.erase(it);

    // entries count check
    for (int level = 0; level < rtree.leaf_level(); ++level)
    {
      for (auto ni = rtree.begin(level); ni != rtree.end(level); ++ni)
      {
        if (level != 0)
        {
        }
      }
    }
    for (auto ni = rtree.leaf_begin(); ni != rtree.leaf_end(); ++ni)
    {
      if (rtree.leaf_level() != 0)
      {
      }
    }

    // entries bound check
    for (int level = 0; level < rtree.leaf_level(); ++level)
    {
      for (auto ni = rtree.begin(level); ni != rtree.end(level); ++ni)
      {
        auto* node = *ni;
        for (auto& c : *node)
        {
          auto a = c.first;
          if (level + 1 == rtree.leaf_level())
          {
            auto b = c.second->as_leaf()->calculate_bound();
          }
          else
          {
            auto b = c.second->as_node()->calculate_bound();
          }
        }
      }
    }
  }
}

void assignTest() {
  using rtree_type = rtree::RTree<rtree::aabb_t<int>, int, int>;
  using traits = rtree_type::traits;
  using bound_type = rtree_type::geometry_type;
  using node_type = rtree_type::node_type;
  std::mt19937 mt(std::random_device {}());
  std::uniform_int_distribution<int> dist(-100, 100);

  rtree_type rtree;
  rtree.reinsert_nodes(3);
  std::vector<rtree_type::value_type> original;

  for (int i = 0; i < 1000; ++i)
  {
    int pos = dist(mt);
    rtree.insert({ pos, i });
    original.push_back({ pos, i });
  }
  std::sort(original.begin(), original.end());

  rtree_type rtree_copy = rtree;
  std::vector<rtree_type::value_type> copied(rtree_copy.begin(),
                                             rtree_copy.end());
  std::sort(copied.begin(), copied.end());

  rtree_type rtree_moved = std::move(rtree);
  std::vector<rtree_type::value_type> moved(rtree_moved.begin(),
                                            rtree_moved.end());
  std::sort(moved.begin(), moved.end());
}

int main() {

  // insert test
  insertTest();
/*
  // delete test
  deleteTest();

  // assign test
  assignTest();
*/
  return 0;
}