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
		using coord_type_t = base_t::coord_type_t;
		using array_t = base_t::array_t;
		using value_t = base_t::value_t;

		using point_t = TPoint<T, DIMENSION, bROUND>;
		using size_t = TSize<T, DIMENSION, bROUND>;

		constexpr static inline value_t default_depth() requires (dim >= 3) { return 1; }

		using base_t::base_t;
		TRect(point_t pt0, point_t pt1) {
			this->pt() = pt0;
			this->size() = pt1 - pt0;
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


	public:
		auto CenterPoint() const { return pt()+size()/2; }

		// returns true if rectangle has no area
		bool IsRectEmpty() const {
			if constexpr (dim == 2) {
				return (this->width <= 0) || (this->height <= 0);
			} else if constexpr (dim == 3) {
				return (this->width <= 0) || (this->height <= 0) || (this->depth <= 0);
			}
			else {
				static_assert(false);
			}
		}
		bool IsRectHavingLength2d() const {
			return (width > 0) or (height > 0);
		}
		// returns true if rectangle is at (0,0,0) and has no area
		bool IsRectNull() const {
			return (pt() == {}) and (size() == {});
		}
		template < concepts::coord::is_point_ tpoint >
		bool PtInRect(tpoint const& pt) const {
			// Normalized Rect, also, must output the same result
			if (auto r = this->x + this->width; this->x <= r) {
				if (pt.x < this->x || pt.x >= r) return false;
			} else {
				if (pt.x < r || pt.x >= this->x) return false;
			}
			if (auto b = this->y + this->height; this->y <= b) {
				if (pt.y < this->y || pt.y >= b) return false;
			} else {
				if (pt.y < b || pt.y >= this->y) return false;
			}

			if constexpr ((dim == 3) && concepts::coord::has_z<tpoint>) {
				if (auto b = this->z + this->depth; this->z <= b) {
					if (pt.z < this->z || pt.z >= b) return false;
				}
				else {
					if (pt.z < b || pt.z >= this->z) return false;
				}
			}
			return true;
		}

		// returns true if rect is within rectangle
		/// @brief *this including B
		/// @return 
		bool RectInRect(this_t const& B) const {
			for (size_t i{}; i < dim; i++) {
				if (B.pt0().member(i) < this->pt0().member(i))
					return false;
				if (B.pt1().member(i) > this->pt1().member(i))
					return false;
			}
			return true;
		}
		/// @brief *this including B (No boundary)
		/// @return 
		bool RectInRectNE(this_t const& B) const {
			for (size_t i{}; i < dim; i++) {
				if (B.pt0().member(i) <= this->pt0().member(i))
					return false;
				if (B.pt1().member(i) >= this->pt1().member(i))
					return false;
			}
			return true;
		}

		// Operations

		// set rectangle from left, y, right, and bottom
		void SetRect(T x = 0, T y = 0, T width = 0, T height = 0) requires (dim == 2) {
			this->x = x; this->y = y; this->width = width; this->height = height;
		}
		void SetRect(T x = 0, T y = 0, T z = 0, T width = 0, T height = 0, T depth = default_depth()) requires (dim == 3) {
			this->x = x; this->y = y; this->z = z; this->width = width; this->height = height; this->depth = depth;
		}

		void SetRect(concepts::coord::is_point_ auto const& pt, concepts::coord::is_size_ auto const& size)		{ pt() = pt; size() = size; }
		void SetRect(concepts::coord::is_point_ auto const& pt0, concepts::coord::is_point_ auto const& pt1)	{ pt() = pt; size() = (pt1-pt0); }

		// inflate the rectangle's width, height and depth
		this_t& InflateRect(T x, T y)						requires (dim == 2) { this->x -= x; this->y -= y; this->width += x+x; this->height += y+y; return *this; }
		this_t& InflateRect(T x, T y, T z)					requires (dim == 3) { this->x -= x; this->y -= y; this->z -= z; this->width += x+x; this->height += y+y; this->depth += z+z; return *this; }
		this_t& InflateRect(size_t const& size)									{ pt() -= size; size() += size+size; return *this; }
		this_t& InflateRect(this_t const& rect)									{ pt() -= rect.pt(); size() += rect.pt() + rect.size(); return *this; }
		this_t& InflateRect(T l, T t, T r, T b)				requires (dim == 2) { this->x -= l; this->y -= t; this->width += l+r; this->height += t+b; return *this; }
		this_t& InflateRect(T l, T t, T f, T r, T b, T bk)	requires (dim == 3) { this->x -= l; this->y -= t; this->z -= f; this->width += l+r; this->height += t+b; this->depth += f+bk; return *this; }

		// deflate the rectangle's width, height and depth
		this_t& DeflateRect(T x, T y)						requires (dim == 2) { this->x += x; this->y += y; this->width -= x+x; this->height -= y+y; return *this; }
		this_t& DeflateRect(T x, T y, T z)					requires (dim == 3) { this->x += x; this->y += y; this->z += z; this->width -= x+x; this->height -= y+y; this->depth -= z+z; return *this; }
		this_t& DeflateRect(size_t const& size)									{ pt() += size; size() -= size+size; return *this; }
		this_t& DeflateRect(this_t const& rect)									{ pt() += rect.pt(); size() -= rect.pt() + rect.size(); return *this; }
		this_t& DeflateRect(T l, T t, T r, T b)				requires (dim == 2) { this->x += l; this->y += t; this->width -= l+r; this->height -= t+b; return *this; }
		this_t& DeflateRect(T l, T t, T f, T r, T b, T bk)	requires (dim == 3) { this->x += l; this->y += t; this->z += f; this->width -= l+r; this->height -= t+b; this->depth -= f+bk; return *this; }

		void NormalizeRect() {
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
		this_t GetNormalizedRect() const {
			this_t rect(*this);
			rect.NormalizeRect();
			return rect;
		}
		bool IsNormalized() const {
			if constexpr (dim == 2) {
				return this->width >= 0 and this->height >= 0;
			}
			else if constexpr (dim >= 3) {
				return this->width >= 0 and this->height >= 0 and this->depth >= 0;
			}
		}

		// set this rectangle to intersection of two others
		this_t& IntersectRect(this_t const& b) {
			//NormalizeRect();
			//b.NormalizeRect();

			x = std::max(x, rect2.x);
			y = std::max(y, rect2.y);
			if constexpr (dim >= 3)
				z = std::max(z, rect2.z);

			pt1().x = std::min(pt1().x, rect2.pt1().x);
			pt1().y = std::min(pt1().y, rect2.pt1().y);
			if constexpr (dim >= 3)
				pt1().z = std::min(pt1().z, rect2.pt1().z);

			return *this;
		}

		// set this rectangle to bounding union of two others
		this_t& UnionRect(this_t rect2) {
			NormalizeRect();
			rect2.NormalizeRect();

			pt0().x = std::min(pt0().x, rect2.pt0().x);
			pt0().y = std::min(pt0().y, rect2.pt0().y);
			if constexpr (dim >= 3)
				pt0().z = std::min(pt0().z, rect2.pt0().z);

			pt1().x = std::max(pt1().x, rect2.pt1().x);
			pt1().y = std::max(pt1().y, rect2.pt1().y);
			if constexpr (dim >= 3)
				pt1().z = std::max(pt1().z, rect2.pt1().z);

			return *this;
		}

		bool UpdateBoundary(coord_point_t const& pt) {
			bool bModified{};
			for (size_t i {}; i < pt.size(); i++) {
				if (pt0().member(i) > pt.member(i)) {
					bModified = true;
					pt0().member(i) = pt.member(i);
				}
			}
			for (size_t i = 0; i < pt.size(); i++) {
				if (pt1().member(i) < pt.member(i)) {
					bModified = true;
					pt1().member(i) = pt.member(i);
				}
			}
			return bModified;
		}

		//-----------------------------------------------------------------------------
		// ROI
		//
		template < std::integral T_INT = std::int32_t >
		[[nodiscard]] bool IsROI_Valid(TSize2<T_INT> const& sizeImage) const {
			if constexpr (!std::is_integral_v(T)) {
				TRectT<T_INT, 2> rect(*this);
				return rect.IsROI_Valid(sizeImage);
			}

			return 1
				&& (this->left >= 0)
				&& (this->y >= 0)
				&& ( (sizeImage.cx < 0) || ( (this->left < sizeImage.cx) && (this->right < sizeImage.cx) && (this->left < this->right) ) )
				&& ( (sizeImage.cy < 0) || ( (this->y < sizeImage.cy) && (this->bottom < sizeImage.cy) && (this->y < this->bottom) ) )
				;
		}

		template < std::integral T_INT = std::int32_t >
		bool AdjustROI(TSize2<T> const& sizeImage) {
			NormalizeRect();

			if (this->left < 0)
				this->left = 0;
			if (this->y < 0)
				this->y = 0;
			if ( (sizeImage.cx > 0) && (this->right > sizeImage.cx) )
				this->right = sizeImage.cx;
			if ( (sizeImage.cy > 0) && (this->bottom > sizeImage.cy) )
				this->bottom = sizeImage.cy;

			return !IsRectEmpty();
		}

		template < std::integral T_INT = std::int32_t>
		[[nodiscard]] TRectT<T_INT, 2> GetSafeROI(TSize2<T_INT> const& sizeImage) const {
			TRectT<T_INT, 2> rect(*this);

			rect.NormalizeRect();

			if (rect.left < 0)
				rect.left = 0;
			if (rect.y < 0)
				rect.y = 0;
			if ( (sizeImage.cx > 0) && (rect.right > sizeImage.cx) )
				rect.right = sizeImage.cx;
			if ( (sizeImage.cy > 0) && (rect.bottom > sizeImage.cy) )
				rect.bottom = sizeImage.cy;

			return rect;
		}

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
