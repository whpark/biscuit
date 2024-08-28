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

#include <cerrno>
#include "biscuit/macro.h"

export module biscuit.string.compares;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.string.basic;

namespace concepts = biscuit::concepts;

namespace biscuit {

	struct sToSignedInt {
		int operator () (auto v) const {
			return (int)v;
		}
	};
	struct sToLowerInt {
		template < concepts::string_elem tchar >
		int operator () (tchar v) const {
			if constexpr (sizeof v > 1)
				return std::towlower(v);
			else
				return std::tolower(v);
		}
	};
	struct sToUpperInt {
		template < concepts::string_elem tchar >
		int operator () (tchar v) const {
			if constexpr (sizeof v > 1)
				return std::towupper(v);
			else
				return std::toupper(v);
		}
	};

}

export namespace biscuit {

	namespace detail {

		/// @brief compare string
		/// @return first none zero of (*pszA - *pszB)
		///         0 if pszA == pszB, both empty (nullptr or "")
		template < concepts::string_elem tcharA, concepts::string_elem tcharB, bool bUseCount, class Eval >
			requires (sizeof(tcharA) == sizeof(tcharB))
		BSC__NODISCARD constexpr int tszcmp(tcharA const* pszA, size_t lenA, tcharB const* pszB, size_t lenB) {
			using tchar = std::remove_cvref_t<tcharA>;
			constexpr static Eval eval{};
			if constexpr (bUseCount) {
				if (lenA == 0 and lenB == 0)
					return 0;
				if (lenA == 0)
					return pszB ? -eval(*pszB) : 0;
				if (lenB == 0)
					return pszA ? eval(*pszA) : 0;
				if ( (lenA > RSIZE_MAX) or (lenB > RSIZE_MAX) )
					throw std::invalid_argument(BSC__FUNCSIG "too big!");
			}
			if (!pszA and !pszB)
				return 0;
			if (!pszA)
				return -eval(*pszB);
			if (!pszB)
				return eval(*pszA);

			for (; *pszA && *pszB; pszA++, pszB++) {
				if constexpr (bUseCount) {
					if (lenA == 0 and lenB == 0)
						return 0;
					if (lenA == 0)
						return -eval(*pszB);
					if (lenB == 0)
						return eval(*pszA);
				}

				int r = eval(*pszA) - eval(*pszB);
				if (r)
					return r;

				if constexpr (bUseCount) {
					--lenA; --lenB;
				}
			}
			if (!*pszA && !*pszB)
				return 0;
			int r = eval(*pszA) - eval(*pszB);
			return r;
		}

	}

	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszcmp(tchar const* pszA, tchar const* pszB) {
		return detail::tszcmp<tchar, tchar, false, sToSignedInt>(pszA, {}, pszB, {});
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszncmp(tchar const* pszA, tchar const* pszB, size_t nCount) {
		return detail::tszcmp<tchar, tchar, true, sToSignedInt>(pszA, nCount, pszB, nCount);
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszicmp(tchar const* pszA, tchar const* pszB) {
		return detail::tszcmp<tchar, tchar, false, sToLowerInt>(pszA, {}, pszB, {});
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tsznicmp(tchar const* pszA, tchar const* pszB, size_t nCount) {
		return detail::tszcmp<tchar, tchar, true, sToLowerInt>(pszA, nCount, pszB, nCount);
	}

	template < concepts::tchar_string_like tstringA, concepts::tchar_string_like tstringB >
		requires concepts::have_same_tchar<tstringA, tstringB>
	BSC__NODISCARD constexpr int tszcmp(tstringA const& svA, tstringB const& svB) {
		using tcharA = concepts::value_t<tstringA>;
		using tcharB = concepts::value_t<tstringB>;
		return detail::tszcmp<tcharA, tcharB, true, sToSignedInt>(
			std::data(svA), std::size(svA),
			std::data(svB), std::size(svB));
	}
	template < concepts::tchar_string_like tstringA, concepts::tchar_string_like tstringB >
		requires concepts::have_same_tchar<tstringA, tstringB>
	BSC__NODISCARD constexpr int tszncmp(tstringA const& svA, tstringB const& svB, size_t nCount) {
		using tcharA = concepts::value_t<tstringA>;
		using tcharB = concepts::value_t<tstringB>;
		return detail::tszcmp<tcharA, tcharB, true, sToSignedInt>(
			std::data(svA), std::min(nCount, std::size(svA)),
			std::data(svB), std::min(nCount, std::size(svB)));
	}
	template < concepts::tchar_string_like tstringA, concepts::tchar_string_like tstringB >
		requires concepts::have_same_tchar<tstringA, tstringB>
	BSC__NODISCARD constexpr int tszicmp(tstringA const& svA, tstringB const& svB) {
		using tcharA = concepts::value_t<tstringA>;
		using tcharB = concepts::value_t<tstringB>;
		return detail::tszcmp<tcharA, tcharB, true, sToLowerInt>(
			std::data(svA), std::size(svA),
			std::data(svB), std::size(svB));
	}
	template < concepts::tchar_string_like tstringA, concepts::tchar_string_like tstringB >
		requires concepts::have_same_tchar<tstringA, tstringB>
	BSC__NODISCARD constexpr int tsznicmp(tstringA const& svA, tstringB const& svB, size_t nCount) {
		using tcharA = concepts::value_t<tstringA>;
		using tcharB = concepts::value_t<tstringB>;
		return detail::tszcmp<tcharA, tcharB, true, sToLowerInt>(
			std::data(svA), std::min(nCount, std::size(svA)),
			std::data(svB), std::min(nCount, std::size(svB)));
	}


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
	BSC__NODISCARD constexpr inline int CompareStringContainingNumbers(std::basic_string_view<tchar> strA, std::basic_string_view<tchar> strB, bool bIgnoreCase) {
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
