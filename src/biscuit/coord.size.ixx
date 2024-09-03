module;

export module biscuit.coord.size;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;
import biscuit.coord.base;

export namespace biscuit {

	template < typename T, int DIMENSION, bool bROUND > requires (std::is_arithmetic_v<T>)
	using TSize = coord::TCoordBase<coord::TSize_, T, DIMENSION, bROUND>;

	using sSize2i = TSize<int, 2, false>;
	using sSize3i = TSize<int, 3, false>;
	using sSize2d = TSize<double, 2, false>;
	using sSize3d = TSize<double, 3, false>;
	using xSize2i = TSize<int, 2, true>;
	using xSize3i = TSize<int, 3, true>;
	using xSize2d = TSize<double, 2, true>;
	using xSize3d = TSize<double, 3, true>;

	static_assert(requires(sSize2d ) { sSize2d{1.0, 2.0}; });
	static_assert(requires(sSize3d ) { sSize3d{1.0, 2.0, 3.0}; });
	static_assert(sizeof(sSize2d) == sizeof(double)*2);
	static_assert(sizeof(sSize3d) == sizeof(double)*3);
	static_assert(std::is_trivially_copyable_v<sSize2d>);

	static_assert(!concepts::has_point2<sSize2i>);
	static_assert(concepts::has_size2<sSize2i>);
	static_assert(!concepts::has_rect2<sSize2i>);
	static_assert(!concepts::is_point2<sSize2i>);
	static_assert(concepts::is_size2<sSize2i>);
	static_assert(!concepts::is_rect2<sSize2i>);

}
