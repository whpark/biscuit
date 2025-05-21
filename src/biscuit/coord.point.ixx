module;

#include "biscuit/config.h"
#include "biscuit/macro.h"

export module biscuit.coord.point;
import std;
import "biscuit/dependencies/eigen.hxx";
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;
import biscuit.coord.base;

export namespace biscuit {

	template < typename T, int DIMENSION, bool bROUND > requires (std::is_arithmetic_v<T>)
	using TPoint = coord::TCoordBase<coord::TPoint_, T, DIMENSION, bROUND>;

	//template < typename tvalue, int DIMENSION, bool bROUND >
	//	requires (std::is_arithmetic_v<tvalue>)
	//struct TPoint : coord::TCoordBase<coord::TPoint_, tvalue, DIMENSION, bROUND> {
	//	using base_t = coord::TCoordBase<coord::TPoint_, tvalue, DIMENSION, bROUND>;
	//	using this_t = TPoint<tvalue, DIMENSION, bROUND>;
	//	using coord_type_t = base_t::coord_type_t;
	//	using array_t = base_t::array_t;
	//	using value_t = base_t::value_t;

	//	using base_t::base_t;
	//	using base_t::operator =;
	//	//using base_t::operator <=>;

	//	using base_t::operator +=;
	//	using base_t::operator -=;
	//	using base_t::operator *=;
	//	using base_t::operator /=;
	//	using base_t::operator +;
	//	using base_t::operator -;
	//	using base_t::operator *;

	//public:
	//	using base_t::dim;
	//	using base_t::count;
	//	using base_t::arr;
	//	using base_t::x;
	//	using base_t::y;
	//	//using base_t::z requires (base_t::dim >= 3);
	//	// Math


	//};

	using xPoint2i = TPoint<int, 2, false>;
	using xPoint3i = TPoint<int, 3, false>;
	using xPoint4i = TPoint<int, 4, false>;
	using xPoint2d = TPoint<double, 2, false>;
	using xPoint3d = TPoint<double, 3, false>;
	using xPoint4d = TPoint<double, 4, false>;
	using xPoint2ri = TPoint<int, 2, true>;
	using xPoint3ri = TPoint<int, 3, true>;
	using xPoint4ri = TPoint<int, 4, true>;
	using xPoint2rd = TPoint<double, 2, false>;
	using xPoint3rd = TPoint<double, 3, false>;
	using xPoint4rd = TPoint<double, 4, false>;

	static_assert(requires(xPoint2d ) { xPoint2d{1.0, 2.0}; });
	static_assert(requires(xPoint3d ) { xPoint3d{1.0, 2.0, 3.0}; });
	static_assert(requires(xPoint4d ) { xPoint4d{1.0, 2.0, 3.0, 4.0}; });
	static_assert(sizeof(xPoint2d) == sizeof(double)*2);
	static_assert(sizeof(xPoint3d) == sizeof(double)*3);
	static_assert(sizeof(xPoint4d) == sizeof(double)*4);
	static_assert(std::is_trivially_copyable_v<xPoint2d>);
	static_assert(std::is_constructible_v<xPoint2d, double, double>);
	static_assert(std::is_constructible_v<xPoint2d, int, int>);
	static_assert(!std::is_constructible_v<xPoint2d, double, double, double>);
	static_assert(std::is_constructible_v<xPoint2d, double>);
	static_assert(!std::is_constructible_v<xPoint2d, int, int, int>);
	static_assert(std::is_constructible_v<xPoint2d, int>);

	static_assert(std::is_constructible_v<xPoint2i, double, double>);

	static_assert(concepts::coord::has_point2<xPoint2i>);

}
