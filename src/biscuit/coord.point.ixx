module;

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_eigen.h"

export module biscuit.coord.point;
import std;
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

	using sPoint2i = TPoint<int, 2, false>;
	using sPoint3i = TPoint<int, 3, false>;
	using sPoint4i = TPoint<int, 4, false>;
	using sPoint2d = TPoint<double, 2, false>;
	using sPoint3d = TPoint<double, 3, false>;
	using sPoint4d = TPoint<double, 4, false>;
	using sPoint2ri = TPoint<int, 2, true>;
	using sPoint3ri = TPoint<int, 3, true>;
	using sPoint4ri = TPoint<int, 4, true>;
	using sPoint2rd = TPoint<double, 2, false>;
	using sPoint3rd = TPoint<double, 3, false>;
	using sPoint4rd = TPoint<double, 4, false>;

	static_assert(requires(sPoint2d ) { sPoint2d{1.0, 2.0}; });
	static_assert(requires(sPoint3d ) { sPoint3d{1.0, 2.0, 3.0}; });
	static_assert(requires(sPoint4d ) { sPoint4d{1.0, 2.0, 3.0, 4.0}; });
	static_assert(sizeof(sPoint2d) == sizeof(double)*2);
	static_assert(sizeof(sPoint3d) == sizeof(double)*3);
	static_assert(sizeof(sPoint4d) == sizeof(double)*4);
	static_assert(std::is_trivially_copyable_v<sPoint2d>);
	static_assert(std::is_constructible_v<sPoint2d, double, double>);
	static_assert(std::is_constructible_v<sPoint2d, int, int>);
	static_assert(!std::is_constructible_v<sPoint2d, double, double, double>);
	static_assert(std::is_constructible_v<sPoint2d, double>);
	static_assert(!std::is_constructible_v<sPoint2d, int, int, int>);
	static_assert(std::is_constructible_v<sPoint2d, int>);

	static_assert(std::is_constructible_v<sPoint2i, double, double>);

	static_assert(concepts::coord::has_point2<sPoint2i>);

}
