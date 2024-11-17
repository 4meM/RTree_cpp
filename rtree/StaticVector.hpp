#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "Global.hpp"

  namespace rtree {
  /*
   * static_vector is a fixed-size vector container.
   * T element type
   * N number of elements
   * No Copy or Move Assignment defined since this would only be used in
   * child-node container
   */
  template <typename T, size_type N>
  class static_vector {
  public:
    using value_type = T;
    using size_type = ::rtree::size_type;
  protected:
    alignas(T) char data_[sizeof(T) * N]; // aligned storage for T type
    size_type size_;
  public:
    static_vector(): size_(0){}
    // copy constructor
    static_vector(static_vector const& rhs): size_(0) {
      for (size_type i = 0; i < rhs.size(); ++i) {
        push_back(rhs[i]);
      }
    }
    /**
     * move constructor
     * @note that rhs.size() is not changed.
     */
    static_vector(static_vector&& rhs): size_(0) {
      for (size_type i = 0; i < rhs.size(); ++i) {
        push_back(std::move(rhs[i]));
      }
    }

    /**
     * copy assignment
     * @note clear() is called before copying elements, which calls destructor of elements
     */
    static_vector& operator=(static_vector const& rhs) {
      clear();
      for (size_type i = 0; i < rhs.size(); ++i){
        push_back(rhs[i]);
      }
      return *this;
    }
    /**
     * move assignment
     * @note clear() is called before copying elements, which calls destructor of elements
     */
    static_vector& operator=(static_vector&& rhs) {
      clear();
      for (size_type i = 0; i < rhs.size(); ++i) {
        push_back(std::move(rhs[i]));
      }
      return *this;
    }
    /** destructor
     * @note calls clear() to call destructor of elements
     */
    ~static_vector() {
      clear();
    }
    /**
     * access to element at index i
     * @param i index
     * @return reference to element at index i
     * @note no bound check
     */
    value_type& at(size_type i) {
      return reinterpret_cast<value_type*>(data_)[i];
    }
    value_type const& at(size_type i) const {
      return reinterpret_cast<value_type const*>(data_)[i];
    }
    /**
     * access to element at index i
     * @param i index
     * @return reference to element at index i
     */
    value_type& operator[](size_type i) {
      assert(i < size());
      return at(i);
    }
    value_type const& operator[](size_type i) const {
      assert(i < size());
      return at(i);
    }
    void clear() {
      for (size_type i = 0; i < size_; ++i) {
        at(i).~value_type(); // call destructor explicitly
      }
      size_ = 0;
    }

    value_type* data() {
      return reinterpret_cast<value_type*>(data_);
    }
    value_type const* data() const {
      return reinterpret_cast<value_type const*>(data_);
    }
    size_type size() const {
      return size_;
    }
    size_type capacity() const {
      return N;
    }
    bool empty() const {
      return size_ == 0;
    }

    value_type& front() {
      assert(size_ > 0);
      return at(0);
    }
    value_type const& front() const {
      assert(size_ > 0);
      return at(0);
    }
    value_type& back() {
      assert(size_ > 0);
      return at(size_ - 1);
    }
    value_type const& back() const {
      assert(size_ > 0);
      return at(size_ - 1);
    }
    void push_back(value_type value) {
      assert(size_ < N);
      new (&at(size_)) value_type(std::move(value));// placement new operator to construct at the end of data_
      ++size_;
    }
    /**
     * @brief emplace_back constructs element in-place at the end of data_
     * @tparam Ts types of arguments
     * @param args arguments
     */
    template <typename... Ts>
    void emplace_back(Ts&&... args) {
      assert(size_ < N);
      new (&at(size_)) value_type(std::forward<Ts>(args)...);// forward arguments to construct at the end of data_
      ++size_;
    }
    /** pop_back removes the last element
     * @note destructor of the last element is called
     */
    void pop_back() {
      assert(size_ > 0);
      back().~value_type();
      --size_;
    }

    value_type* begin() {
      return data();
    }
    value_type const* begin() const {
      return data();
    }
    value_type* end()
    {
      return data() + size();
    }
    value_type const* end() const
    {
      return data() + size();
    }
  };
}