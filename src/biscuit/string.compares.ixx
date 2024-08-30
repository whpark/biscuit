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

export namespace biscuit {

	namespace detail {

		/// @brief compare string
		/// @return first none zero of (*pszA - *pszB)
		///         0 if pszA == pszB, both empty (nullptr or "")
		template < class TEvaluator, concepts::string_elem tcharA, concepts::string_elem tcharB>
			requires (sizeof(tcharA) == sizeof(tcharB))
		BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszcmp(tcharA const* pszA, tcharB const* pszB) {
			using tchar = std::remove_cvref_t<tcharA>;
			constexpr static TEvaluator eval{};
			if (!pszA and !pszB)
				return 0;
			if (!pszA)
				return -eval(*pszB);
			if (!pszB)
				return eval(*pszA);

			for (; *pszA && *pszB; pszA++, pszB++) {
				if (auto r = eval(*pszA) - eval(*pszB))
					return r;
			}
			int r = eval(*pszA) - eval(*pszB);
			return r;
		}

		/// @brief compare string
		/// @return first none zero of (*pszA - *pszB)
		///         0 if pszA == pszB, both empty (nullptr or "")
		template < class TEvaluator, concepts::string_elem tcharA, concepts::string_elem tcharB >
			requires (sizeof(tcharA) == sizeof(tcharB))
		BSC__NODISCARD constexpr int tszcmp(std::span<tcharA const> szA, std::span<tcharB const> szB) {
			using tchar = std::remove_cvref_t<tcharA>;
			constexpr static TEvaluator eval{};
			tcharA const* pszA = szA.data();
			tcharB const* pszB = szB.data();
			size_t sizeA = szA.size();
			size_t sizeB = szB.size();

			if (sizeA == 0 and sizeB == 0)
				return 0;
			if ( (sizeA > RSIZE_MAX) or (sizeB > RSIZE_MAX) )	// check validity first, before accessing pszA or pszB
				throw std::invalid_argument(BSC__FUNCSIG "too big!");
			if (sizeA == 0)
				return pszB ? -eval(*pszB) : 0;
			if (sizeB == 0)
				return pszA ? eval(*pszA) : 0;

			if (!pszA and !pszB)
				return 0;
			if (!pszA)
				return -eval(*pszB);
			if (!pszB)
				return eval(*pszA);

			for ( ; ; pszA++, pszB++, sizeA--, sizeB--) {
				tchar a = sizeA ? *pszA : 0;
				tchar b = sizeB ? *pszB : 0;
				if (auto r = eval(a) - eval(b))
					return r;
				if (a == 0 or b == 0)	// Actually, just need to check if either a or b is 0.
					break;
			}
			return 0;
		}
	}

	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszcmp(tchar const* pszA, tchar const* pszB) {
	#pragma warning(disable: 4996)
		return detail::tpszcmp<detail::TToTransparent<int>>(pszA, pszB);
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszncmp(tchar const* pszA, tchar const* pszB, size_t nMaxCount) {
		return detail::tszcmp<detail::TToTransparent<int>>({pszA, nMaxCount}, {pszB, nMaxCount});
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszicmp(tchar const* pszA, tchar const* pszB) {
	#pragma warning(disable: 4996)
		return detail::tpszcmp<detail::TToLower<int>>(pszA, pszB);
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tsznicmp(tchar const* pszA, tchar const* pszB, size_t nMaxCount) {
		return detail::tszcmp<detail::TToLower<int>>({pszA, nMaxCount}, {pszB, nMaxCount});
	}

	template < concepts::string_elem tchar, concepts::tstring_like tstring >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszcmp(tchar const* pszA, tstring const& svB) {
		static_assert(concepts::have_same_tchar<std::basic_string<tchar>, tstring>);
		using tcharB = concepts::value_t<tstring>;
		return detail::tszcmp<detail::TToTransparent<int>, tchar, tcharB>({pszA, RSIZE_MAX}, {std::data(svB), std::size(svB)});
	}
	template < concepts::string_elem tchar, concepts::tstring_like tstring >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszncmp(tchar const* pszA, tstring const& svB, size_t nMaxCount) {
		static_assert(concepts::have_same_tchar<std::basic_string<tchar>, tstring>);
		using tcharB = concepts::value_t<tstring>;
		return detail::tszcmp<detail::TToTransparent<int>, tchar, tcharB>({pszA, nMaxCount}, {std::data(svB), std::min(std::size(svB), nMaxCount)});
	}
	template < concepts::string_elem tchar, concepts::tstring_like tstring >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tpszicmp(tchar const* pszA, tstring const& svB) {
		static_assert(concepts::have_same_tchar<std::basic_string<tchar>, tstring>);
		using tcharB = concepts::value_t<tstring>;
		return detail::tszcmp<tchar, tchar, true, detail::TToLower<int>, tchar, tcharB >({pszA, RSIZE_MAX}, {std::data(svB), std::size(svB)});
	}
	template < concepts::string_elem tchar, concepts::tstring_like tstring >
	BSC__NODISCARD BSC__DEPR_SEC constexpr int tsznicmp(tchar const* pszA, tstring const& svB, size_t nMaxCount) {
		static_assert(concepts::have_same_tchar<std::basic_string<tchar>, tstring>);
		using tcharB = concepts::value_t<tstring>;
		return detail::tszcmp<tchar, tchar, true, detail::TToLower<int>, tchar, tcharB >({pszA, nMaxCount}, {std::data(svB), std::min(std::size(svB), nMaxCount)});
	}

	template < concepts::tstring_like tstringA, concepts::tstring_like tstringB >
		requires concepts::have_same_tchar<tstringA, tstringB>
	BSC__NODISCARD constexpr int tszcmp(tstringA const& svA, tstringB const& svB) {
		using tcharA = concepts::value_t<tstringA>;
		using tcharB = concepts::value_t<tstringB>;
		return detail::tszcmp<detail::TToTransparent<int>, tcharA, tcharB>(
			{std::data(svA), std::size(svA)},
			{std::data(svB), std::size(svB)});
	}
	template < concepts::tstring_like tstringA, concepts::tstring_like tstringB >
		requires concepts::have_same_tchar<tstringA, tstringB>
	BSC__NODISCARD constexpr int tszncmp(tstringA const& svA, tstringB const& svB, size_t nMaxCount) {
		using tcharA = concepts::value_t<tstringA>;
		using tcharB = concepts::value_t<tstringB>;
		return detail::tszcmp<detail::TToTransparent<int>, tcharA, tcharB>(
			{std::data(svA), std::min(nMaxCount, std::size(svA))},
			{std::data(svB), std::min(nMaxCount, std::size(svB))});
	}
	template < concepts::tstring_like tstringA, concepts::tstring_like tstringB >
		requires concepts::have_same_tchar<tstringA, tstringB>
	BSC__NODISCARD constexpr int tszicmp(tstringA const& svA, tstringB const& svB) {
		using tcharA = concepts::value_t<tstringA>;
		using tcharB = concepts::value_t<tstringB>;
		return detail::tszcmp<detail::TToLower<int>, tcharA, tcharB>(
			{std::data(svA), std::size(svA)},
			{std::data(svB), std::size(svB)});
	}
	template < concepts::tstring_like tstringA, concepts::tstring_like tstringB >
		requires concepts::have_same_tchar<tstringA, tstringB>
	BSC__NODISCARD constexpr int tsznicmp(tstringA const& svA, tstringB const& svB, size_t nMaxCount) {
		using tcharA = concepts::value_t<tstringA>;
		using tcharB = concepts::value_t<tstringB>;
		return detail::tszcmp<detail::TToLower<int>, tcharA, tcharB>(
			{std::data(svA), std::min(nMaxCount, std::size(svA))},
			{std::data(svB), std::min(nMaxCount, std::size(svB))});
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
	template < bool bIgnoreCase = false >
	BSC__NODISCARD constexpr int CompareNumericStrings(concepts::tstring_like auto const& svA, concepts::tstring_like auto const& svB)
		requires (concepts::have_same_tchar<decltype(svA), decltype(svB)>)
	{
		using tcharA = concepts::value_t<decltype(svA)>;
		using tcharB = concepts::value_t<decltype(svB)>;

		auto const* pszA			= svA.data();
		auto const* const pszAend	= svA.data() + svA.size();
		auto const* pszB			= svB.data();
		auto const* const pszBend	= svB.data() + svB.size();

		for (; (pszA < pszAend) && (pszB < pszBend); pszA++, pszB++) {
			if (IsDigit(*pszA) && IsDigit(*pszB)) {	// for numbers
				tcharA const* const pszAs {pszA};
				tcharB const* const pszBs {pszB};

				// skip '0'
				auto Skip0 = [](auto const*& psz, auto const* const pszEnd) {
					for ( ; (psz < pszEnd) and ('0' == *psz); psz++)
						;
					};
				Skip0(pszA, pszAend);
				Skip0(pszB, pszBend);

				// Count Digit Length
				auto CountDigitLength = [](auto const*& psz, auto const* const pszEnd) -> int {
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
				tcharA cA = *pszA;
				tcharB cB = *pszB;
				if constexpr (bIgnoreCase) { cA = ToLower(cA); cB = ToLower(cB); }
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

	BSC__NODISCARD constexpr inline int CompareNumericStrings(concepts::tstring_like auto const& svA, concepts::tstring_like auto const& svB, bool bIgnoreCase)
		requires (concepts::have_same_tchar<decltype(svA), decltype(svB)>)
	{
		return bIgnoreCase
			? CompareNumericStrings<true>(svA, svB)
			: CompareNumericStrings<false>(svA, svB);
	}

	BSC__NODISCARD constexpr inline int tszdcmp(concepts::tstring_like auto const& svA, concepts::tstring_like auto const& svB) 
		requires (concepts::have_same_tchar<decltype(svA), decltype(svB)>)
	{
		return CompareNumericStrings<false>(svA, svB);
	}
	BSC__NODISCARD constexpr inline int tszdicmp(concepts::tstring_like auto const& svA, concepts::tstring_like auto const& svB) 
		requires (concepts::have_same_tchar<decltype(svA), decltype(svB)>)
	{
		return CompareNumericStrings<true>(svA, svB);
	}

}	// namespace biscuit
