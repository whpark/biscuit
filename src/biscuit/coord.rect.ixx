module;

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_eigen.h"

export module biscuit.coord.rect;
import std;
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
		using coord_type_t = base_t::coord_type_t;
		using array_t = base_t::array_t;
		using value_t = base_t::value_t;

		using coord_point_t = TPoint<T, DIMENSION, bROUND>;
		using coord_size_t = TSize<T, DIMENSION, bROUND>;
		using base_t::dim;

		constexpr static inline value_t default_depth() requires (dim >= 3) { return 1; }

		using base_t::base_t;
		TRect(coord_point_t pt0, coord_point_t pt1) {
			this->pt() = pt0;
			this->size() = pt1 - pt0;
		}
		TRect(coord_point_t pt, coord_size_t size) {
			this->pt() = pt;
			this->size() = size;
		}
		TRect(coord_size_t size) {
			this->pt() = {};
			this->size() = size;
		}

		using base_t::operator =;
		//using base_t::operator <=>;
		BSC__NODISCARD bool operator == (this_t const&) const = default;
		BSC__NODISCARD bool operator != (this_t const&) const = default;

		BSC__NODISCARD constexpr inline coord_point_t& pt()					{ return  *reinterpret_cast<coord_point_t*>(this); }
		BSC__NODISCARD constexpr inline coord_point_t const& pt() const		{ return  *reinterpret_cast<coord_point_t const*>(this); }
		BSC__NODISCARD constexpr inline coord_size_t& size()				{ return *(reinterpret_cast<coord_size_t*>(this)+1); }
		BSC__NODISCARD constexpr inline coord_size_t const& size() const	{ return *(reinterpret_cast<coord_size_t const*>(this)+1); }
 
		BSC__NODISCARD constexpr inline coord_point_t pt0() const			{ return pt(); }
		BSC__NODISCARD constexpr inline coord_point_t pt1() const			{ return pt()+size(); }

		friend auto min(this_t const&, this_t const&) = delete;
		friend auto max(this_t const&, this_t const&) = delete;
		friend auto min(std::initializer_list<this_t>) = delete;
		friend auto min(std::initializer_list<this_t>) = delete;

		using base_t::operator +=;
		using base_t::operator -=;
		using base_t::operator *=;
		using base_t::operator /=;
		using base_t::operator +;
		using base_t::operator -;
		using base_t::operator *;
		using base_t::operator /;


	public:
		auto CenterPoint() const { return pt()+size()/2; }

		// returns true if rectangle has no area
		bool IsEmpty() const {
			if constexpr (dim == 2) {
				return (this->width <= 0) || (this->height <= 0);
			} else if constexpr (dim == 3) {
				return (this->width <= 0) || (this->height <= 0) || (this->depth <= 0);
			}
			else {
				static_assert(false);
			}
		}
		bool Is1dNotEmptyXY() const {
			return this->width > 0 or this->height > 0;
		}
		// returns true if rectangle is at (0,0,0) and has no area
		bool IsNull() const {
			return (pt() == coord_point_t{}) and (size() == coord_size_t{});
		}

		/// @brief Valid for normalized rect
		/// @return true if pt is in *this
		template < concepts::coord::is_point_ tpoint >
		bool Contains(tpoint const& pt) const {
			return (pt0() <= pt) and (pt < pt1());
		}

		/// @brief Valid for normalized rect
		/// @return return B is included in *this
		bool Contains(this_t const& B) const {
			return (pt0() <= B.pt0()) and (B.pt1() <= pt1());
		}

		// set rectangle from left, y, width, and height
		void Set(T x = 0, T y = 0, T width = 0, T height = 0) requires (dim == 2) {
			this->x = x; this->y = y; this->width = width; this->height = height;
		}
		void Set(T x = 0, T y = 0, T z = 0, T width = 0, T height = 0, T depth = default_depth()) requires (dim == 3) {
			this->x = x; this->y = y; this->z = z; this->width = width; this->height = height; this->depth = depth;
		}

		void Set(concepts::coord::is_point_ auto const& pt, concepts::coord::is_size_ auto const& size)		{ pt() = pt; size() = size; }
		void Set(concepts::coord::is_point_ auto const& pt0, concepts::coord::is_point_ auto const& pt1)	{ pt() = pt; size() = (pt1-pt0); }

		// inflate the rectangle's width, height and depth
		this_t& Inflate(T x, T y)						requires (dim == 2) { this->x -= x; this->y -= y; this->width += x+x; this->height += y+y; return *this; }
		this_t& Inflate(T x, T y, T z)					requires (dim == 3) { this->x -= x; this->y -= y; this->z -= z; this->width += x+x; this->height += y+y; this->depth += z+z; return *this; }
		this_t& Inflate(coord_size_t const& size)							{ pt() -= size; size() += size+size; return *this; }
		this_t& Inflate(this_t const& rect)									{ pt() -= rect.pt(); size() += rect.pt() + rect.size(); return *this; }
		this_t& Inflate(T l, T t, T r, T b)				requires (dim == 2) { this->x -= l; this->y -= t; this->width += l+r; this->height += t+b; return *this; }
		this_t& Inflate(T l, T t, T f, T r, T b, T bk)	requires (dim == 3) { this->x -= l; this->y -= t; this->z -= f; this->width += l+r; this->height += t+b; this->depth += f+bk; return *this; }

		// deflate the rectangle's width, height and depth
		this_t& Deflate(T x, T y)						requires (dim == 2) { this->x += x; this->y += y; this->width -= x+x; this->height -= y+y; return *this; }
		this_t& Deflate(T x, T y, T z)					requires (dim == 3) { this->x += x; this->y += y; this->z += z; this->width -= x+x; this->height -= y+y; this->depth -= z+z; return *this; }
		this_t& Deflate(coord_size_t const& size)							{ pt() += size; size() -= size+size; return *this; }
		this_t& Deflate(this_t const& rect)									{ pt() += rect.pt(); size() -= rect.pt() + rect.size(); return *this; }
		this_t& Deflate(T l, T t, T r, T b)				requires (dim == 2) { this->x += l; this->y += t; this->width -= l+r; this->height -= t+b; return *this; }
		this_t& Deflate(T l, T t, T f, T r, T b, T bk)	requires (dim == 3) { this->x += l; this->y += t; this->z += f; this->width -= l+r; this->height -= t+b; this->depth -= f+bk; return *this; }

		void Normalize() {
			if (this->width < 0) {
				this->x += this->width;
				this->width = -this->width;
			}
			if (this->height < 0) {
				this->y += this->height;
				this->height = -this->height;
			}
			if constexpr (dim >= 3) {
				if (this->depth < 0) {
					this->z += this->depth;
					this->depth = -this->depth;
				}
			}
		}
		BSC__NODISCARD this_t GetNormalized() const {
			this_t rect(*this);
			rect.Normalize();
			return rect;
		}
		BSC__NODISCARD bool IsNormalized() const {
			if constexpr (dim == 2) {
				return this->width >= 0 and this->height >= 0;
			}
			else if constexpr (dim >= 3) {
				return this->width >= 0 and this->height >= 0 and this->depth >= 0;
			}
		}

		/// @brief fast intersect (without normalization)
		this_t& Intersect(this_t const& b){
			size() = min(pt1(), b.pt1());
			pt0() = max(pt0(), b.pt0());
			size() -= pt0();
			return *this;
		}
		/// @brief fast union (without normalization)
		this_t& Union(this_t const& b){
			size() = max(pt1(), b.pt1());
			pt0() = min(pt0(), b.pt0());
			size() -= pt0();
			return *this;
		}

		//!!! Normalizing can cause an empty rect to turn falsely non-empty one. !!!///

		///// @brief safe intersect
		//this_t& IntersectSafe(this_t const& b){
		//	auto pta0 = min(pt0(), pt0()+size());
		//	auto pta1 = max(pt0(), pt0()+size());
		//	auto ptb0 = min(b.pt0(), b.pt0()+b.size());
		//	auto ptb1 = max(b.pt0(), b.pt0()+b.size());

		//	pt0() = max(pta0, ptb0);
		//	size() = min(pta1, ptb1) - pt0();
		//	return *this;
		//}

		///// @brief safe union
		//this_t& UnionSafe(this_t const& b){
		//	auto pta0 = min(pt0(), pt0()+size());
		//	auto pta1 = max(pt0(), pt0()+size());
		//	auto ptb0 = min(b.pt0(), b.pt0()+b.size());
		//	auto ptb1 = max(b.pt0(), b.pt0()+b.size());

		//	pt0() = min(pta0, ptb0);
		//	size() = max(pta1, ptb1) - pt0();
		//	return *this;
		//}

		this_t& operator &= (this_t const& b) { return Intersect(); }
		this_t& operator |= (this_t const& b) { return Union(); }

		constexpr this_t operator & (this_t const& b) { return this_t(*this) &= b; }
		constexpr this_t operator | (this_t const& b) { return this_t(*this) |= b; }

		//-----------------------------------------------------------------------------
		// ROI
		//
		[[nodiscard]] bool IsROI_Valid(coord_size_t const& sizeImage) const {
			return !this_t(sizeImage).IntersectSafe(*this).IsEmpty();
		}

	};

	using sRect2i = TRect<int, 2, false>;
	using sRect3i = TRect<int, 3, false>;
	using sRect2d = TRect<double, 2, false>;
	using sRect3d = TRect<double, 3, false>;
	using sRect2ri = TRect<int, 2, true>;
	using sRect3ri = TRect<int, 3, true>;
	using sRect2rd = TRect<double, 2, true>;
	using sRect3rd = TRect<double, 3, true>;

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
