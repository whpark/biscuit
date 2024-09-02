module;

export module biscuit.coord.base;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;

namespace concepts = biscuit::concepts;

export namespace biscuit {

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename T >
	struct TPoint2 {
		using type_t = biscuit::coord::mPoint_t;
		using this_t = TPoint2;
		using value_t = T;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim>;

		value_t x, y;
	};

	template < typename T >
	struct TPoint3 {
		using type_t = biscuit::coord::mPoint_t;
		using this_t = TPoint3;
		using value_t = T;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim>;

		value_t x, y, z;
	};
	template < typename T >
	struct TPoint4 {
		using type_t = biscuit::coord::mPoint_t;
		using this_t = TPoint4;
		using value_t = T;
		constexpr static int const dim = 4;
		using array_t = std::array<value_t, dim>;

		value_t x, y, z, w;
	};

	template < typename T >
	struct TSize2 {
		using type_t = biscuit::coord::mSize_t;
		using value_t = T;
		using this_t = TSize2;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim>;

		value_t width, height;
	};
	template < typename T >
	struct TSize3 {
		using type_t = biscuit::coord::mSize_t;
		using this_t = TSize3;
		using value_t = T;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim>;

		value_t width, height, depth;
	};

	template < typename T >
	struct TRect2 {
		using type_t = biscuit::coord::mRect_t;
		using this_t = TRect2;
		using value_t = T;
		constexpr static int const dim = 2;
		using array_t = std::array<value_t, dim*2>;

		value_t x, y, width, height;
	};
	template < typename T >
	struct TRect3 {
		using type_t = biscuit::coord::mRect_t;
		using this_t = TRect3;
		using value_t = T;
		constexpr static int const dim = 3;
		using array_t = std::array<value_t, dim*2>;

		value_t x, y, z, width, height, depth;
	};

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tvalue, int DIMENSION, int nMEMBER, typename tcoord_type > 
	struct TCoordBase {
		using value_t = tvalue;
		constexpr static int const dim = DIMENSION;
		constexpr static int size() { return nMEMBER; }
		using coord_type_t = tcoord_type;
		using array_t = std::array<value_t, size()>;
		using this_t = TCoordBase;

		//constexpr auto& arr(this auto&& self) {
		//	if constexpr (std::is_const_v<std::remove_reference_t<decltype(self)>>) {
		//		return *reinterpret_cast<std::array<value_t, DIMENSION>const*>(&self);
		//	}
		//	else {
		//		return *reinterpret_cast<std::array<value_t, DIMENSION>*>(&self);
		//	}
		//}
		constexpr inline array_t& arr() { return *reinterpret_cast<array_t*>(this); }
		constexpr inline array_t const& arr() const { return *reinterpret_cast<array_t const*>(this); }

		// constructors
		TCoordBase() = default;
		TCoordBase(value_t i0) requires(size() == 1) { arr()[0] = i0; }
		TCoordBase(value_t i0, value_t i1) requires(size() == 2) { arr()[0] = i0; arr()[1] = i1; }
		TCoordBase(value_t i0, value_t i1, value_t i2) requires(size() == 3) { arr()[0] = i0; arr()[1] = i1; arr()[2] = i2; }
		TCoordBase(value_t i0, value_t i1, value_t i2, value_t i3) requires(size() == 4) { arr()[0] = i0; arr()[1] = i1; arr()[2] = i2; arr()[3] = i3; }
		TCoordBase(value_t x, value_t y, value_t z, value_t width, value_t height, value_t depth) requires(size() == 6) {
			arr()[0] = x; arr()[1] = y; arr()[2] = z;
			arr()[3] = width; arr()[4] = height; arr()[5] = depth;
		}
		//TCoordBase(this_t const& other) { arr() = other.arr(); }
		//TCoordBase(this_t&& other) { arr() = std::move(other.arr()); }
		//auto& operator = (this_t const& other) { arr() = other.arr(); return *this; }
		//auto& operator = (this_t&& other) { arr() = std::move(other.arr()); return *this; }

		template < concepts::generic_coord tother >
			requires !std::is_same_v<tother, this_t>
		explicit TCoordBase(tother const& b) {
			if constexpr (std::is_same_v<coord_type_t, coord::mRect_t>) {
			}
			else {
			}
		}
		//template < typename tcoord2 >
		//	//requires (!std::is_same_v<tcoord2, this_t> and !std::is_same_v<tcoord2, target_t>)
		//auto& operator = (tcoord2 const& b) {
		//	arr()[0] = b.x;
		//	arr()[1] = b.y;
		//	return *this;
		//}

		auto& operator [] (int i) { return arr()[i]; }
		auto const& operator [] (int i) const { return arr()[i]; }


		template < typename tvalue_to = int >
		TCoordBase<tvalue_to, DIMENSION, nMEMBER, coord_type_t> Round() const {
			if constexpr (dim == 2) {
				return { (tvalue_to)std::round(arr()[0]), (tvalue_to)std::round(arr()[1]) };
			}
			else if constexpr (dim == 3) {
				return { (tvalue_to)std::round(arr()[0]), (tvalue_to)std::round(arr()[1]), (tvalue_to)std::round(arr()[2]) };
			}
			else if constexpr (dim == 4) {
				return { (tvalue_to)std::round(arr()[0]), (tvalue_to)std::round(arr()[1]), (tvalue_to)std::round(arr()[2]), (tvalue_to)std::round(arr()[3]) };
			}
			else {
				static_assert(false);
			}
		}
		template < typename tvalue_to = int >
		TCoordBase<tvalue_to, DIMENSION, nMEMBER, coord_type_t> Floor() const {
			if constexpr (dim == 2) {
				return { (tvalue_to)std::floor(arr()[0]), (tvalue_to)std::floor(arr()[1]) };
			}
			else if constexpr (dim == 3) {
				return { (tvalue_to)std::floor(arr()[0]), (tvalue_to)std::floor(arr()[1]), (tvalue_to)std::floor(arr()[2]) };
			}
			else if constexpr (dim == 4) {
				return { (tvalue_to)std::floor(arr()[0]), (tvalue_to)std::floor(arr()[1]), std::round(arr()[2]), (tvalue_to)std::floor(arr()[3]) };
			}
			else {
				static_assert(false);
			}
		}


	};

}

