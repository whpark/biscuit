module;

//////////////////////////////////////////////////////////////////////
//
// string_manipulates.ixx:
//
// PWH
// 2020.12.16.
// 2024-08-24 biscuit
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"

export module biscuit.string.manipulates;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.convert_codepage;
import biscuit.string.defines;
import biscuit.string.basic;

namespace concepts = biscuit::concepts;
//using namespace std::literals;

namespace biscuit::detail {
	
	/// @brief Split stirng. delimiters must contain '\0' as data not only for null-terminator. using '\0' for searching string end.
	template < typename treturn_item, concepts::tstring_like tstring >
	BSC__NODISCARD auto Split(tstring&& sz, std::basic_string_view<concepts::value_t<tstring>> const& svDelimiters)
		-> std::vector<treturn_item>
	{
		using tchar = concepts::value_t<tstring>;
		using view_t = std::basic_string_view<tchar>;

		static_assert(!std::is_same_v<treturn_item, std::basic_string_view<tchar>> or std::is_lvalue_reference_v<decltype(sz)> );

		std::vector<treturn_item> result;
		if (!std::data(sz) || !std::size(sz))
			return result;
		auto const* cur = std::data(sz);
		auto const* const end = [&] {
				if constexpr (requires (tstring v) { v.size(); }) {
					return cur + sz.size();
				} else {
					return cur + biscuit::tszlen(sz);	// for the case tstring is raw array
				}
			}();
		auto next = std::find_first_of(cur, end, svDelimiters.begin(), svDelimiters.end());
		do {
			result.emplace_back(cur, next);
			if (next >= end)
				break;
			cur = next+1;
			next = std::find_first_of(cur, end, svDelimiters.begin(), svDelimiters.end());
		} while (true);
		return result;
	}

	template < typename tchar, size_t max_digit_len = 0 >
	BSC__NODISCARD constexpr bool CheckDigitLen(std::basic_string<tchar>& str, tchar const*& pos, tchar const* const end, int radix, tchar cFill, tchar cTerminating) {
		if constexpr (max_digit_len != 0) {
			if ( (pos + max_digit_len > end) or (*pos == cTerminating) )
				return false;
		}
		else {
			if ( (pos >= end) or (*pos == cTerminating) )
				return false;
		}

		// check end point
		tchar const* pHexEnd{};
		char32_t v{};
		if constexpr (max_digit_len != 0) {
			v = tsztoi<char32_t>(std::basic_string_view<tchar>{pos, pos+max_digit_len}, &pHexEnd, radix);
			if (pos + max_digit_len != pHexEnd) {
				str += cFill;
				return false;
			}
		}
		else {
			v = tsztoi<char32_t>(std::basic_string_view<tchar>{pos, end}, &pHexEnd, radix);
		}

		// if no digits,
		if (pos == pHexEnd) {
			str += cFill;
			return false;
		}

		if constexpr (std::is_same_v<tchar, char>) {
			str += (v <= 0xff) ? (char)v : cFill;
		}
		else if constexpr (std::is_same_v<tchar, char8_t>) {//, char16_t, char32_t, wchar_t>) {
			if (v <= 0x7f) {
				str += (char8_t)v;
			}
			else {
				str += ConvertString<tchar>(std::basic_string_view<char32_t>(&v, &v+1));
			}
		}
		else if constexpr (std::is_same_v<tchar, charKSSM_t>) {
			str += (v < 0xffff) ? (charKSSM_t)v : cFill;
		}
		else if constexpr (sizeof(tchar) == sizeof(char16_t)) {
			if (v <= 0xffff) {
				str += (char16_t)v;
			} else {
				str += ConvertString<tchar>(std::basic_string_view<char32_t>(&v, &v+1));
			}
		}
		else if constexpr (sizeof(tchar) == sizeof(char32_t)) {
			str += v;
		}
		else {
			static_assert(false, "not supported");
		}

		pos = pHexEnd-1;

		return true;
	}


}	// namespace detail


export namespace biscuit {


	//=============================================================================================================================
	// Trim
	template < typename tchar >
	void TrimLeft(std::basic_string<tchar>& str, std::basic_string_view<tchar> svTrim = GetSpaceString<tchar>()) {
		auto pos = str.find_first_not_of(svTrim);
		if (pos == str.npos)
			str.clear();
		else
			str.erase(str.begin(), str.begin()+pos);
	}
	template < typename tchar >
	void TrimRight(std::basic_string<tchar>& str, std::basic_string_view<tchar> svTrim = GetSpaceString<tchar>()) {
		str.erase(str.begin() + (str.find_last_not_of(svTrim) + 1), str.end());
	}
	//template < concepts::string_elem tchar >
	template < typename tchar >
	void Trim(std::basic_string<tchar>& str, std::basic_string_view<tchar> svTrim = GetSpaceString<tchar>()) {
		TrimRight(str, svTrim);
		TrimLeft(str, svTrim);
	}

	// Trim-View
	template < typename tchar >
	BSC__NODISCARD auto TrimLeftView(std::basic_string_view<tchar> sv, std::basic_string_view<tchar> svTrim = GetSpaceString<tchar>())
		-> std::basic_string_view<tchar>
	{
		if (auto pos = sv.find_first_not_of(svTrim); pos != sv.npos)
			return { sv.begin()+pos, sv.end() };
		else
			return {};
	}
	template < typename tchar >
	BSC__NODISCARD auto TrimRightView(std::basic_string_view<tchar> sv, std::basic_string_view<tchar> svTrim = GetSpaceString<tchar>())
		-> std::basic_string_view<tchar>
	{
		return { sv.begin(), sv.begin() + (sv.find_last_not_of(svTrim)+1) };
	}
	template < typename tchar >
	BSC__NODISCARD auto TrimView(std::basic_string_view<tchar> sv, std::basic_string_view<tchar> svTrim = GetSpaceString<tchar>())
		-> std::basic_string_view<tchar>
	{
		return TrimRightView(TrimLeftView(sv, svTrim), svTrim);
	}

	//=============================================================================================================================
	// Split
	template < concepts::tstring_like tstring >
	BSC__NODISCARD inline auto Split(tstring const& sv, std::basic_string_view<concepts::value_t<tstring>> svDelimiters)
		-> std::vector<std::basic_string<concepts::value_t<tstring>>>
	{
		using tchar = concepts::value_t<tstring>;
		return detail::Split<std::basic_string<tchar>>(sv, svDelimiters);
	}
	template < concepts::tstring_like tstring >
	BSC__NODISCARD inline auto Split(tstring const& sv, concepts::value_t<tstring> cDelimiter)
		-> std::vector<std::basic_string<concepts::value_t<tstring>>>
	{
		using tchar = concepts::value_t<tstring>;
		return detail::Split<std::basic_string<tchar>>(sv, {&cDelimiter, 1uz});
	}
	template < concepts::tstring_like tstring >
	BSC__NODISCARD inline auto SplitView(tstring const& sv, std::basic_string_view<concepts::value_t<tstring>> svDelimiters)
		-> std::vector<std::basic_string_view<concepts::value_t<tstring>>>
	{
		using tchar = concepts::value_t<tstring>;
		return detail::Split<std::basic_string_view<tchar>>(sv, svDelimiters);
	}
	template < concepts::tstring_like tstring >
	BSC__NODISCARD inline auto SplitView(tstring const& sv, concepts::value_t<tstring> cDelimiter)
		-> std::vector<std::basic_string_view<concepts::value_t<tstring>>>
	{
		using tchar = concepts::value_t<tstring>;
		return detail::Split<std::basic_string_view<tchar>>(sv, {&cDelimiter, 1uz});
	}

	//=============================================================================================================================
	// Escape Sequence

	/// @brief Translate Escape Sequence String
	template < typename tchar >
	BSC__NODISCARD constexpr auto TranslateEscapeSequence(std::basic_string_view<tchar> sv, tchar cFill, tchar cTerminating)
		-> std::optional<std::basic_string<tchar>>
	{
		std::basic_string<tchar> str;
		str.reserve(sv.size());
		const tchar* pos{};
		const tchar* const end = sv.data() + sv.size();

		// first, check and count number of bytes to be translated into
		for (pos = sv.data(); (pos < end) and (*pos != cTerminating); pos++) {
			if (*pos == (tchar)'\\') {
				if ( (++pos >= end) or (*pos == cTerminating) )
					return {};
				if (*pos == (tchar)'x') { // Hex ASCII Code
					pos++;
					if (!detail::CheckDigitLen<tchar, 0>(str, pos, end, 16, cFill, cTerminating))
						return {};
				} else if (*pos == (tchar)'u') { // Unicode (16 bit)
					pos++;
					if (!detail::CheckDigitLen<tchar, 4>(str, pos, end, 16, cFill, cTerminating))
						return {};
				} else if (*pos == (tchar)'U') {	// Unicode (32 bit)
					pos++;
					if (!detail::CheckDigitLen<tchar, 8>(str, pos, end, 16, cFill, cTerminating))
						return {};
				} else if (IsOdigit(*pos)) {		// Octal ASCII Code
					if (!detail::CheckDigitLen<tchar, 0>(str, pos, std::min(pos+3, end), 8, cFill, cTerminating))
						return {};
				} else {
					// constexpr map ... (not yet)
					constexpr static const struct {
						tchar cEscape;
						tchar cValue;
					} escapes[] = {
						{ (tchar)'0', (tchar)'\0' },
						{ (tchar)'a', (tchar)'\a' },
						{ (tchar)'b', (tchar)'\b' },
						{ (tchar)'f', (tchar)'\f' },
						{ (tchar)'n', (tchar)'\n' },
						{ (tchar)'r', (tchar)'\r' },
						{ (tchar)'t', (tchar)'\t' },
						{ (tchar)'v', (tchar)'\v' },
					};
					tchar v {*pos};
					auto iter = std::find_if(std::begin(escapes), std::end(escapes), [v](auto p){ return p.cEscape == v;});
					str += (iter == std::end(escapes)) ? v : iter->cValue;
				}
			} else {
				str += *pos;
			}
		}
		return str;
	}

	//=============================================================================================================================
	/// @brief BinaryData to Hex String
	/// @return 
	template < concepts::string_elem tchar_t >
	BSC__NODISCARD auto FormatDataToHexString(std::span<uint8> data, size_t nCol, int cDelimiter, bool bAddText, int cDelimiterText)
		-> std::vector<std::basic_string<tchar_t>>
	{
		std::vector<std::basic_string<tchar_t>> result;

		if (data.size())
			return result;

		if (nCol == 0)
			nCol = data.size();
		//strsResult.clear();
		result.reserve((data.size()-1) / nCol + 1);
		uint8_t const* pos = data.data();
		size_t nLeft = data.size();
		int const nLenDelimiter = (cDelimiter ? 1 : 0);
		int const nLenDelimiterText = (cDelimiterText ? 1 : 0);
		while (nLeft > 0) {
			result.push_back(TString<tchar_t>());
			auto& str = result.back();

			size_t n = std::min(nCol, nLeft);
			if (bAddText) {
				auto len = nCol * (2 + nLenDelimiter) + nLenDelimiterText + nLenDelimiter + n;
				str.reserve(len);	// if cDelimiter == 0, no delimiter would be appended.
			}
			else
				str.resize(n*(2 + (cDelimiter ? 1 : 0)));	// if cDelimiter == 0, no delimiter would be appended.

			auto* pHead = pos;

			//tchar_t* psz = str.data();
			int i = 0;
			for (i = 0; i < n; i++, pos++) {
				auto ch = (*pos >> 4) & 0xF;
				auto cl = *pos & 0xF;
				str += tchar_t((ch > 9) ? (ch-9) + 'A' : ch + '0');
				str += tchar_t((cl > 9) ? (cl-9) + 'A' : cl + '0');
				if (cDelimiter)
					str += (tchar_t)cDelimiter;
			}
			if (bAddText) {
				for (; i < nCol; i++) {
					constinit static tchar_t const szFill[] = { ' ', ' ', 0 };
					str += szFill;
					str += (tchar_t)cDelimiter;
				}

				if (cDelimiterText)
					str += (tchar_t)cDelimiterText;
				if (cDelimiter)
					str += (tchar_t)cDelimiter;

				pos = pHead;
				for (int i = 0; i < n; i++, pos++) {
					if ( *pos && !std::isspace(*pos) && (*pos > ' ') )
						str += (char)*pos;
					else
						str += '.';
				}
			}

			nLeft -= n;
		}
		return result;
	}

}	// namespace biscuit
