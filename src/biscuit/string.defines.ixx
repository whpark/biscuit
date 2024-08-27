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
	BSC__NODISCARD constexpr std::basic_string_view<tchar> GetSpaceString() {
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

}	// namespace biscuit
