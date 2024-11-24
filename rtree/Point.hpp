#pragma once

#include <algorithm>

#include "Global.hpp"

namespace rtree {
	template <typename ScalarType, unsigned int Dim>
	class point_t {
	public:
		using size_type = unsigned int;
		using scalar_type = ScalarType;
		using value_type = ScalarType;

	protected:
		scalar_type _data[Dim];

	public:
		// default constructor
		point_t() = default;
		// copy constructor
		point_t(point_t const& rhs) {
			for (size_type i = 0; i < size(); ++i) {
				_data[i] = rhs._data[i];
			}
		}
		// constructor with variadic arguments
		template <typename T0, typename... Ts>
		point_t(T0 arg0, Ts... args) {
			static_assert(sizeof...(args) + 1 == Dim, "Constructor Dimension not match");
			set<0>(arg0, args...);
		}
		// recursive variadic template function to set values to _data
		template <size_type I = 0, typename T0, typename... Ts>
		void set(T0 arg0, Ts... args){
			_data[I] = arg0;
			set<I + 1>(args...);
		}
		template <size_type I>
		void set(){}
		// probably unused function
		template <typename Iterator>
		void assign(Iterator begin, Iterator end) {
			size_type i = 0;
			while (begin != end && i < Dim) {
				_data[i++] = *begin++;
			}
		}
		// copy assignment operator
		point_t& operator=(point_t const& rhs) {
			for (size_type i = 0; i < size(); ++i) {
				_data[i] = rhs._data[i];
			}
			return *this;
		}
		// dimensionality of point
		constexpr static size_type size() {
			return Dim;
		}
		// access to _data given index: for non-const object
		scalar_type& operator[](size_type i) {
			return _data[i];
		}
		// probably unused function: for const object
		scalar_type operator[](size_type i) const {
			return _data[i];
		}
		scalar_type* data() {
			return _data;
		}
		scalar_type const* data() const {
			return _data;
		}
		scalar_type* begin(){
			return _data;
		}
		scalar_type const* begin() const{
			return _data;
		}
		scalar_type* end() {
			return _data + size();
		}
		scalar_type const* end() const {
			return _data + size();
		}

		friend std::ostream& operator<<(std::ostream& os, const point_t& point) {
			os << "(";
			for (size_type i = 0; i < Dim; ++i) {
				os << point._data[i];
				if (i < Dim - 1) {
					os << ", ";
				}
			}
			os << ")";
			return os;
		}

	};

}