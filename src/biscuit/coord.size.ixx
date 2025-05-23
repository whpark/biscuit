﻿module;

#include "biscuit/config.h"
#include "biscuit/macro.h"

export module biscuit.coord.size;
import std;
import "biscuit/dependencies/eigen.hxx";
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;
import biscuit.coord.base;

export namespace biscuit {

	template < typename T, int DIMENSION, bool bROUND > requires (std::is_arithmetic_v<T>)
	using TSize = coord::TCoordBase<coord::TSize_, T, DIMENSION, bROUND>;

	using xSize2i = TSize<int, 2, false>;
	using xSize3i = TSize<int, 3, false>;
	using xSize2d = TSize<double, 2, false>;
	using xSize3d = TSize<double, 3, false>;

	static_assert(requires(xSize2d ) { xSize2d{1.0, 2.0}; });
	static_assert(requires(xSize3d ) { xSize3d{1.0, 2.0, 3.0}; });
	static_assert(sizeof(xSize2d) == sizeof(double)*2);
	static_assert(sizeof(xSize3d) == sizeof(double)*3);
	static_assert(std::is_trivially_copyable_v<xSize2d>);

	static_assert(!concepts::coord::has_point2<xSize2i>);
	static_assert(concepts::coord::has_size2<xSize2i>);
	static_assert(!concepts::coord::has_rect2<xSize2i>);
	static_assert(!concepts::coord::is_point2<xSize2i>);
	static_assert(concepts::coord::is_size2<xSize2i>);
	static_assert(!concepts::coord::is_rect2<xSize2i>);

}
