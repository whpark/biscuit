module;

//////////////////////////////////////////////////////////////////////
//
// string_convert_codepage.ixx:
//
// PWH
// 2020.12.16.
// 2024-08-22 biscuit
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_simdutf.h"

export module biscuit.convert_codepage;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.misc;
import biscuit.iconv_wrapper;

namespace concepts = biscuit::concepts;
using namespace std::literals;

export namespace biscuit {

	BSC__NODISCARD auto	u8A(std::string& str)		-> std::u8string&		{ return reinterpret_cast<std::u8string&>(str); }
	BSC__NODISCARD auto	Au8(std::u8string& str)		-> std::string&			{ return reinterpret_cast<std::string&>(str); }
	BSC__NODISCARD auto	Au8(std::string_view str)	-> std::u8string_view	{ return reinterpret_cast<std::u8string_view&>(str); }
	BSC__NODISCARD auto	u8A(std::u8string_view str) -> std::string_view		{ return reinterpret_cast<std::string_view&>(str); }


	//-----------------------------------------------------------------------------
	/// @brief just char -> tchar_t (this function does not change sv's values.)
	template < typename tcharTarget, typename tcharSource >
	constexpr std::basic_string<tcharTarget> ElevateAnsiToStandard(std::basic_string_view<tcharSource> sv) {
		std::basic_string<tcharTarget> result;
		result.reserve(sv.size());
		for (auto c : sv)
			result += static_cast<tcharTarget>(c);
		return result;
	}


	//=============================================================================================================================
	// Convert String
	template < concepts::string_elem tchar_to, concepts::tstring_like tstring >
	BSC__NODISCARD auto ConvertString(tstring const& str) -> std::optional<std::basic_string<tchar_to>> {
		using char_from_t = concepts::value_t<tstring>;
		static_assert(concepts::string_elem<char_from_t>);
		char_from_t const* data = std::data(str);
		auto l = std::size(str);
		if constexpr (std::is_array_v<tstring>) {
			if (l > 0 and str[l - 1] == 0)
				l--;
		}
		if (l == 0)
			return std::basic_string<tchar_to>{};

		// Don't know wchar_t is 2 or 4 bytes...
		if constexpr (sizeof(char_from_t) == sizeof(tchar_to))
			return std::basic_string<tchar_to>((tchar_to const*)data, l);
		else if constexpr (sizeof(char_from_t) == sizeof(char)) {	// from char or char8_t
			//if constexpr (sizeof(tchar_to) == sizeof(char)) {
			//	return std::basic_string<tchar_to>((tchar_to const*)data, str.size());
			//} else 
			if constexpr (sizeof(tchar_to) == sizeof(char16_t)) {
				if (auto len = simdutf::utf16_length_from_utf8((char const*)data, l)) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf8_to_utf16((char const*)data, l, (char16_t*)result.data());
					return result;
				}
				else return {};
			}
			else if constexpr (sizeof(tchar_to) == sizeof(char32_t)) {
				if (auto len = simdutf::utf32_length_from_utf8((char const*)data, l)) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf8_to_utf32((char const*)data, l, (char32_t*)result.data());
					return result;
				}
				else return {};
			}
			else {
				static_assert(false, "Unsupported conversion");
			}
		}
		else if constexpr (sizeof(char_from_t) == sizeof(char16_t))	{ // from char16_t
			if constexpr (sizeof(tchar_to) == sizeof(char)) {
				if (auto len = simdutf::utf8_length_from_utf16((char16_t const*)data, l)) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf16_to_utf8((char16_t const*)data, l, (char*)result.data());
					return result;
				}
				else return {};
			}
			//else if constexpr (sizeof(tchar_to) == sizeof(char16_t)) {
			//	return std::basic_string<tchar_to>((tchar_to const*)data, l);
			//}
			else if constexpr (sizeof(tchar_to) == sizeof(char32_t)) {
				if (auto len = simdutf::utf32_length_from_utf16((char16_t const*)data, l)) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf16_to_utf32((char16_t const*)data, l, (char32_t*)result.data());
					return result;
				}
				else return {};
			}
			else {
				static_assert(false, "Unsupported conversion");
			}
		}
		else if constexpr (sizeof(char_from_t) == sizeof(char32_t)) {	// from char32_t
			if constexpr (sizeof(tchar_to) == sizeof(char)) {
				if (auto len = simdutf::utf8_length_from_utf32((char32_t const*)data, l)) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf32_to_utf8((char32_t const*)data, l, (char*)result.data());
					return result;
				}
				else return {};
			}
			else if constexpr (sizeof(tchar_to) == sizeof(char16_t)) {
				if (auto len = simdutf::utf16_length_from_utf32((char32_t const*)data, l)) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf32_to_utf16((char32_t const*)data, l, (char16_t*)result.data());
					return result;
				}
				else return {};
			}
			//else if constexpr (sizeof(tchar_to) == sizeof(char32_t)) {
			//	return std::basic_string<tchar_to>((tchar_to const*)data, l);
			//}
			else {
				static_assert(false, "Unsupported conversion");
			}
		}
		else {
			static_assert(false, "Unsupported conversion");
		}
	}

	//=============================================================================================================================
	// Convert String using iconv
	template < concepts::string_elem tchar_to, concepts::tstring_like tstring,
		xStringLiteral szCodepageTo = "", xStringLiteral szCodepageFrom = "", size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertString_iconv(tstring const& strFrom) {
		using char_from_t = concepts::value_t<tstring>;
		thread_local static Ticonv<tchar_to, char_from_t, initial_dst_buf_size> iconv(szCodepageTo.str, szCodepageFrom.str);
		return iconv.Convert(strFrom);
	}

	template < concepts::string_elem tchar_to, concepts::tstring_like tstring, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertString_iconv(tstring const& strFrom, std::string szCodepageTo, std::string szCodepageFrom = "") {
		using char_from_t = concepts::value_t<tstring>;
		using iconv_type = Ticonv<tchar_to, char_from_t, initial_dst_buf_size>;
		thread_local static std::map<std::pair<std::string, std::string>, iconv_type> map_iconv;
		if (szCodepageTo.empty())
			szCodepageTo = iconv_type::template GuessCodepageFromType<tchar_to>();
		if (szCodepageFrom.empty())
			szCodepageFrom = iconv_type::template GuessCodepageFromType<char_from_t>();

		if (map_iconv.find({ szCodepageTo, szCodepageFrom }) == map_iconv.end())
			map_iconv.emplace(std::pair{szCodepageTo, szCodepageFrom}, iconv_type(szCodepageTo, szCodepageFrom));
		auto& iconv = map_iconv[{szCodepageTo, szCodepageFrom}];
		return iconv.Convert(strFrom);
	}

	// codepage 949
	template < concepts::string_elem tchar_to, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertStringFromCP949(std::string_view strFrom) {
		return ConvertString_iconv<tchar_to, std::string_view, "", "CP949", initial_dst_buf_size>(strFrom);
	}

	template < concepts::tstring_like tstring, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertStringToCP949(tstring const& strFrom) -> std::string {
		using char_from_t = std::remove_cvref_t<tstring>::value_type;
		return ConvertString_iconv<char, tstring, "CP949", "", initial_dst_buf_size>(strFrom);
	}

	// KSSM (JOHAB)
	template < concepts::string_elem tchar_to, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertStringFromKSSM(std::basic_string_view<charKSSM_t> strFrom) {
		return ConvertString_iconv<tchar_to, std::basic_string_view<charKSSM_t>, "", "JOHAB", initial_dst_buf_size>(strFrom);
	}

	template < concepts::tstring_like tstring, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertStringToKSSM(tstring const& strFrom) {
		return ConvertString_iconv<char, tstring, "JOHAB", "", initial_dst_buf_size>(strFrom);
	}

	template < concepts::string_elem tchar_to, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertStringFromKSSM_MBCS(std::string_view strFrom) {
		return ConvertString_iconv<tchar_to, std::string_view, "", "JOHAB", initial_dst_buf_size>(strFrom);
	}

	template < concepts::tstring_like tstring, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertStringToKSSM_MBCS(tstring const& strFrom) {
		return ConvertString_iconv<char, tstring, "JOHAB", "", initial_dst_buf_size>(strFrom);
	}


	//=============================================================================================================================
	// Compares
	std::strong_ordering Compare(concepts::tstring_like auto const& strA, concepts::tstring_like auto const& strB) {
		using strA_t = std::remove_cvref_t<decltype(strA)>;
		using strB_t = std::remove_cvref_t<decltype(strB)>;
		using charA_t = concepts::value_t<strA_t>;
		using charB_t = concepts::value_t<strB_t>;
		using strA_view_t = std::basic_string_view<charA_t>;
		using strB_view_t = std::basic_string_view<charB_t>;
		auto lenA = std::size(strA);
		auto lenB = std::size(strB);
		if constexpr (std::is_array_v<strA_t>) {
			if (lenA > 0 and strA[lenA - 1] == 0)
				lenA--;
		}
		if constexpr (std::is_array_v<strB_t>) {
			if (lenB > 0 and strB[lenB - 1] == 0)
				lenB--;
		}
		strA_view_t svA(std::data(strA), lenA);
		strB_view_t svB(std::data(strB), lenB);

		if constexpr (sizeof(charA_t) == sizeof(charB_t)) {
		#pragma warning(suppress: 4244)
			return (std::strong_ordering)(svA.compare((strA_view_t&)svB));
		}
		if (lenA*sizeof(charA_t) <= lenB*sizeof(charB_t)) {
			auto strA2 = ConvertString<charB_t>(svA).value_or(std::basic_string<charB_t>{});
		#pragma warning(suppress: 4244)
			return (std::strong_ordering)(strA2.compare(svB));
		}
		else {
			auto strB2 = ConvertString<charA_t>(svB).value_or(std::basic_string<charA_t>{});
		#pragma warning(suppress: 4244)
			return (std::strong_ordering)(svA.compare(strB2));
		}
	}


}	// namespace biscuit

