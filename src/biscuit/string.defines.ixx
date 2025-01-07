module;

//////////////////////////////////////////////////////////////////////
//
// string_defines.ixx:
//
// PWH
// 2020.12.16.
// 2024-08-24 biscuit
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
//#include "biscuit/dependencies_fmt.h"

export module biscuit.string.defines;
import std;
import fmt;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.misc;

namespace concepts = biscuit::concepts;
//using namespace std::literals;

export namespace biscuit {

	//template < concepts::string_elem tchar >
	template < typename tchar >
	BSC__NODISCARD constexpr inline auto GetSpaceString() -> std::basic_string_view<tchar> {
	#define SPACES " \t\r\n"
		if constexpr (std::is_same_v<tchar, char>)			{ return SPACES; }
		else if constexpr (std::is_same_v<tchar, char8_t>)	{ return u8"" SPACES; }
		else if constexpr (std::is_same_v<tchar, char16_t>)	{ return u"" SPACES; }
		else if constexpr (std::is_same_v<tchar, char32_t>)	{ return U"" SPACES; }
		else if constexpr (std::is_same_v<tchar, wchar_t>)	{ return L"" SPACES; }
		else {
			static_assert(false, "Unsupported type");
		}
	#undef SPACES
	}

	template < concepts::string_elem tchar_to, xStringLiteral literal, typename ... targs >
	std::basic_string<tchar_to> FormatToTString(targs&& ... args) {
		fmt::basic_format_string<tchar_to, targs...>{TStringLiteral<tchar_to, literal>{}.value};	// Compiltime Validation only
		constexpr TStringLiteral<tchar_to, literal> tfmt{};
		if constexpr (concepts::is_one_of<tchar_to, char, wchar_t>) {
			return fmt::format(fmt::runtime(tfmt.value), std::forward<targs>(args)...);
		}
		else {
			return fmt::format(tfmt.value, std::forward<targs>(args)...);
		}
	}

	//template < typename tchar = char, concepts::arithmetic T >
	//BSC__NODISCARD constexpr std::basic_string<tchar> ToString(std::span<T const> arr) {
	//	if constexpr (std::is_same_v<tchar, char>)			return fmt::format(  "{:n}", arr);
	//	else if constexpr (std::is_same_v<tchar, char8_t>)	return fmt::format(u8"{:n}", arr);
	//	else if constexpr (std::is_same_v<tchar, wchar_t>)	return fmt::format( L"{:n}", arr);
	//	else if constexpr (std::is_same_v<tchar, char16_t>) return fmt::format( u"{:n}", arr);
	//	else if constexpr (std::is_same_v<tchar, char32_t>) return fmt::format( U"{:n}", arr);
	//	else { static_assert(false); }
	//}

	//template < concepts::string_elem tchar, typename tvalue >
	//BSC__NODISCARD constexpr auto GetDefaultFormatString() -> fmt::basic_format_string<tchar, tvalue> {
	//	using namespace std::literals;
	//	if constexpr (std::is_same_v<tchar, char>) { return "{}"; }
	//	else if constexpr (std::is_same_v<tchar, wchar_t>) { return L"{}"; }
	//	else if constexpr (std::is_same_v<tchar, char8_t>) { return u8"{}"; }
	//	else if constexpr (std::is_same_v<tchar, char16_t>) { return u"{}"; }
	//	else if constexpr (std::is_same_v<tchar, char32_t>) { return U"{}"; }
	//	else { static_assert(false); }
	//}

}	// namespace biscuit

