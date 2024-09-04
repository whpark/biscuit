module;

export module biscuit.coord.rect;
import std;
import "biscuit/macro.h";
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;
import biscuit.coord.base;
import biscuit.coord.point;
import biscuit.coord.size;


export namespace biscuit {

	template < typename T, int DIMENSION, bool bROUND >
		requires (std::is_arithmetic_v<T>)
	struct TRect : coord::TCoordBase<coord::TRect_, T, DIMENSION, bROUND> {
		using base_t = coord::TCoordBase<coord::TRect_, T, DIMENSION, bROUND>;
		using this_t = TRect<T, DIMENSION, bROUND>;

		using point_t = TPoint<T, DIMENSION, bROUND>;
		using size_t = TSize<T, DIMENSION, bROUND>;

		using base_t::base_t;
		TRect(point_t pt0, point_t pt1) {
			pt() = pt0;
			size() = pt1 - pt0;
		}
		TRect(point_t pt, size_t size) {
			this->pt() = pt;
			this->size() = size;
		}
		using base_t::operator =;
		using base_t::operator <=>;

		BSC__NODISCARD constexpr inline auto& pt()					{ return  *reinterpret_cast<point_t*>(this); }
		BSC__NODISCARD constexpr inline auto const& pt() const		{ return  *reinterpret_cast<point_t const*>(this); }
		BSC__NODISCARD constexpr inline auto& size()				{ return *(reinterpret_cast<size_t*>(this)+1); }
		BSC__NODISCARD constexpr inline auto const& size() const	{ return *(reinterpret_cast<size_t const*>(this)+1); }


		using base_t::operator +=;
		using base_t::operator -=;
		using base_t::operator *=;
		using base_t::operator /=;
		using base_t::operator +;
		using base_t::operator -;
		using base_t::operator *;
		using base_t::operator /;

	};

	using sRect2i = TRect<int, 2, false>;
	using sRect3i = TRect<int, 3, false>;
	using sRect2d = TRect<double, 2, false>;
	using sRect3d = TRect<double, 3, false>;
	using xRect2i = TRect<int, 2, true>;
	using xRect3i = TRect<int, 3, true>;
	using xRect2d = TRect<double, 2, true>;
	using xRect3d = TRect<double, 3, true>;

	static_assert(requires(sRect2d ) { sRect2d{1.0, 2.0, 3.0, 4.0}; });
	static_assert(requires(sRect3d ) { sRect3d{1.0, 2.0, 3.0, 4.0, 5, 6}; });
	static_assert(sizeof(sRect2d) == sizeof(double)*2*2);
	static_assert(sizeof(sRect3d) == sizeof(double)*3*2);
	static_assert(std::is_trivially_copyable_v<sRect2d>);
	static_assert(concepts::coord::has_point2<sRect2i>);
	static_assert(concepts::coord::has_size2<sRect2i>);
	static_assert(concepts::coord::has_rect2<sRect2i>);
	static_assert(!concepts::coord::is_point2<sRect2i>);
	static_assert(!concepts::coord::is_size2<sRect2i>);
	static_assert(concepts::coord::is_rect2<sRect2i>);

}
