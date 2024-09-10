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
	template < concepts::string_elem tchar_to, concepts::tstring_like tstring, typename tchar_from = tstring::value_type >
	BSC__NODISCARD auto ConvertString(tstring const& str) -> std::basic_string<tchar_to> {
		//using tchar_from = std::remove_cvref_t<tstring>::value_type;
		//static_assert(concepts::string_elem<tchar_to>);

		// Don't know wchar_t is 2 or 4 bytes...
		if constexpr (sizeof(tchar_from) == sizeof(tchar_to))
			return std::basic_string<tchar_to>((tchar_to const*)str.data(), str.size());
		else if constexpr (sizeof(tchar_from) == sizeof(char)) {	// from char or char8_t
			//if constexpr (sizeof(tchar_to) == sizeof(char)) {
			//	return std::basic_string<tchar_to>((tchar_to const*)str.data(), str.size());
			//} else 
			if constexpr (sizeof(tchar_to) == sizeof(char16_t)) {
				if (auto len = simdutf::utf16_length_from_utf8((char const*)str.data(), str.size())) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf8_to_utf16((char const*)str.data(), str.size(), (char16_t*)result.data());
					return result;
				}
				else return {};
			}
			else if constexpr (sizeof(tchar_to) == sizeof(char32_t)) {
				if (auto len = simdutf::utf32_length_from_utf8((char const*)str.data(), str.size())) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf8_to_utf32((char const*)str.data(), str.size(), (char32_t*)result.data());
					return result;
				}
				else return {};
			}
			else {
				static_assert(false, "Unsupported conversion");
			}
		}
		else if constexpr (sizeof(tchar_from) == sizeof(char16_t))	{ // from char16_t
			if constexpr (sizeof(tchar_to) == sizeof(char)) {
				if (auto len = simdutf::utf8_length_from_utf16((char16_t const*)str.data(), str.size())) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf16_to_utf8((char16_t const*)str.data(), str.size(), (char*)result.data());
					return result;
				}
				else return {};
			}
			//else if constexpr (sizeof(tchar_to) == sizeof(char16_t)) {
			//	return std::basic_string<tchar_to>((tchar_to const*)str.data(), str.size());
			//}
			else if constexpr (sizeof(tchar_to) == sizeof(char32_t)) {
				if (auto len = simdutf::utf32_length_from_utf16((char16_t const*)str.data(), str.size())) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf16_to_utf32((char16_t const*)str.data(), str.size(), (char32_t*)result.data());
					return result;
				}
				else return {};
			}
			else {
				static_assert(false, "Unsupported conversion");
			}
		}
		else if constexpr (sizeof(tchar_from) == sizeof(char32_t)) {	// from char32_t
			if constexpr (sizeof(tchar_to) == sizeof(char)) {
				if (auto len = simdutf::utf8_length_from_utf32((char32_t const*)str.data(), str.size())) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf32_to_utf8((char32_t const*)str.data(), str.size(), (char*)result.data());
					return result;
				}
				else return {};
			}
			else if constexpr (sizeof(tchar_to) == sizeof(char16_t)) {
				if (auto len = simdutf::utf16_length_from_utf32((char32_t const*)str.data(), str.size())) {
					std::basic_string<tchar_to> result(len, 0);
					simdutf::convert_utf32_to_utf16((char32_t const*)str.data(), str.size(), (char16_t*)result.data());
					return result;
				}
				else return {};
			}
			//else if constexpr (sizeof(tchar_to) == sizeof(char32_t)) {
			//	return std::basic_string<tchar_to>((tchar_to const*)str.data(), str.size());
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
		xStringLiteral szCodeTo = "", xStringLiteral szCodeFrom = "", size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertString_iconv(tstring const& strFrom) {
		using tchar_from = std::remove_cvref_t<tstring>::value_type;
		thread_local static Ticonv<tchar_to, tchar_from, initial_dst_buf_size> iconv{szCodeTo.str, szCodeFrom.str};
		return iconv.Convert(strFrom);
	}

	// codepage 949
	template < concepts::string_elem tchar_to, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertStringFromCP949(std::string_view strFrom) {
		return ConvertString_iconv<tchar_to, std::string_view, "", "CP949", initial_dst_buf_size>(strFrom);
	}

	template < concepts::tstring_like tstring, size_t initial_dst_buf_size = 1024 >
	BSC__NODISCARD auto ConvertStringToCP949(tstring const& strFrom) -> std::string {
		using tchar_from = std::remove_cvref_t<tstring>::value_type;
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


}	// namespace biscuit
