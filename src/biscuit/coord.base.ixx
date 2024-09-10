module;

#ifdef _MSC_VER
#pragma warning(disable: 4201)
#endif

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_eigen.h"

export module biscuit.coord.base;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.arithmetic;
import biscuit.misc;
import biscuit.coord.concepts;
import biscuit.convert_codepage;
import biscuit.string;

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
	inline arithmetic_dummy_t operator += (auto&, arithmetic_dummy_t const& b) { return b; };
	inline arithmetic_dummy_t operator -= (auto&, arithmetic_dummy_t const& b) { return b; };
	inline arithmetic_dummy_t operator *= (auto&, arithmetic_dummy_t const& b) { return b; };
	inline arithmetic_dummy_t operator /= (auto&, arithmetic_dummy_t const& b) { return b; };
	static_assert(!std::is_arithmetic_v<arithmetic_dummy_t>);	// can be used with +,-,*,/, but, not arithmetic.

}

export namespace biscuit::coord {

	//using namespace biscuit::concepts::coord;

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tvalue >
	struct TPoint2 {
		using coord_type_t = mPoint_t;
		using this_t = TPoint2;
		using value_t = tvalue;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim>;

		value_t x, y;

		auto operator <=> (this_t const&) const = default;
	};
	template < typename tvalue >
	struct TPoint3 {
		using coord_type_t = mPoint_t;
		using this_t = TPoint3;
		using value_t = tvalue;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim>;

		value_t x, y, z;
		auto operator <=> (this_t const&) const = default;
	};
	template < typename tvalue >
	struct TPoint4 {
		using coord_type_t = mPoint_t;
		using this_t = TPoint4;
		using value_t = tvalue;
		constexpr static int const dim = 4;
		using array_t = std::array<value_t, dim>;

		value_t x, y, z, w;
		auto operator <=> (this_t const&) const = default;
	};

	template < typename tvalue, int DIMENSION >
	using TPoint_ = std::conditional_t<DIMENSION == 2, TPoint2<tvalue>, std::conditional_t<DIMENSION == 3, TPoint3<tvalue>, std::conditional_t<DIMENSION == 4, TPoint4<tvalue>, void >>>;

	/// @brief Interpolation (lerp)
	template < typename T, int dim >
	TPoint_<T, dim> lerp(TPoint_<T, dim> const& a, TPoint_<T, dim> const& b, double t) {
		TPoint_<T, dim> c;
		for (int i = 0; i < dim; i++)
			c[i] = std::lerp(a[i], b[i], t);
		return c;
	}


	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tvalue >
	struct TSize2 {
		using coord_type_t = mSize_t;
		using value_t = tvalue;
		using this_t = TSize2;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim>;

		value_t width, height;
		auto operator <=> (this_t const&) const = default;
	};
	template < typename tvalue >
	struct TSize3 {
		using coord_type_t = mSize_t;
		using this_t = TSize3;
		using value_t = tvalue;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim>;

		value_t width, height, depth;
		auto operator <=> (this_t const&) const = default;
	};

	template < typename tvalue, int DIMENSION >
	using TSize_ = std::conditional_t<DIMENSION == 2, TSize2<tvalue>, std::conditional_t<DIMENSION == 3, TSize3<tvalue>, void >>;

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tvalue >
	struct TRect2 {
		using coord_type_t = mRect_t;
		using this_t = TRect2;
		using value_t = tvalue;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim*2>;

		value_t x, y, width, height;
		auto operator <=> (this_t const&) const = default;
	};
	template < typename tvalue >
	struct TRect3 {
		using coord_type_t = mRect_t;
		using this_t = TRect3;
		using value_t = tvalue;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim*2>;

		value_t x, y, z, width, height, depth;
		auto operator <=> (this_t const&) const = default;
	};

	template < typename tvalue, int DIMENSION >
	using TRect_ = std::conditional_t<DIMENSION == 2, TRect2<tvalue>, std::conditional_t<DIMENSION == 3, TRect3<tvalue>, void >>;

	//=============================================================================================================================
	//template < typename ttarget >
	//	requires (concepts::is_one_of<typename ttarget::coord_type_t, mPoint_t, mSize_t, mRect_t>)
	template < template < typename, int> typename ttarget, typename tvalue, int DIM, bool bROUND >
		requires (concepts::is_one_of<typename ttarget<tvalue, DIM>::coord_type_t, mPoint_t, mSize_t, mRect_t>)
	struct /*alignas(alignof(Eigen::Vector<tvalue, DIM>)) */TCoordBase : ttarget<tvalue, DIM> {
		using base_t = ttarget<tvalue, DIM>;
		using coord_type_t = base_t::coord_type_t;
		using array_t = base_t::array_t;
		using value_t = base_t::value_t;
		constexpr static int const dim = base_t::dim;
		constexpr static auto count() { return array_t().size(); }
		constexpr static bool const bPoint = std::is_same_v<coord_type_t, mPoint_t>;
		constexpr static bool const bSize = std::is_same_v<coord_type_t, mSize_t>;
		constexpr static bool const bRect = std::is_same_v<coord_type_t, mRect_t>;
		constexpr static bool const bRound = bROUND;

		using this_t = TCoordBase<ttarget, tvalue, DIM, bROUND>;

		template < typename tvalue2, int DIM2 >
		using tbase_t = ttarget<tvalue2, DIM2>;

		static_assert(!std::is_integral_v<value_t> or std::is_signed_v<value_t>, "value_t must be one of floating point or signed integral.");

		// constructors
		//TCoordBase() = default;
		TCoordBase(value_t i0={}) requires(count() == 1) { arr()[0] = i0; }
		TCoordBase(value_t i0={}, value_t i1={}) requires(count() == 2) { arr()[0] = i0; arr()[1] = i1; }
		TCoordBase(value_t i0={}, value_t i1={}, value_t i2={}) requires(count() == 3) { arr()[0] = i0; arr()[1] = i1; arr()[2] = i2; }
		TCoordBase(value_t i0={}, value_t i1={}, value_t i2={}, value_t i3={}) requires(count() == 4 && !bRect) { arr()[0] = i0; arr()[1] = i1; arr()[2] = i2; arr()[3] = i3; }
		TCoordBase(value_t x={}, value_t y={}, value_t width={}, value_t height={}) requires(dim == 2 && bRect) { arr()[0] = x; arr()[1] = y; arr()[2] = width; arr()[3] = height; }
		TCoordBase(value_t x={}, value_t y={}, value_t z={}, value_t width={}, value_t height={}, value_t depth=1) requires(dim == 3 and bRect) {
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
		BSC__NODISCARD constexpr inline array_t& arr()				{ return *reinterpret_cast<array_t*>(this); }
		BSC__NODISCARD constexpr inline array_t const& arr() const	{ return *reinterpret_cast<array_t const*>(this); }

		auto& operator [] (size_t i) { return arr()[i]; }
		auto const& operator [] (size_t i) const { return arr()[i]; }

		template < typename tchar = char >
		constexpr std::basic_string<tchar> ToString() {
			if constexpr (std::is_same_v<tchar, char>)			return std::format("{:n}", arr());
			else if constexpr (std::is_same_v<tchar, wchar_t>)	return std::format(L"{:n}", arr());
			else if constexpr (std::is_same_v<tchar, char16_t>) return std::format(u"{:n}", arr());
			else if constexpr (std::is_same_v<tchar, char32_t>) return std::format(U"{:n}", arr());
			else { static_assert(false); }
		}
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

		/// @brief Interpolation (lerp)
		this_t lerp(this_t const& b, double t) requires (bPoint) {
			this_t c;
			for (int i{}; i < dim; i++)
				c[i] = std::lerp(arr()[i], b[i], t);
			return c;
		}

		//--------------------------------------------------------------------------------------------------------------------------

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

		auto Area() requires (bSize or bRect) {
			if constexpr (std::is_integral_v<value_t>) {
				using return_t = std::conditional_t<sizeof(value_t) <= 2, int, int64_t>;
				return (return_t)this->width * (return_t)this->height;
			}
			else
				return this->width * this->height;
		}
		auto Volume() requires ((bSize or bRect) and (dim >= 3)) {
			if constexpr (std::is_integral_v<value_t>) {
				using return_t = std::conditional_t<sizeof(value_t) <= 2, int, int64_t>;
				return (return_t)this->width * (return_t)this->height * (return_t)this->depth;
			}
			else
				return this->width * this->height * this->depth;
		}

		//=========================================================================================================================
		// operator
		template < typename toperator, concepts::coord::generic_coord tcoord2>
		this_t& Operation(tcoord2 const& b) {
			constexpr static toperator op;
			if constexpr (bRect or concepts::coord::generic_rect<tcoord2>) {
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
					op(MemberGet_w(), MemberGet_w(b));
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

	protected:
		struct sOpAdd { inline void operator() (auto&& a, auto const& b) const { a += b; } };
		struct sOpSub { inline void operator() (auto&& a, auto const& b) const { a -= b; } };
		struct sOpMul { inline void operator() (auto&& a, auto const& b) const { a *= b; } };
		struct sOpDiv { inline void operator() (auto&& a, auto const& b) const { a /= b; } };

	public:
		template < concepts::coord::generic_coord tcoord2 >
		inline this_t& operator += (tcoord2 const& b) { return Operation<sOpAdd>(b); }
		template < concepts::coord::generic_coord tcoord2 >
		inline this_t& operator -= (tcoord2 const& b) { return Operation<sOpSub>(b); }
		template < concepts::coord::generic_coord tcoord2 >
		inline this_t& operator *= (tcoord2 const& b) { return Operation<sOpMul>(b); }
		template < concepts::coord::generic_coord tcoord2 >
		inline this_t& operator /= (tcoord2 const& b) { return Operation<sOpDiv>(b); }

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

		template < typename T > requires (concepts::arithmetic<T>)
		BSC__NODISCARD friend inline this_t operator + (T const& a, this_t const& b) { return  b + a; }
		template < typename T > requires (concepts::arithmetic<T>)
		BSC__NODISCARD friend inline this_t operator - (T const& a, this_t const& b) { return (-b + a); }
		template < typename T > requires (concepts::arithmetic<T>)
		BSC__NODISCARD friend inline this_t operator * (T const& a, this_t const& b) { return  b * a; }
		template < typename T > requires (concepts::arithmetic<T>)
		friend inline this_t operator / (auto a, this_t const& b) = delete;


		//=========================================================================================================================
		// round, floor
		template < typename tvalue_to = value_t >
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
		template < typename tvalue_to = value_t >
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
		template < typename tvalue_to = value_t >
		TCoordBase<tbase_t, tvalue_to, dim, bROUND> Ceil() const {
			if constexpr (dim == 2) {
				return { (tvalue_to)std::ceil(arr()[0]), (tvalue_to)std::ceil(arr()[1]) };
			}
			else if constexpr (dim == 3) {
				return { (tvalue_to)std::ceil(arr()[0]), (tvalue_to)std::ceil(arr()[1]), (tvalue_to)std::ceil(arr()[2]) };
			}
			else if constexpr (dim == 4) {
				return { (tvalue_to)std::ceil(arr()[0]), (tvalue_to)std::ceil(arr()[1]), std::round(arr()[2]), (tvalue_to)std::ceil(arr()[3]) };
			}
			else if constexpr (count() == 6) {
				return { (tvalue_to)std::ceil(arr()[0]), (tvalue_to)std::ceil(arr()[1]), (tvalue_to)std::ceil(arr()[2]),
					(tvalue_to)std::ceil(arr()[3]), (tvalue_to)std::ceil(arr()[4]), (tvalue_to)std::ceil(arr()[5]) };
			}
			else {
				static_assert(false);
			}
		}

	public:
		//=========================================================================================================================
		BSC__NODISCARD constexpr inline double GetDistance(this_t const& pt) const requires (bPoint or bSize) {
			return (*this - pt).GetDistance();
		}
		BSC__NODISCARD constexpr inline double GetDistance() const requires (bPoint or bSize) {
			double sum {};
			for (auto v : arr())
				sum += Square(v);
			return std::sqrt(sum);
		}
		BSC__NODISCARD constexpr inline double GetTaxicabDistance(this_t const& pt) const requires (bPoint or bSize) {
			return (*this - pt).GetTaxicabDistance();
		}
		BSC__NODISCARD constexpr inline double GetTaxicabDistance() const requires (bPoint or bSize) {
			double sum {};
			for (auto v : arr())
				sum += std::abs(v);
			return sum;
		}
		BSC__NODISCARD rad_t GetAngleXY() const requires (bPoint)				{ return rad_t(std::atan2(this->y, this->x)); }
		BSC__NODISCARD rad_t GetAngleYZ() const requires (bPoint and dim >= 3)	{ return rad_t(std::atan2(this->z, this->y)); }
		BSC__NODISCARD rad_t GetAngleZX() const requires (bPoint and dim >= 3)	{ return rad_t(std::atan2(this->x, this->z)); }

		BSC__NODISCARD this_t GetNormalized() const requires (bPoint) { return *this / GetDistance(); }	// Length == 1.0
		bool Normalize() requires (bPoint) { *this /= GetDistance(); return base_t::IsAllValid(); }

		BSC__NODISCARD this_t GetNormalVectorXY() const requires (bPoint)				{ return {this->y, -this->x}; }			// Perpendicular(Normal) Vector (XY-Plan)
		BSC__NODISCARD this_t GetNormalVectorYZ() const requires (bPoint and dim >= 3)	{ return {{}, this->z, -this->y}; }		// Perpendicular(Normal) Vector (YZ-Plan)
		BSC__NODISCARD this_t GetNormalVectorZX() const requires (bPoint and dim >= 3)	{ return {-this->z, {}, this->x}; }		// Perpendicular(Normal) Vector (ZX-Plan)
		// Cross
		BSC__NODISCARD this_t operator * (this_t const& B) const requires (bPoint)			 		{ return Cross(B); }

		//// Mathmatical Operator
		//template < typename T2 >
		//friend this_t operator * (cv::Matx<T2, dim, dim> const& A, this_t const& B) {
		//	this_t C{};
		//	//C.x = A(0, 0) * B.x + A(0, 1) * B.y + A(0, 2) * B.z;
		//	//C.y = A(1, 0) * B.x + A(1, 1) * B.y + A(1, 2) * B.z;
		//	//C.z = A(2, 0) * B.x + A(2, 1) * B.y + A(2, 2) * B.z;
		//	for (int row {}; row < dim; row++) {
		//		for (int col {}; col < dim; col++) {
		//			C.data()[row] += A(row, col) * B.data()[col];
		//		}
		//	}
		//	return C;
		//}

		BSC__NODISCARD this_t Cross(this_t const& B) const requires (bPoint and dim >= 3) {
			using base_t::x, base_t::y, base_t::z;
			return {y*B.z - z*B.y, z*B.x - x*B.z, x*B.y - y*B.x};
		}
		BSC__NODISCARD value_t CrossZ(this_t const& B) const requires (bPoint) {
			return this->x*B.y - this->y*B.x;
		}
		BSC__NODISCARD value_t Dot(this_t const& B) const requires (bPoint) {
			value_t sum{};
			for (size_t i{}; i < count(); i++)
				sum += arr()[i] * B[i];
			return sum;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		// eigen
		BSC__NODISCARD Eigen::Vector<value_t, dim>			vec()			requires (bPoint)				{ 
			static_assert(sizeof(this_t) == sizeof(Eigen::Vector<value_t, dim>));
			return *reinterpret_cast<Eigen::Vector<value_t, dim>*>(this);
		}
		BSC__NODISCARD Eigen::Vector<value_t, dim> const&	vec() const		requires (bPoint)				{ return *reinterpret_cast<Eigen::Vector<value_t, dim> const *>(this); }
		BSC__NODISCARD Eigen::Vector<value_t, 2>			vec2()			requires (bPoint and dim >= 2)	{ return *reinterpret_cast<Eigen::Vector<value_t, 2>*>(this); }
		BSC__NODISCARD Eigen::Vector<value_t, 2> const&		vec2() const	requires (bPoint and dim >= 2)	{ return *reinterpret_cast<Eigen::Vector<value_t, 2> const *>(this); }
		BSC__NODISCARD Eigen::Vector<value_t, 3>			vec3()			requires (bPoint and dim >= 3)	{ return *reinterpret_cast<Eigen::Vector<value_t, 3>*>(this); }
		BSC__NODISCARD Eigen::Vector<value_t, 3> const&		vec3() const	requires (bPoint and dim >= 3)	{ return *reinterpret_cast<Eigen::Vector<value_t, 3> const *>(this); }
		BSC__NODISCARD Eigen::Vector<value_t, 4>			vec4()			requires (bPoint and dim >= 4)	{ return *reinterpret_cast<Eigen::Vector<value_t, 4>*>(this); }
		BSC__NODISCARD Eigen::Vector<value_t, 4> const&		vec4() const	requires (bPoint and dim >= 4)	{ return *reinterpret_cast<Eigen::Vector<value_t, 4> const *>(this); }
		operator Eigen::Vector<value_t, dim>&					()			requires (bPoint)				{ return vec(); }
		operator Eigen::Vector<value_t, dim> const&				() const	requires (bPoint)				{ return vec(); }

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
			if constexpr (concepts::coord::has_##VAR<base_t>) { return (value_t&)base_t::VAR; }\
			else { return arithmetic_dummy_t{}; }\
		}\
		BSC__NODISCARD constexpr inline auto const MemberGet_##VAR() const {\
			if constexpr (concepts::coord::has_##VAR<base_t>) { return (value_t&)base_t::VAR; }\
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

