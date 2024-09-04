module;

#include <glaze/glaze.hpp>
#include "biscuit/macro.h"

export module biscuit.coord.base;
import std;
//import "biscuit/macro.h";
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;

namespace concepts = biscuit::concepts;

namespace biscuit::coord {

	struct arithmetic_dummy_t {
		inline arithmetic_dummy_t operator + (auto ) const { return *this; };
		inline arithmetic_dummy_t operator - (auto ) const { return *this; };
		inline arithmetic_dummy_t operator * (auto ) const { return *this; };
		inline arithmetic_dummy_t operator / (auto ) const { return *this; };
		friend inline arithmetic_dummy_t operator + (auto, arithmetic_dummy_t const& ) { return *this; };
		friend inline arithmetic_dummy_t operator - (auto, arithmetic_dummy_t const& ) { return *this; };
		friend inline arithmetic_dummy_t operator * (auto, arithmetic_dummy_t const& ) { return *this; };
		friend inline arithmetic_dummy_t operator / (auto, arithmetic_dummy_t const& ) { return *this; };
	};
	inline arithmetic_dummy_t operator += (auto&, arithmetic_dummy_t const& ) { return *this; };
	inline arithmetic_dummy_t operator -= (auto&, arithmetic_dummy_t const& ) { return *this; };
	inline arithmetic_dummy_t operator *= (auto&, arithmetic_dummy_t const& ) { return *this; };
	inline arithmetic_dummy_t operator /= (auto&, arithmetic_dummy_t const& ) { return *this; };
	static_assert(!std::is_arithmetic_v<arithmetic_dummy_t>);

}

export namespace biscuit::coord {

	//using namespace biscuit::concepts::coord;

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename T >
	struct TPoint2 {
		using coord_type_t = mPoint_t;
		using this_t = TPoint2;
		using value_t = T;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim>;

		value_t x, y;

		auto operator <=> (this_t const&) const = default;
	};
	template < typename T >
	struct TPoint3 {
		using coord_type_t = mPoint_t;
		using this_t = TPoint3;
		using value_t = T;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim>;

		value_t x, y, z;
		auto operator <=> (this_t const&) const = default;
	};
	template < typename T >
	struct TPoint4 {
		using coord_type_t = mPoint_t;
		using this_t = TPoint4;
		using value_t = T;
		constexpr static int const dim = 4;
		using array_t = std::array<value_t, dim>;

		value_t x, y, z, w;
		auto operator <=> (this_t const&) const = default;
	};

	template < typename T, int DIMENSION >
	using TPoint_ = std::conditional_t<DIMENSION == 2, TPoint2<T>, std::conditional_t<DIMENSION == 3, TPoint3<T>, std::conditional_t<DIMENSION == 4, TPoint4<T>, void >>>;

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename T >
	struct TSize2 {
		using coord_type_t = mSize_t;
		using value_t = T;
		using this_t = TSize2;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim>;

		value_t width, height;
		auto operator <=> (this_t const&) const = default;
	};
	template < typename T >
	struct TSize3 {
		using coord_type_t = mSize_t;
		using this_t = TSize3;
		using value_t = T;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim>;

		value_t width, height, depth;
		auto operator <=> (this_t const&) const = default;
	};

	template < typename T, int DIMENSION >
	using TSize_ = std::conditional_t<DIMENSION == 2, TSize2<T>, std::conditional_t<DIMENSION == 3, TSize3<T>, void >>;

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename T >
	struct TRect2 {
		using coord_type_t = mRect_t;
		using this_t = TRect2;
		using value_t = T;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim*2>;

		value_t x, y, width, height;
		auto operator <=> (this_t const&) const = default;
	};
	template < typename T >
	struct TRect3 {
		using coord_type_t = mRect_t;
		using this_t = TRect3;
		using value_t = T;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim*2>;

		value_t x, y, z, width, height, depth;
		auto operator <=> (this_t const&) const = default;
	};

	template < typename T, int DIMENSION >
	using TRect_ = std::conditional_t<DIMENSION == 2, TRect2<T>, std::conditional_t<DIMENSION == 3, TRect3<T>, void >>;

	//=============================================================================================================================
	//template < typename ttarget >
	//	requires (concepts::is_one_of<typename ttarget::coord_type_t, mPoint_t, mSize_t, mRect_t>)
	template < template < typename, int> typename ttarget, typename tvalue, int DIM, bool bROUND >
		requires (concepts::is_one_of<typename ttarget<tvalue, DIM>::coord_type_t, mPoint_t, mSize_t, mRect_t>)
	struct TCoordBase : ttarget<tvalue, DIM> {
		using base_t = ttarget<tvalue, DIM>;
		using coord_type_t = base_t::coord_type_t;
		using value_t = base_t::value_t;
		constexpr static int const dim = base_t::dim;
		constexpr static auto count() { return base_t::array_t().size(); }
		constexpr static bool const bPoint = std::is_same_v<coord_type_t, mPoint_t>;
		constexpr static bool const bSize = std::is_same_v<coord_type_t, mSize_t>;
		constexpr static bool const bRect = std::is_same_v<coord_type_t, mRect_t>;
		constexpr static bool const bRound = bROUND;

		using array_t = base_t::array_t;
		using this_t = TCoordBase;

		template < typename T, int DIM >
		using tbase_t = ttarget<T, DIM>;

		// constructors
		//TCoordBase() = default;
		TCoordBase(value_t i0 = {}) requires(count() == 1) { arr()[0] = i0; }
		TCoordBase(value_t i0={}, value_t i1={}) requires(count() == 2) { arr()[0] = i0; arr()[1] = i1; }
		TCoordBase(value_t i0={}, value_t i1={}, value_t i2={}) requires(count() == 3) { arr()[0] = i0; arr()[1] = i1; arr()[2] = i2; }
		TCoordBase(value_t i0={}, value_t i1={}, value_t i2={}, value_t i3={}) requires(count() == 4) { arr()[0] = i0; arr()[1] = i1; arr()[2] = i2; arr()[3] = i3; }
		TCoordBase(value_t x={}, value_t y={}, value_t z={}, value_t width={}, value_t height={}, value_t depth={}) requires(count() == 6) {
			arr()[0] = x; arr()[1] = y; arr()[2] = z;
			arr()[3] = width; arr()[4] = height; arr()[5] = depth;
		}
		//constexpr TCoordBase(std::initializer_list<value_t> v) {
		//	int i = 0;
		//	for (auto& e : v) {
		//		arr()[i++] = e;
		//		if (i >= count()) break;
		//	}
		//}
		//TCoordBase(this_t const& other) { arr() = other.arr(); }
		//TCoordBase(this_t&& other) { arr() = std::move(other.arr()); }
		//auto& operator = (this_t const& other) { arr() = other.arr(); return *this; }
		//auto& operator = (this_t&& other) { arr() = std::move(other.arr()); return *this; }

		template < typename tcoord2 >
			requires !std::is_same_v<tcoord2, this_t> and concepts::coord::generic_coord<tcoord2>
		explicit TCoordBase(tcoord2 const& b) {
			*this = b;
		}

		template < typename tcoord2 >
			requires !std::is_same_v<tcoord2, this_t> and concepts::coord::generic_coord<tcoord2>
		TCoordBase& operator = (tcoord2 const& b) {
			if constexpr (concepts::coord::has_point2<base_t>) {
				if constexpr (concepts::coord::has_point2<tcoord2> or concepts::coord::has_ipoint2<tcoord2>) {
					MemberSet_x(MemberGet_x(b));
					MemberSet_y(MemberGet_y(b));
					MemberSet_z(MemberGet_z(b));
					MemberSet_w(MemberGet_w(b));
				}
				else if constexpr (concepts::coord::has_size2<tcoord2> or concepts::coord::has_isize2<tcoord2>) {
					MemberSet_x(MemberGet_width(b));
					MemberSet_y(MemberGet_height(b));
					MemberSet_z(MemberGet_depth(b));
				}
				else {
					static_assert(false);
				}
			}
			if constexpr (concepts::coord::has_size2<base_t>) {
				if constexpr (concepts::coord::has_size2<tcoord2> or concepts::coord::has_isize2<tcoord2>) {
					MemberSet_width (MemberGet_width(b));
					MemberSet_height(MemberGet_height(b));
					MemberSet_depth (MemberGet_depth(b));
				}
				else if constexpr (concepts::coord::has_point2<tcoord2> or concepts::coord::has_ipoint2<tcoord2>) {
					MemberSet_width (MemberGet_x(b));
					MemberSet_height(MemberGet_y(b));
					MemberSet_depth (MemberGet_z(b));
				}
			}
			return *this;
		}

		template < concepts::coord::generic_coord tcoord2 >
		operator tcoord2() const {
			tcoord2 r;
			if constexpr (bRect or concepts::coord::generic_rect<tcoord2>) {
				MemberSet_x(r, MemberGet_x());
				MemberSet_y(r, MemberGet_y());
				MemberSet_z(r, MemberGet_z());
				MemberSet_w(r, MemberGet_w());
				MemberSet_width(r, MemberGet_width());
				MemberSet_depth(r, MemberGet_depth());
				MemberSet_height(r, MemberGet_height());
			}
			else if constexpr (concepts::coord::generic_point<tcoord2> and bSize) {
				MemberSet_x(r, MemberGet_width());
				MemberSet_y(r, MemberGet_height());
				MemberSet_z(r, MemberGet_depth());
			}
			else if constexpr (concepts::coord::generic_point<tcoord2> and bPoint) {
				MemberSet_x(r, MemberGet_x());
				MemberSet_y(r, MemberGet_y());
				MemberSet_z(r, MemberGet_z());
				MemberSet_w(r, MemberGet_w());
			}
			else if constexpr (concepts::coord::generic_size<tcoord2> and bSize) {
				MemberSet_width(r, MemberGet_width());
				MemberSet_height(r, MemberGet_height());
				MemberSet_depth(r, MemberGet_depth());
			}
			else if constexpr (concepts::coord::generic_size<tcoord2> and bPoint) {
				MemberSet_width(r, MemberGet_x());
				MemberSet_height(r, MemberGet_y());
				MemberSet_depth(r, MemberGet_z());
			}
			return r;
		}
		//--------------------------------------------------------------------------------------------------------------------------
		BSC__NODISCARD auto operator <=> (this_t const&) const = default;
		BSC__NODISCARD auto operator <=> (value_t const& v) const { return *this <=> this_t::All(v); }

		//=========================================================================================================================
		//--------------------------------------------------------------------------------------------------------------------------
		// get member as array
		//constexpr auto& arr(this auto&& self) {
		//	if constexpr (std::is_const_v<std::remove_reference_t<decltype(self)>>) {
		//		return *reinterpret_cast<std::array<value_t, DIMENSION>const*>(&self);
		//	}
		//	else {
		//		return *reinterpret_cast<std::array<value_t, DIMENSION>*>(&self);
		//	}
		//}
		BSC__NODISCARD constexpr inline array_t& arr()				{ return *reinterpret_cast<array_t*>(this); }
		BSC__NODISCARD constexpr inline array_t const& arr() const	{ return *reinterpret_cast<array_t const*>(this); }

		auto& operator [] (size_t i) { return arr()[i]; }
		auto const& operator [] (size_t i) const { return arr()[i]; }

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

		//--------------------------------------------------------------------------------------------------------------------------

		BSC__NODISCARD static this_t Zero() {
			return this_t{};
		}
		BSC__NODISCARD static this_t All(value_t v = {}) requires (bPoint or bSize) {
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

		//=========================================================================================================================
		// operator
		template < typename toperator, concepts::coord::generic_coord tcoord2>
		this_t& Operation(tcoord2 const& b) {
			constexpr static toperator op;
			if constexpr (bRect or concepts::coord::generic_coord<tcoord2>) {
				if constexpr (concepts::coord::has_point2<tcoord2>) {
					op(MemberGet_x(), MemberGet_x(b));
					op(MemberGet_y(), MemberGet_y(b));
					op(MemberGet_z(), MemberGet_z(b));
				}
				if constexpr (concepts::coord::has_size2<tcoord2>) {
					op(MemberGet_width (), MemberGet_width (b));
					op(MemberGet_height(), MemberGet_height(b));
					op(MemberGet_depth (), MemberGet_depth (b));
				}
			}
			else if constexpr (bPoint) {
				if constexpr (concepts::coord::has_point2<tcoord2>) {
					op(MemberGet_x(), MemberGet_x(b));
					op(MemberGet_y(), MemberGet_y(b));
					op(MemberGet_z(), MemberGet_z(b));
				}
				else if constexpr (concepts::coord::has_size2<tcoord2>) {
					op(MemberGet_x(), MemberGet_width(b));
					op(MemberGet_y(), MemberGet_height(b));
					op(MemberGet_z(), MemberGet_depth(b));
				}
			}
			else if constexpr (bSize) {
				if constexpr (concepts::coord::has_point2<tcoord2>) {
					op(MemberGet_width(), MemberGet_x(b));
					op(MemberGet_height(), MemberGet_y(b));
					op(MemberGet_depth(), MemberGet_z(b));
				}
				else if constexpr (concepts::coord::has_size2<tcoord2>) {
					op(MemberGet_width(), MemberGet_width(b));
					op(MemberGet_height(), MemberGet_height(b));
					op(MemberGet_depth(), MemberGet_depth(b));
				}
			}
			return *this;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		this_t& operator += (this_t const& b) {
			for (size_t i{}; i < count(); i++)
				arr()[i] += b[i];
			return *this;
		}
		this_t& operator -= (this_t const& b) {
			for (size_t i{}; i < count(); i++)
				arr()[i] -= b[i];
			return *this;
		}

		template < concepts::coord::generic_coord tcoord2 >
		inline this_t& operator += (tcoord2 const& b) { struct sOp { inline void operator() (auto& a, auto& b) { a += b; } }; return Operation<sOp>(b); }
		template < concepts::coord::generic_coord tcoord2 >
		inline this_t& operator -= (tcoord2 const& b) { struct sOp { inline void operator() (auto& a, auto& b) { a -= b; } }; return Operation<sOp>(b); }
		template < concepts::coord::generic_coord tcoord2 >
		inline this_t& operator *= (tcoord2 const& b) { struct sOp { inline void operator() (auto& a, auto& b) { a *= b; } }; return Operation<sOp>(b); }
		template < concepts::coord::generic_coord tcoord2 >
		inline this_t& operator /= (tcoord2 const& b) { struct sOp { inline void operator() (auto& a, auto& b) { a /= b; } }; return Operation<sOp>(b); }

		inline this_t& operator += (concepts::arithmetic auto v) { for (auto& c : arr()) c += v; return *this; }
		inline this_t& operator -= (concepts::arithmetic auto v) { for (auto& c : arr()) c -= v; return *this; }
		inline this_t& operator *= (concepts::arithmetic auto v) { for (auto& c : arr()) c *= v; return *this; }
		inline this_t& operator /= (concepts::arithmetic auto v) { for (auto& c : arr()) c /= v; return *this; }

		// member wise +, -, *, /
		BSC__NODISCARD constexpr inline this_t operator - () const { this_t r; for (size_t i{}; i < count(); i++) r[i] = -arr()[i]; return r; }
		template < typename T > requires concepts::coord::generic_coord<T> or concepts::arithmetic<T>
		BSC__NODISCARD inline this_t operator + (T const& b) const { return this_t(*this) += b; }
		template < typename T > requires concepts::coord::generic_coord<T> or concepts::arithmetic<T>
		BSC__NODISCARD inline this_t operator - (T const& b) const { return this_t(*this) -= b; }
		template < typename T > requires concepts::coord::generic_coord<T> or concepts::arithmetic<T>
		BSC__NODISCARD inline this_t operator * (T const& b) const { return this_t(*this) *= b; }
		template < typename T > requires concepts::coord::generic_coord<T> or concepts::arithmetic<T>
		BSC__NODISCARD inline this_t operator / (T const& b) const { return this_t(*this) /= b; }

		template < typename T > requires concepts::coord::generic_coord<T> or concepts::arithmetic<T>
		BSC__NODISCARD friend inline this_t operator + (T const& a, this_t const& b) { return  b + a; }
		template < typename T > requires concepts::coord::generic_coord<T> or concepts::arithmetic<T>
		BSC__NODISCARD friend inline this_t operator - (T const& a, this_t const& b) { return -b += a; }	// not just naively (-b + a)
		template < typename T > requires concepts::coord::generic_coord<T> or concepts::arithmetic<T>
		BSC__NODISCARD friend inline this_t operator * (T const& a, this_t const& b) { return  b * a; }
		template < typename T > requires concepts::coord::generic_coord<T> or concepts::arithmetic<T>
		friend inline this_t operator / (auto a, this_t const& b) = delete;


		//=========================================================================================================================
		// round, floor
		template < typename tvalue_to = int >
		TCoordBase<tbase_t, tvalue_to, dim, bROUND> Round() const {
			if constexpr (count() == 2) {
				return { (tvalue_to)std::round(arr()[0]), (tvalue_to)std::round(arr()[1]) };
			}
			else if constexpr (count() == 3) {
				return { (tvalue_to)std::round(arr()[0]), (tvalue_to)std::round(arr()[1]), (tvalue_to)std::round(arr()[2]) };
			}
			else if constexpr (count() == 4) {
				return { (tvalue_to)std::round(arr()[0]), (tvalue_to)std::round(arr()[1]), (tvalue_to)std::round(arr()[2]), (tvalue_to)std::round(arr()[3]) };
			}
			else if constexpr (count() == 6) {
				return { (tvalue_to)std::round(arr()[0]), (tvalue_to)std::round(arr()[1]), (tvalue_to)std::round(arr()[2]),
					(tvalue_to)std::round(arr()[3]), (tvalue_to)std::round(arr()[4]), (tvalue_to)std::round(arr()[5]) };
			}
			else {
				static_assert(false);
			}
		}
		template < typename tvalue_to = int >
		TCoordBase<tbase_t, tvalue_to, dim, bROUND> Floor() const {
			if constexpr (dim == 2) {
				return { (tvalue_to)std::floor(arr()[0]), (tvalue_to)std::floor(arr()[1]) };
			}
			else if constexpr (dim == 3) {
				return { (tvalue_to)std::floor(arr()[0]), (tvalue_to)std::floor(arr()[1]), (tvalue_to)std::floor(arr()[2]) };
			}
			else if constexpr (dim == 4) {
				return { (tvalue_to)std::floor(arr()[0]), (tvalue_to)std::floor(arr()[1]), std::round(arr()[2]), (tvalue_to)std::floor(arr()[3]) };
			}
			else if constexpr (count() == 6) {
				return { (tvalue_to)std::floor(arr()[0]), (tvalue_to)std::floor(arr()[1]), (tvalue_to)std::floor(arr()[2]),
					(tvalue_to)std::floor(arr()[3]), (tvalue_to)std::floor(arr()[4]), (tvalue_to)std::floor(arr()[5]) };
			}
			else {
				static_assert(false);
			}
		}

		//=========================================================================================================================
	protected:
		// round, floor
		template < typename treturn = value_t >
		constexpr static inline treturn PassValue(auto value) {
			if constexpr (bRound and std::is_integral_v<treturn> and std::is_floating_point_v<std::remove_cvref_t<decltype(value)>>)
				return (treturn)std::round(value);
			else
				return (treturn)value;
		}

		// get/set member (for internal)
	#define DEFINE_MEMBER_GET_SET(VAR, VAR_U) \
		BSC__NODISCARD constexpr inline decltype(auto) MemberGet_##VAR() {\
			if constexpr (concepts::coord::has_##VAR<base_t>) { return base_t::VAR; }\
			else { return arithmetic_dummy_t{}; }\
		}\
		BSC__NODISCARD constexpr inline auto const MemberGet_##VAR() const {\
			if constexpr (concepts::coord::has_##VAR<base_t>) { return base_t::VAR; }\
			else { return arithmetic_dummy_t{}; }\
		}\
		constexpr inline void MemberSet_##VAR(auto value) {\
			if constexpr (std::is_arithmetic_v<std::remove_cvref_t<decltype(value)>>) {\
				if constexpr (concepts::coord::has_##VAR<base_t>) { base_t::VAR = PassValue<value_t>(value); }\
			}\
		}\
		\
		template < concepts::coord::generic_coord tcoord2 >\
		BSC__NODISCARD constexpr static inline decltype(auto) MemberGet_##VAR(tcoord2&& coord) {\
			if constexpr (concepts::coord::has_##VAR<tcoord2>) { return PassValue<value_t>(coord.VAR); }\
			else if constexpr (concepts::coord::has_i##VAR<tcoord2>) { return PassValue<value_t>(coord.VAR()); }\
			else { return arithmetic_dummy_t{}; }\
		}\
		template < concepts::coord::generic_coord tcoord2 >\
		constexpr static inline void MemberSet_##VAR(tcoord2&& coord, auto value) {\
			if constexpr (std::is_arithmetic_v<std::remove_cvref_t<decltype(value)>>) {\
				if constexpr (concepts::coord::has_##VAR<tcoord2>) { coord.VAR = PassValue<std::remove_cvref_t<decltype(coord.VAR)>>(value); }\
				if constexpr (concepts::coord::has_i##VAR<tcoord2>) { coord.set##VAR_U( PassValue<std::remove_cvref_t<decltype(coord.VAR())>>(value)); }\
			}\
		}

		DEFINE_MEMBER_GET_SET(x, X);
		DEFINE_MEMBER_GET_SET(y, Y);
		DEFINE_MEMBER_GET_SET(z, Z);
		DEFINE_MEMBER_GET_SET(w, W);
		DEFINE_MEMBER_GET_SET(width, Width);
		DEFINE_MEMBER_GET_SET(height, Height);
		DEFINE_MEMBER_GET_SET(depth, Depth);

	#undef DEFINE_MEMBER_GET_SET
	};

//#define DEFINE_MEMBER_GET_SET(VAR, VAR_U) \
//	template < concepts::coord::generic_coord tcoord2 >\
//	BSC__NODISCARD constexpr inline decltype(auto) MemberGet_##VAR(tcoord2&& coord) {\
//		if constexpr (concepts::coord::has_##VAR<tcoord2>) { return coord.VAR; }\
//		else if constexpr (concepts::coord::has_i##VAR<tcoord2>) { return coord.VAR(); }\
//		else { return detail::dummy_t{}; }\
//	}\
//	template < concepts::coord::generic_coord tcoord2 >\
//	constexpr inline void MemberSet_##VAR(tcoord2&& coord, auto value) {\
//		if constexpr (std::is_arithmetic_v<std::remove_cvref_t<decltype(value)>>) {\
//			if constexpr (concepts::coord::has_##VAR<tcoord2>) { coord.VAR = value; }\
//			if constexpr (concepts::coord::has_i##VAR<tcoord2>) { coord.set##VAR_U(value); }\
//		}\
//	}

}

