module;

//////////////////////////////////////////////////////////////////////
//
// string_compares.ixx:
//
// PWH
// 2020.12.16.
// 2024-08-27 biscuit
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"

export module biscuit.string.compares;
import std;
import biscuit.aliases;
import biscuit.concepts;


namespace concepts = biscuit::concepts;

namespace biscuit {

	template < concepts::string_elem tchar, bool bUseCount, class Eval >
	BSC__NODISCARD constexpr int tszcmp(tchar const* pszA, tchar const* pszB, size_t nCount, Eval&& eval) {
		if constexpr (bUseCount) {
			if (!nCount)
				return 0;
			if (nCount > RSIZE_MAX)
				throw std::invalid_argument(BSC__FUNCSIG "nCount is too big!");
		}
		if (!pszA && !pszB)	// if both are nullptr, return 0;
			return 0;
		if (pszA && !pszB)	// if only one has value, its bigger.
			return *pszA ? *pszA : 0;//1;
		else if (!pszA && pszB)
			return *pszB ? -*pszB : 0;//-1;

		for (; *pszA && *pszB; pszA++, pszB++) {
			int r = eval(*pszA) - eval(*pszB);
			if (r)
				return r;
			if constexpr (bUseCount) {
				if (--nCount == 0)
					return 0;
			}
		}
		if (!*pszA && !*pszB)
			return 0;
		int r = eval(*pszA) - eval(*pszB);
		return r;
	}

	template < concepts::string_elem tchar, bool bUseCount, class Eval >
	BSC__NODISCARD constexpr int tszcmp(std::basic_string_view<tchar> svA, std::basic_string_view<tchar> svB, size_t nCount, Eval&& eval) {
		if constexpr (bUseCount) {
			if (!nCount)
				return 0;
			if (nCount > RSIZE_MAX)
				throw std::invalid_argument(BSC__FUNCSIG "nCount is too big!");
		}
		bool bEmptyA = svA.empty();
		bool bEmptyB = svB.empty();
		if (bEmptyA || bEmptyB) {
			if (bEmptyA and bEmptyB)
				return 0;
			if (!bEmptyA)
				return 1;
			if (!bEmptyB)
				return -1;
		}
		tchar const* pszA = svA.data();
		tchar const* pszB = svB.data();
		tchar const* const pszAEnd = svA.data() + svA.size();
		tchar const* const pszBEnd = svB.data() + svB.size();

		for (; pszA < pszAEnd and pszB < pszBEnd; pszA++, pszB++) {
			int r = eval(*pszA) - eval(*pszB);
			if (r)
				return r;
			if constexpr (bUseCount) {
				if (--nCount == 0)
					return 0;
			}
			if (!*pszA)	[[unlikely]]
				return 0;
		}
		//if ((pszA < pszAEnd) and (pszB < pszBEnd)) [[unlkely]] // actually, never happens here.
		//{
		//	
		//	return eval(*pszA, *pszB);
		//}
		//else {
		if (pszA < pszAEnd)
			return eval(*pszA);
		if (pszB < pszBEnd)
			return -eval(*pszB);
		//}

		return 0;
	}

}

export namespace biscuit {

	template < typename tchar > requires std::is_integral_v<tchar>
	BSC__NODISCARD constexpr inline bool IsDigit(tchar const c) { return (c >= '0') && (c <= '9'); }
	template < typename tchar > requires std::is_integral_v<tchar>
	BSC__NODISCARD constexpr inline bool IsOdigit(tchar const c) { return (c >= '0') && (c <= '7'); }
	template < typename tchar > requires std::is_integral_v<tchar>
	BSC__NODISCARD constexpr inline bool IsXdigit(tchar const c) { return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')); }
	template < typename tchar > requires std::is_integral_v<tchar>
	BSC__NODISCARD constexpr inline bool IsSpace(tchar const c) { return (c == '\t') || (c == '\r') || (c == '\n') || (c == ' '); }
	template < typename tchar > requires std::is_integral_v<tchar>
	BSC__NODISCARD constexpr inline bool IsNotSpace(tchar const c) { return !IsSpace(c); }




	/// @brief Compare two Strings. according to number (only for '0'-'9' are taken as number. no '-' sign, neither '.' for floating point 
	///  ex)
	///      "123" > "65"         // 123 > 65
	///      "abc123" > "abc6"    // 123 > 65 ("abc" == "abc")
	///      "-100" < "-125"      // '-' is just taken as part of string. ===> 100 < 125
	///      "00001" < "0000003"  // 1 < 3
	///      "01" < "001"         // if same (1 == 1) ===> longer gets bigger value.
	/// @param pszA
	/// @param pszB
	/// @return 
	template < bool bIgnoreCase = false, typename tchar >
	BSC__NODISCARD constexpr int CompareStringContainingNumbers(std::basic_string_view<tchar> svA, std::basic_string_view<tchar> svB) {
		auto const* pszA			= svA.data();
		auto const* const pszAend	= svA.data() + svA.size();
		auto const* pszB			= svB.data();
		auto const* const pszBend	= svB.data() + svB.size();

		for (; (pszA < pszAend) && (pszB < pszBend); pszA++, pszB++) {
			if (IsDigit(*pszA) && IsDigit(*pszB)) {	// for numbers
				tchar const* const pszAs {pszA};
				tchar const* const pszBs {pszB};

				// skip '0'
				auto Skip0 = [](tchar const*& psz, tchar const* const pszEnd) {
					for ( ; (psz < pszEnd) and ('0' == *psz); psz++)
						;
					};
				Skip0(pszA, pszAend);
				Skip0(pszB, pszBend);

				// Count Digit Length
				auto CountDigitLength = [](tchar const*& psz, tchar const* const pszEnd) -> int {
					int nDigit{};
					for (; (psz + nDigit < pszEnd) and IsDigit(psz[nDigit]); nDigit++)
						;
					return nDigit;
					};
				int const nDigitA = CountDigitLength(pszA, pszAend);
				int const nDigitB = CountDigitLength(pszB, pszBend);

				// Compare digit length
				if (auto diff = nDigitA - nDigitB; diff)
					return diff;

				if (nDigitA == 0) {
					continue;
				}
				for (int nDigit {nDigitA}; (nDigit > 0); pszA++, pszB++, nDigit--) {
					if (*pszA == *pszB)
						continue;
					return *pszA - *pszB;
				}
				if (auto diff = (pszA-pszAs) - (pszB-pszBs); diff)
					return (int)diff;
				pszA--;
				pszB--;
			} else {
				tchar cA = *pszA;
				tchar cB = *pszB;
				if constexpr (bIgnoreCase) { cA = (tchar)ToLower(cA); cB = (tchar)ToLower(cB); }
				auto r = cA - cB;
				if (r == 0)
					continue;
				return r;
			}
		}
		if (pszA < pszAend)
			return *pszA;
		else if (pszB < pszBend)
			return -*pszB;

		return 0;
	}

	template < typename tchar >
	BSC__NODISCARD constexpr int CompareStringContainingNumbers(std::basic_string_view<tchar> strA, std::basic_string_view<tchar> strB, bool bIgnoreCase) {
		return bIgnoreCase
			? CompareStringContainingNumbers<true>(strA, strB)
			: CompareStringContainingNumbers<false>(strA, strB);
	}

	template < typename tchar >
	BSC__NODISCARD constexpr inline int tdszcmp(std::basic_string_view<tchar> svA, std::basic_string_view<tchar> svB) {
		return CompareStringContainingNumbers<false>(svA, svB);
	}
	template < typename tchar >
	BSC__NODISCARD constexpr inline int tdszicmp(std::basic_string_view<tchar> svA, std::basic_string_view<tchar> svB) {
		return CompareStringContainingNumbers<true>(svA, svB);
	}

}	// namespace biscuit
