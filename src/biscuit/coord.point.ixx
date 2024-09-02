module;

export module biscuit.coord.point;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;
import biscuit.coord.base;

namespace biscuit::coord {

	template < typename T, int DIMENSION >
	using TPointD_ = std::conditional_t<DIMENSION == 2, TPoint2<T>, std::conditional_t<DIMENSION == 3, TPoint3<T>, std::conditional_t<DIMENSION == 4, TPoint4<T>, void >>>;

}

export namespace biscuit {

	template < typename T, int DIMENSION > requires (std::is_arithmetic_v<T>)
	struct TPoint : 
		public coord::TPointD_<T, DIMENSION>,
		public TCoordBase<T, DIMENSION, DIMENSION, coord::mPoint_t>
	{
		//constexpr static int const dim = DIMENSION;
		using this_t = TPoint<T, DIMENSION>;
		using base_t = TCoordBase<T, DIMENSION, DIMENSION, coord::mPoint_t >;
		using value_t = base_t::value_t;

		using base_t::base_t;

		using base_t::operator =;
		using base_t::operator [];

	};

	using sPoint2i = TPoint<int, 2>;
	using sPoint3i = TPoint<int, 3>;
	using sPoint4i = TPoint<int, 4>;
	using sPoint2d = TPoint<double, 2>;
	using sPoint3d = TPoint<double, 3>;
	using sPoint4d = TPoint<double, 4>;

	static_assert(requires(sPoint2d ) { sPoint2d(1.0, 2.0); });
	static_assert(requires(sPoint3d ) { sPoint3d(1.0, 2.0, 3.0); });
	static_assert(requires(sPoint4d ) { sPoint4d(1.0, 2.0, 3.0, 4.0); });
	static_assert(sizeof(sPoint2d) == sizeof(double)*2);
	static_assert(sizeof(sPoint3d) == sizeof(double)*3);
	static_assert(sizeof(sPoint4d) == sizeof(double)*4);
	static_assert(std::is_trivially_copyable_v<sPoint2d>);
	static_assert(std::is_constructible_v<sPoint2d, double, double>);
	static_assert(std::is_constructible_v<sPoint2d, int, int>);
	static_assert(!std::is_constructible_v<sPoint2d, double, double, double>);
	static_assert(!std::is_constructible_v<sPoint2d, double>);
	static_assert(!std::is_constructible_v<sPoint2d, int, int, int>);
	static_assert(!std::is_constructible_v<sPoint2d, int>);

	static_assert(std::is_constructible_v<sPoint2i, double, double>);
}
