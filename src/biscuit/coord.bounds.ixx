module;

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_eigen.h"

#ifdef _MSC_VER
#pragma warning(disable: 4201)
#endif

export module biscuit.coord.bounds;

import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;
import biscuit.coord.base;
import biscuit.coord.point;
import biscuit.coord.size;
import biscuit.coord.rect;
import biscuit.string;

export namespace biscuit {


	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tvalue, bool bROUND >
	struct TBounds2 {
		using value_t = tvalue;
		using array_t = std::array<value_t, 4>;
		using this_t = TBounds2<value_t, bROUND>;

		value_t l, t, r, b;

		auto operator <=> (this_t const&) const = default;
	};

	template < typename tvalue, bool bROUND >
	struct TBounds3 {
		using value_t = tvalue;
		using array_t = std::array<value_t, 6>;
		using this_t = TBounds3<value_t, bROUND>;

		value_t l, t, f, r, b, bk;

		auto operator <=> (this_t const&) const = default;
	};

	template < typename tvalue, int DIMENSION, bool bROUND >
	using TBounds_ = std::conditional_t<(DIMENSION == 2), TBounds2<tvalue, bROUND>, TBounds3<tvalue, bROUND>>;


	//=============================================================================================================================
	template < typename tvalue, int DIMENSION, bool bROUND >
		requires (std::is_arithmetic_v<tvalue>)
	struct TBounds : TBounds_<tvalue, DIMENSION, bROUND> {
		using base_t = TBounds_<tvalue, DIMENSION, bROUND>;
		using this_t = TBounds<tvalue, DIMENSION, bROUND>;
		using array_t = base_t::array_t;
		using value_t = base_t::value_t;

		using coord_point_t = TPoint<value_t, DIMENSION, bROUND>;
		using coord_size_t = TSize<value_t, DIMENSION, bROUND>;
		using coord_rect_t = TRect<value_t, DIMENSION, bROUND>;

		constexpr static inline int dim = DIMENSION;
		constexpr static auto count() { return array_t().size(); }

		constexpr static inline value_t default_depth() requires (dim >= 3) { return 1; }

		BSC__NODISCARD constexpr inline auto& pt0()					{ return  *reinterpret_cast<coord_point_t*>(this); }
		BSC__NODISCARD constexpr inline auto const& pt0() const		{ return  *reinterpret_cast<coord_point_t const*>(this); }
		BSC__NODISCARD constexpr inline auto& pt1()					{ return *(reinterpret_cast<coord_point_t*>(this)+1); }
		BSC__NODISCARD constexpr inline auto const& pt1() const		{ return *(reinterpret_cast<coord_point_t const*>(this)+1); }

		BSC__NODISCARD constexpr inline auto pt() const				{ return pt0(); }
		BSC__NODISCARD constexpr inline auto size() const			{ return pt1() - pt0(); }

		TBounds(value_t left = 0, value_t top = 0, value_t right = 0, value_t bottom = 0) requires (dim == 2)
			: base_t{left, top, right, bottom} {
		}
		TBounds(value_t left = 0, value_t top = 0, value_t front = 0, value_t right = 0, value_t bottom = 0, value_t back = default_depth()) requires (dim == 3)
			: base_t{left, top, front, right, bottom, back} {
		}
		TBounds(this_t const&) = default;
		TBounds(this_t&&) = default;
		TBounds(concepts::coord::generic_point auto const& pt0, concepts::coord::generic_point auto const& pt1) { this->pt0() = pt0; this->pt1() = pt1; }
		TBounds(concepts::coord::generic_point auto const& pt,  concepts::coord::generic_size auto const& size) { this->pt0() = pt, this->pt1() = pt+size; }
		TBounds(concepts::coord::generic_size auto const& size) { this->pt0() = coord_point_t{}; this->pt1() = pt+size; }
		TBounds(coord_rect_t const& rect) { this->pt0() = rect; this->pt1() = rect.pt1(); }

		TBounds& operator = (this_t const&) = default;
		TBounds& operator = (this_t&&) = default;

		TBounds& operator = (concepts::coord::generic_point auto const& B) { pt0() = B; return *this; }
		TBounds& operator = (concepts::coord::generic_size auto const& B) { pt1() = pt0() + B; return *this; }
		TBounds& operator = (coord_rect_t const& rect) { this->pt0() = rect; this->pt1() = rect.pt1(); return *this; }

		auto operator <=> (this_t const&) const = default;

		void SetRectEmptyForMinMax2d() {
			this->l =  std::numeric_limits<value_t>::max();
			this->t =  std::numeric_limits<value_t>::max();
			this->r = -std::numeric_limits<value_t>::max();
			this->b = -std::numeric_limits<value_t>::max();
		}
		//=========================================================================================================================
		//--------------------------------------------------------------------------------------------------------------------------
		// get member as array
		BSC__NODISCARD constexpr inline array_t& arr()				{ return *reinterpret_cast<array_t*>(this); }
		BSC__NODISCARD constexpr inline array_t const& arr() const	{ return *reinterpret_cast<array_t const*>(this); }

		auto& operator [] (size_t i) { return arr()[i]; }
		auto const& operator [] (size_t i) const { return arr()[i]; }

		template < typename tchar = char >
		constexpr std::basic_string<tchar> ToString() { return biscuit::FormatToTString<tchar, "{:n}">(arr()); }
		template < concepts::tstring_like tstring >
		bool FromString(tstring const& sv) {
			size_t i{};
			for (auto item : SplitView(sv, ',')) {
				if (i >= count())
					break;
				arr()[i++] = tszto<value_t>(item, 0);
			}
			return i == count();
		}

		//--------------------------------------------------------------------------------------------------------------------------
		struct glaze {
			using T = this_t;
			static constexpr auto value = glz::object(arr());
		};
		// Archiving 
		//friend class cereal::serialization::access;
		template < typename tarchive >
		void serialize(tarchive &ar) {
			ar(arr());
		}

		friend auto min(this_t const& a, this_t const& b) {
			this_t r;
			for (size_t i{}; i < count(); i++) {
				r[i] = std::min(a[i], b[i]);
			}
			return r;
		}
		friend auto max(this_t const& a, this_t const& b) {
			this_t r;
			for (size_t i{}; i < count(); i++) {
				r[i] = std::max(a[i], b[i]);
			}
			return r;
		}
		friend auto min(std::initializer_list<this_t> l) {
			this_t r = *l.begin();
			for (auto& v : l) {
				for (size_t i{}; i < count(); i++) {
					if (r[i] > v[i]) r[i] = v[i];
				}
			}
			return r;
		}
		friend auto max(std::initializer_list<this_t> l) {
			this_t r = *l.begin();
			for (auto& v : l) {
				for (size_t i{}; i < count(); i++) {
					if (r[i] < v[i]) r[i] = v[i];
				}
			}
			return r;
		}

		//--------------------------------------------------------------------------------------------------------------------------
		value_t Width() const { return this->r - this->l; }
		value_t Height() const { return this->b - this->t; }
		value_t Depth() const requires (dim >= 3) { return this->bk - this->f; }

		BSC__NODISCARD static this_t Zero() {
			return this_t{};
		}
		BSC__NODISCARD static this_t All(value_t v = {}) {
			if constexpr (count() == 1) return {v};
			if constexpr (count() == 2) return {v, v};
			if constexpr (count() == 3) return {v, v, v};
			if constexpr (count() == 4) return {v, v, v, v};
			if constexpr (count() == 6) return {v, v, v, v, v, v};
		}

		BSC__NODISCARD auto CountNonZero() const {
			return std::ranges::count_if(arr(), [](auto v) { return v != 0; } );
		}

		//--------------------------------------------------------------------------------------------------------------------------
		BSC__NODISCARD bool IsAllValid() const requires (std::is_floating_point_v<value_t>) {
			for (auto v : arr())
				if ( std::isnan(v) || std::isfinite(v) )
					return false;
			return true;
		}

		bool CheckMinMax(this_t& ptMin, this_t& ptMax) const {
			bool bModified = false;
			auto const& l = arr();
			for (size_t i {}; i < count(); i++) {
				if (ptMin[i] > l[i]) { ptMin[i] = l[i]; bModified = true; }
				if (ptMax[i] < l[i]) { ptMax[i] = l[i]; bModified = true; }
			}
			return bModified;
		}

		auto Area() {
			if constexpr (std::is_integral_v<value_t>) {
				using return_t = std::conditional_t<sizeof(value_t) <= 2, int, int64_t>;
				return (return_t)this->Width() * (return_t)this->Height();
			}
			else
				return this->Width() * this->Height();
		}
		auto Volume() requires (dim >= 3) {
			if constexpr (std::is_integral_v<value_t>) {
				using return_t = std::conditional_t<sizeof(value_t) <= 2, int, int64_t>;
				return (return_t)this->Width() * (return_t)this->Height() * (return_t)this->Depth();
			}
			else
				return this->Width() * this->Height() * this->Depth();
		}


	public:
		auto CenterPoint() const { return (pt0() + pt1())/2; }

		// returns true if rectangle has no area
		bool IsEmpty() const {
			if constexpr (dim == 2) {
				return (this->l >= this->r) || (this->t >= this->b);
			} else if constexpr (dim == 3) {
				return (this->l >= this->r) || (this->t >= this->b) || (this->f >= this->bk);
			}
			else {
				static_assert(false);
			}
		}
		bool Is1dNotEmptyXY() const {
			return this->l < this->r or this->t < this->b;
		}
		// returns true if rectangle is at (0,0,0) and has no area
		bool IsNull() const {
			return (pt() == coord_point_t{}) and (pt1() == coord_point_t{});
		}

		/// @brief Valid for normalized bounds
		/// @return true if pt is in *this
		template < concepts::coord::is_point_ tpoint >
		bool Contains(tpoint const& pt) const {
			return pt0() <= pt and pt < pt1();
		}

		/// @brief Valid for normalized bounds
		/// @return true if pt is in *this
		bool Contains(this_t const& B) const {
			return pt0() <= B.pt0() and B.pt1() <= pt1();
		}

		// Operations

		// set rectangle from left, y, right, and bottom
		void Set(value_t l = 0, value_t t = 0, value_t r = 0, value_t b = 0) requires (dim == 2) {
			this->l = l; this->t = t; this->r = r; this->b = b;
		}
		void Set(value_t l = 0, value_t t = 0, value_t f = 0, value_t r = 0, value_t b = 0, value_t bk = default_depth()) requires (dim == 3) {
			this->l = l; this->t = t; this->f = f; this->r = r; this->t = t; this->bk = bk;
		}

		void Set(concepts::coord::generic_point auto const& pt, concepts::coord::generic_size auto const& size) { pt0() = pt; pt1() = pt + size; }
		void Set(concepts::coord::generic_point auto const& pt0, concepts::coord::generic_point auto const& pt1) { this->pt0() = pt0; this->pt1() = pt1; }

		// inflate the rectangle's width, height and depth
		this_t& Inflate(value_t x, value_t y)												requires (dim == 2) { this->l -= x; this->t -= y;				this->r += x; this->b += y; return *this; }
		this_t& Inflate(value_t x, value_t y, value_t z)									requires (dim == 3) { this->l -= x; this->t -= y; this->f -= z; this->r += x; this->b += y; this->bk += z; return *this; }
		this_t& Inflate(value_t l, value_t t, value_t r, value_t b)							requires (dim == 2) { this->l -= l; this->t -= t;				this->r += r; this->b += b; return *this; }
		this_t& Inflate(value_t l, value_t t, value_t f, value_t r, value_t b, value_t bk)	requires (dim == 3) { this->l -= l; this->t -= t; this->f -= f; this->r += r; this->b += b; this->bk += bk; return *this; }
		this_t& Inflate(this_t const& bounds)																	{ pt() -= bounds.pt0(); pt1() += bounds.pt1(); return *this; }
		template < concepts::coord::generic_coord tcoord >
		this_t& Inflate(tcoord const& coord) {
			if constexpr (concepts::coord::generic_point<tcoord>)		{ auto const v = coord_point_t(coord); pt0() -= v; pt1() += v; }
			else if constexpr (concepts::coord::generic_size<tcoord>)	{ auto const v = coord_size_t(coord); pt0() -= v; pt1() == v; }
			else if constexpr (concepts::coord::generic_rect<tcoord>)	{ auto const v = coord_rect_t(coord); pt0() -= v.pt0(); pt1() += v.pt1(); }
			else { static_assert(false); }
		}

		this_t& Deflate(value_t x, value_t y)												requires (dim == 2) { this->l += x; this->t += y;				this->r -= x; this->b -= y; return *this; }
		this_t& Deflate(value_t x, value_t y, value_t z)									requires (dim == 3) { this->l += x; this->t += y; this->f += z; this->r -= x; this->b -= y; this->bk -= z; return *this; }
		this_t& Deflate(value_t l, value_t t, value_t r, value_t b)							requires (dim == 2) { this->l += l; this->t += t;				this->r -= r; this->b -= b; return *this; }
		this_t& Deflate(value_t l, value_t t, value_t f, value_t r, value_t b, value_t bk)	requires (dim == 3) { this->l += l; this->t += t; this->f += f; this->r -= r; this->b -= b; this->bk -= bk; return *this; }
		this_t& Deflate(this_t const& bounds)																	{ pt() += bounds.pt0(); pt1() -= bounds.pt1(); return *this; }
		template < concepts::coord::generic_coord tcoord >
		this_t& Deflate(tcoord const& coord) {
			if constexpr (concepts::coord::generic_point<tcoord>)		{ auto const v = coord_point_t(coord); pt0() += v; pt1() -= v; }
			else if constexpr (concepts::coord::generic_size<tcoord>)	{ auto const v = coord_size_t(coord); pt0() += v; pt1() == v; }
			else if constexpr (concepts::coord::generic_rect<tcoord>)	{ auto const v = coord_rect_t(coord); pt0() += v.pt0(); pt1() -= v.pt1(); }
			else { static_assert(false); }
		}

		void Normalize() {
			if (this->l > this->r) std::swap(this->l, this->r);
			if (this->t > this->b) std::swap(this->t, this->b);
			if constexpr (dim >= 3) {
				if (this->f > this->bk) std::swap(this->f, this->bk);
			}
		}
		this_t GetNormalized() const {
			this_t rect(*this);
			rect.Normalize();
			return rect;
		}
		bool IsNormalized() const {
			if constexpr (dim == 2) {
				return this->l <= this->r and this->t <= this->b;
			}
			else if constexpr (dim >= 3) {
				return this->l <= this->r and this->t <= this->b and this->f <= this->bk;
			}
		}

		/// @brief fast intersect (without normalization)
		inline this_t& Intersect(this_t const& b){
			pt0() = max(pt0(), b.pt0());
			pt1() = min(pt1(), b.pt1());
			return *this;
		}
		///// @brief safe intersect
		//inline this_t& IntersectSafe(this_t const& b){
		//	Normalize();
		//	return Intersect(b.GetNormalized());
		//}

		/// @brief fast union (without normalization)
		this_t& Union(this_t const& b){
			pt0() = min(pt0(), b.pt0());
			pt1() = max(pt1(), b.pt1());
			return *this;
		}
		///// @brief safe union
		//this_t& UnionSafe(this_t const& b){
		//	Normalize();
		//	return Union(b.GetNormalized());
		//}

		this_t& operator &= (this_t const& b) { return Intersect(b); }
		this_t& operator |= (this_t const& b) { return Union(b); }

		constexpr this_t operator & (this_t const& b) { return this_t(*this) &= b; }
		constexpr this_t operator | (this_t const& b) { return this_t(*this) |= b; }

		bool UpdateBounds(coord_point_t const& pt) {
			bool bModified{};
			for (size_t i {}; i < pt.count(); i++) {
				if (pt0()[i] > pt[i]) {
					bModified = true;
					pt0()[i] = pt[i];
				}
			}
			for (size_t i = 0; i < pt.count(); i++) {
				if (pt1()[i] < pt[i]) {
					bModified = true;
					pt1()[i] = pt[i];
				}
			}
			return bModified;
		}

	};

	using sBounds2i = TBounds<int, 2, false>;
	using sBounds3i = TBounds<int, 3, false>;
	using sBounds2d = TBounds<double, 2, false>;
	using sBounds3d = TBounds<double, 3, false>;
	using sBounds2ri = TBounds<int, 2, true>;
	using sBounds3ri = TBounds<int, 3, true>;
	using sBounds2rd = TBounds<double, 2, true>;
	using sBounds3rd = TBounds<double, 3, true>;

	static_assert(requires(sBounds2d ) { sBounds2d{1.0, 2.0, 3.0, 4.0}; });
	static_assert(requires(sBounds3d ) { sBounds3d{1.0, 2.0, 3.0, 4.0, 5, 6}; });
	static_assert(sizeof(sBounds2d) == sizeof(double)*2*2);
	static_assert(sizeof(sBounds3d) == sizeof(double)*3*2);
	static_assert(std::is_trivially_copyable_v<sBounds2d>);
	//static_assert(concepts::coord::has_point2<sRect2i>);
	//static_assert(concepts::coord::has_size2<sRect2i>);
	//static_assert(concepts::coord::has_rect2<sRect2i>);
	//static_assert(!concepts::coord::is_point2<sRect2i>);
	//static_assert(!concepts::coord::is_size2<sRect2i>);
	//static_assert(concepts::coord::is_rect2<sRect2i>);


}
