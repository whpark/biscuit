module;

//////////////////////////////////////////////////////////////////////
//
// string.ixx:
//
// PWH
// 2020.12.16.
// 2024-08-22 biscuit
//
//////////////////////////////////////////////////////////////////////

#include <simdutf.h>

export module biscuit:string;
import std.compat;
import :aliases;
import :concepts;

namespace concepts = biscuit::concepts;

export namespace biscuit {

	//template<typename ZLike, template<class...>class Z>
	//concept can_range_become = requires (const ZLike& s) {
	//	{ Z{std::ranges::begin(s), std::ranges::end(s)} };
	//};
	//template<typename T>
	//concept stringlike = can_range_become<T, std::basic_string_view> && std::ranges::contiguous_range<T>;

	[[nodiscard]] std::u8string& u8A(std::string& str) { return reinterpret_cast<std::u8string&>(str); }
	[[nodiscard]] std::string& Au8(std::u8string& str) { return reinterpret_cast<std::string&>(str); }
	[[nodiscard]] std::u8string_view Au8(std::string_view str) { return reinterpret_cast<std::u8string_view&>(str); }
	[[nodiscard]] std::string_view u8A(std::u8string_view str) { return reinterpret_cast<std::string_view&>(str); }

	std::basic_string_view<char> sv;

	// Convert String
	template < concepts::string_elem tchar_to, concepts::tchar_string_like tstring, typename tchar_from = tstring::value_type >
	[[nodiscard]] std::basic_string<tchar_to> ConvertString(tstring const& str) {
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

}	// namespace biscuit
