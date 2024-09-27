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

export module biscuit.string.defines;
import std;
import biscuit.aliases;
import biscuit.concepts;

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

	template < concepts::string_elem tchar, typename tvalue >
	BSC__NODISCARD constexpr auto GetDefaultFormatString() -> std::basic_format_string<tchar, tvalue> {
		using namespace std::literals;
		if constexpr (std::is_same_v<tchar, char>) { return "{}"; }
		else if constexpr (std::is_same_v<tchar, wchar_t>) { return L"{}"; }
		else if constexpr (std::is_same_v<tchar, char8_t>) { return u8"{}"; }
		else if constexpr (std::is_same_v<tchar, char16_t>) { return u"{}"; }
		else if constexpr (std::is_same_v<tchar, char32_t>) { return U"{}"; }
		else { static_assert(false); }
	}

}	// namespace biscuit

