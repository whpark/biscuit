module;

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"	// for ToString

export module biscuit.string;

export import biscuit.string.defines;
export import biscuit.string.basic;
export import biscuit.string.compares;
export import biscuit.string.manipulates;
export import biscuit.string.arithmetic;

import std;

export namespace biscuit {

	template < typename tchar = char, typename T >
	BSC__NODISCARD constexpr std::basic_string<tchar> ToString(std::span<T const> arr) {
		if constexpr (std::is_same_v<tchar, char>)			return fmt::format(  "{:n}", arr);
		else if constexpr (std::is_same_v<tchar, char8_t>)	return fmt::format(u8"{:n}", arr);
		else if constexpr (std::is_same_v<tchar, wchar_t>)	return fmt::format( L"{:n}", arr);
		else if constexpr (std::is_same_v<tchar, char16_t>) return fmt::format( u"{:n}", arr);
		else if constexpr (std::is_same_v<tchar, char32_t>) return fmt::format( U"{:n}", arr);
		else { static_assert(false); }
	}

}