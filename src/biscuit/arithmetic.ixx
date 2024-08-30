module;

#include "biscuit/macro.h"

export module biscuit.arithmetic;
import std;
import biscuit.concepts;

//namespace concepts = biscuit::concepts;

export namespace biscuit {

	//-------------------------------------------------------------------------
	// simple math
	// Square
	template < typename T >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T>>)
	BSC__NODISCARD constexpr T Square(T x) { return x * x; }


	//-------------------------------------------------------------------------
	// Round, Ceil, Floor

	/// @brief Round
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	BSC__NODISCARD constexpr auto Round(T_SOURCE v) -> T_DEST {
		return (T_DEST)std::round(v);
	}

	template < typename T_DEST = int, typename T_SOURCE= double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	BSC__NODISCARD constexpr auto Round(T_SOURCE v, T_SOURCE place) -> T_DEST {
		return T_DEST(std::round(v/place*0.1)*place*10);
		//return T_DEST(T_DEST(v/place/10+0.5*(v<T_SOURCE{}?-1:1))*place*10);
	}

	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	BSC__NODISCARD constexpr auto RoundOrForward(T_SOURCE v) -> T_DEST {
		if constexpr (std::is_integral_v<T_SOURCE>) {
			return T_DEST(v);
		}
		else {
			return (T_DEST)std::round(v);
		}
	}

	/// @brief Ceil
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	BSC__NODISCARD constexpr auto Ceil(T_SOURCE v) -> T_DEST {
		if constexpr (std::is_integral_v<T_DEST> && std::is_floating_point_v<T_SOURCE>) {
			return T_DEST(std::ceil(v));	// std::ceil is not constexpr yet.
		}
		else {
			return (T_DEST)v;
		}
	}
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	BSC__NODISCARD constexpr auto Ceil(T_SOURCE v, T_SOURCE place) -> T_DEST {
		return T_DEST(std::ceil(v/place/10)*place*10);
	}

	/// @brief Floor
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	BSC__NODISCARD constexpr auto Floor(T_SOURCE v) -> T_DEST {
		if constexpr (std::is_integral_v<T_DEST> && std::is_floating_point_v<T_SOURCE>) {
			return T_DEST(std::floor(v));	// std::floor is not constexpr yet.
		}
		else {
			return (T_DEST)v;
		}
	}
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	BSC__NODISCARD constexpr auto Floor(T_SOURCE v, T_SOURCE place) -> T_DEST {
		return T_DEST(std::floor(v/place*0.1)*place*10);
	}


}	// namespace biscuit
