#pragma once

#include <algorithm>

#include "GeometryTraits.hpp"
#include "Global.hpp"
#include "Point.hpp"

namespace rtree {
  // bounding box representation
  template <typename PointType>
  struct aabb_t {
    PointType min_, max_;

    aabb_t(PointType const& p): min_(p), max_(p) {}
    aabb_t(PointType const& min__, PointType const& max__): min_(min__), max_(max__){}
  };
  // implementation of geometry_traits for aabb_t type on one dimension
  template <typename ArithmeticType>
  struct geometry_traits<aabb_t<ArithmeticType>> {
    using area_type = ArithmeticType;
    using AABB = aabb_t<ArithmeticType>;
    // check if a point is inside the bounding box
    template <typename PointType>
    static bool is_inside(AABB const& aabb, PointType const& p){
      return aabb.min_ <= p && p <= aabb.max_;
    }
    // check if a bounding box is inside another bounding box
    template <typename PointType>
    static bool is_inside(AABB const& aabb, aabb_t<PointType> const& aabb2) {
      return aabb.min_ <= aabb2.min_ && aabb2.max_ <= aabb.max_;
    }
    // check if a point is overlapping with the bounding box
    template <typename PointType>
    static bool is_overlap(AABB const& aabb, PointType const& p) {
      return is_inside(aabb, p);
    }
    // check if a bounding box is overlapping with another bounding box
    template <typename PointType>
    static bool is_overlap(AABB const& aabb, aabb_t<PointType> const& aabb2) {
      if (aabb2.min_ > aabb.max_) {
        return false;
      }
      if (aabb.min_ > aabb2.max_) {
        return false;
      }
      return true;
    }
    // merge a point with the bounding box
    static AABB merge(AABB const& aabb, ArithmeticType p) {
      return { std::min(aabb.min_, p), std::max(aabb.max_, p) };
    }
    // merge two bounding boxes
    static AABB merge(AABB const& aabb, AABB const& aabb2) {
      return { std::min(aabb.min_, aabb2.min_), std::max(aabb.max_, aabb2.max_) };
    }
    // calculate the area of the bounding box
    static area_type area(AABB const& aabb) {
      return aabb.max_ - aabb.min_;
    }

    // ==================== for R-star-Tree ====================
    // dimension
    constexpr static int DIM = 1;
    static auto min_point(AABB const& bound, int axis){
      return bound.min_;
    }
    static auto max_point(AABB const& bound, int axis){
      return bound.max_;
    }
    // sum of all length of bound for all dimension
    static auto margin(AABB const& bound) {
      return bound.max_ - bound.min_;
    }
    // return the intersection of two bounding boxes
    // if they don't intersect,return an invalid bounding box with min_ = max_?
    static AABB intersection(AABB const& aabb, AABB const& aabb2) {
      const auto ret_min = std::max(aabb.min_, aabb2.min_);
      return { ret_min, std::max(ret_min, std::min(aabb.max_, aabb2.max_)) };
    }
    // ==================== for R-star-Tree ====================
    // distance between center of bounds?
    static ArithmeticType distance_center(AABB const& b1, AABB const& b2) {
      ArithmeticType dist = b1.min_ + b1.max_ - b2.min_ - b2.max_;
      return std::abs(dist);
    }
  };

  // traits for multi-dimension point
  template <typename T, unsigned int Dim>
  struct geometry_traits<aabb_t<point_t<T, Dim>>> {
    using area_type = T;
    using Point = point_t<T, Dim>;
    using AABB = aabb_t<Point>;
    // check if a point is inside the bounding box
    template <typename PointType>
    static bool is_inside(AABB const& aabb, PointType const& p) {
      return less_equal(aabb.min_, p) && less_equal(p, aabb.max_);
    }
    // check if a bounding box is inside another bounding box
    template <typename PointType>
    static bool is_inside(AABB const& aabb, aabb_t<PointType> const& aabb2){
      return less_equal(aabb.min_, aabb2.min_) && less_equal(aabb2.max_, aabb.max_);
    }
    // check if a point is overlapping with the bounding box
    template <typename PointType>
    static bool is_overlap(AABB const& aabb, PointType const& p) {
      return is_inside(aabb, p);
    }
    // check if a bounding box is overlapping with another bounding box
    template <typename PointType>
    static bool is_overlap(AABB const& aabb, aabb_t<PointType> const& aabb2) {
      if (!less_than(aabb2.min_, aabb.max_)) {
        return false;
      }
      if (!less_than(aabb.min_, aabb2.max_)) {
        return false;
      }
      return true;
    }
    // merge a point with the bounding box
    static AABB merge(AABB const& aabb, Point const& p) {
      return { min(aabb.min_, p), max(aabb.max_, p) };
    }
    // merge two bounding boxes
    static AABB merge(AABB const& aabb, AABB const& aabb2) {
      return { min(aabb.min_, aabb2.min_), max(aabb.max_, aabb2.max_) };
    }
    // calculate the area of the bounding box
    static area_type area(AABB const& aabb) {
      area_type ret = 1;
      for (unsigned int i = 0; i < Dim; ++i) {
        ret *= (aabb.max_[i] - aabb.min_[i]);
      }
      return ret;
    }
    /**
    * Return the intersection of two bounding boxes
    * @param aabb: a bounding box aabb_t<Point> object
    * @param aabb2: another bounding box aabb_t<Point> object
    * @return A new aabb_t<Point> object
    * @note If they don't intersect, return a bounding box with zero size on the dimensions that don't intersect
    */
    static AABB intersection(AABB const& aabb, AABB const& aabb2) {
      const auto ret_min = max(aabb.min_, aabb2.min_);
      return { ret_min, max(ret_min, min(aabb.max_, aabb2.max_)) };
    }

    static bool less_than(Point const& lhs, Point const& rhs) {
      for (unsigned int i = 0; i < Dim; ++i) {
        if (lhs[i] >= rhs[i]) {
          return false;
        }
      }
      return true;
    }

    static bool less_equal(Point const& lhs, Point const& rhs) {
      for (unsigned int i = 0; i < Dim; ++i) {
        if (lhs[i] > rhs[i]) {
          return false;
        }
      }
      return true;
    }
    /**
    * Return a point with the minimum value of each dimension
    * @param lhs: a Point object
    * @param rhs: another Point object
    * @return A new Point object with the minimum value of each dimension
    */
    static Point min(Point const& lhs, Point const& rhs) {
      Point ret;
      for (unsigned int i = 0; i < Dim; ++i) {
        ret[i] = std::min(lhs[i], rhs[i]);
      }
      return ret;
    }
    /**
    * Return a point with the maximum value of each dimension
    * @param lhs: a Point object
    * @param rhs: another Point object
    * @return A new Point object with the maximum value of each dimension
    */
    static Point max(Point const& lhs, Point const& rhs) {
      Point ret;
      for (unsigned int i = 0; i < Dim; ++i) {
        ret[i] = std::max(lhs[i], rhs[i]);
      }
      return ret;
    }
    // ==================== for R-star-Tree ====================
    // dimension
    constexpr static int DIM = Dim;

    static auto min_point(AABB const& bound, int axis){
      return bound.min_[axis];
    }

    static auto max_point(AABB const& bound, int axis){
      return bound.max_[axis];
    }
    // sum of all length of bound for all dimension
    static area_type margin(AABB const& bound) {
      area_type sum = 0;
      for (unsigned int i = 0; i < Dim; ++i) {
        sum += max_point(bound, i) - min_point(bound, i);
      }
      return sum;
    }

    static T distance_center(AABB const& b1, AABB const& b2) {
      T ret = 0;
      for (unsigned int i = 0; i < Dim; ++i) {
        T dist = b1.min_[i] + b1.max_[i] - b2.min_[i] - b2.max_[i];
        dist /= 2;
        ret += dist * dist;
      }
      return ret;
    }
  };
}