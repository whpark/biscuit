module;

//////////////////////////////////////////////////////////////////////
//
// string_basic.ixx:
//
// PWH
// 2020.12.16.
// 2024-08-24 biscuit
//
//////////////////////////////////////////////////////////////////////

#include <cerrno>
#include "biscuit/macro.h"

#define BSC__DEPR_SEC [[deprecated("NOT Secure. use 'sv' instead.")]]

export module biscuit.string.basic;
import std;
import biscuit.concepts;

namespace conecpts = biscuit::concepts;

export namespace biscuit {

	//=============================================================================================================================
	// tszlen String Length

	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr BSC__DEPR_SEC size_t tszlen(tchar const*const& psz) {
		if (!psz) return 0;
		tchar const* pos = psz;
		while (*pos) { pos++; }
		return pos-psz;
	}

	namespace detail {
		template < concepts::string_elem tchar >
		BSC__NODISCARD constexpr size_t tszlen(tchar const* psz, tchar const* const pszMax) {
			for (tchar const* pos = psz; pos < pszMax; pos++) {
				if (!*pos)
					return pos-psz;
			}
			return pszMax-psz;	// max size
		}
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr size_t tszlen(tchar const* psz, tchar const* const pszMax) {
		if (!psz)
			return 0;
		return detail::tszlen(psz, pszMax);
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr size_t tszlen(tchar const* psz, size_t sizeOfBuf) {
		return tszlen(psz, psz + sizeOfBuf);
	}
	template < concepts::contiguous_string_container tstring_buf >
	BSC__NODISCARD constexpr size_t tszlen(tstring_buf const& v) {
		return tszlen(std::data(v), std::size(v));
	}


	/// <summary>
	/// tszcpy : utf8/16/32/wchar_t ver. of strcpy_s
	/// </summary>
	/// <param name="pszDest"></param>
	/// <param name="sizeDest"></param>
	/// <param name="pszSrc"></param>
	/// <returns>
	/// 0 : ok.
	/// EINVAL : if !pszDest || !pszSrc
	/// ERANGE : if sizeDest is smaller
	/// </returns>
	template < concepts::string_elem tchar >
	constexpr BSC__DEPR_SEC errno_t tszcpy(tchar* const& pszDest, size_t sizeDest, tchar const* const& pszSrc) {
		if (!pszDest or !sizeDest or (sizeDest > RSIZE_MAX))
			return EINVAL;
		tchar* pos = pszDest;
		if (!pszSrc) {
			*pos = 0;
			return EINVAL;
		}
		tchar const* posSrc = pszSrc;
		tchar* const pszMax = pszDest+sizeDest;
		for (; pos < pszMax; pos++, posSrc++) {
			*pos = *posSrc;
			if (!*posSrc)
				return 0;
		}
		pszDest[0] = 0;
		return ERANGE;
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr BSC__DEPR_SEC errno_t tszcpy(tstring_buf& szDest, tchar const* const& pszSrc) {
#pragma warning(suppress:4996)
		return tszcpy(std::data(szDest), std::size(szDest), pszSrc);
	}

	/// <summary>
	/// tszcpy : utf8/16/32/wchar_t ver. of strcpy_s
	/// </summary>
	/// <param name="pszDest">dest. buffer</param>
	/// <param name="sizeDest">size of dest. including null terminating char.</param>
	/// <param name="svSrc">source string. do not need to be NULL terminated string</param>
	/// <returns>
	/// 0 : ok.
	/// EINVAL : if !pszDest
	/// ERANGE : if sizeDest is not enough.
	/// </returns>
	template < concepts::string_elem tchar >
	constexpr errno_t tszcpy(tchar* pszDest, size_t sizeDest, std::basic_string_view<tchar> svSrc) {
		if (!pszDest or !sizeDest)
			return EINVAL;
		tchar* pos = pszDest;
		auto sizeSrc = size(svSrc);
		if (sizeDest <= sizeSrc) {
			*pos = 0;
			return ERANGE;
		}
		tchar const* s = data(svSrc);
		tchar const* const e = s + sizeSrc;
		while  (s < e)
			*pos++ = *s++;
		*pos = 0;
		return 0;
	}

	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr errno_t tszcpy(tstring_buf& szDest, std::basic_string_view<tchar> svSrc) {
		tchar* const pszDest = std::data(szDest);
		size_t sizeDest = std::size(szDest);
		return tszcpy(pszDest, sizeDest, svSrc);
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr errno_t tszcpy(tstring_buf& szDest, std::basic_string<tchar> const& strSrc) {
		tchar* const pszDest = std::data(szDest);
		size_t sizeDest = std::size(szDest);
		return tszcpy(pszDest, sizeDest, std::basic_string_view{strSrc.data(), strSrc.size()});
	}

#if 0	// any type <- any type.... 너무 많은 옵션.
	template < concepts::string_buffer_fixed tcontainer1, concepts::string_buffer_fixed tcontainer2 >
	requires requires (tcontainer1 t1, tcontainer2 t2) {
		requires std::is_same_v<std::remove_cvref_t<decltype(t1[0])>, std::remove_cvref_t<decltype(t2[0])> >
		&& concepts::string_elem<std::remove_cvref_t<decltype(t1[0])>>;
	}
	constexpr errno_t tszcpy(tcontainer1& szDest, tcontainer2 const& svSrc) {
		using tchar = std::remove_cvref_t<decltype(szDest[0])>;
		tchar* const pszDest = std::data(szDest);
		size_t sizeDest = std::size(szDest);
		if (!pszDest or !sizeDest)
			return EINVAL;
		tchar* pos = pszDest;
		auto sizeSrc = tszlen(svSrc);
		if (sizeDest <= sizeSrc) {
			*pos = 0;
			return ERANGE;
		}
		tchar const* s = std::data(svSrc);
		tchar const* const e = s + sizeSrc;
		while  (s < e) {
			*pos++ = *s++;
		}
		*pos = 0;
		return 0;
	}
#endif

	// tszncpy
	template < concepts::string_elem tchar >
	constexpr std::errno_t tszncpy(tchar*const& pszDest, size_t sizeDest, tchar const* pszSrc, size_t nCount) {
		if (!pszDest or !sizeDest or (sizeDest > RSIZE_MAX) )
			return EINVAL;
		tchar* pos = pszDest;
		if (!pszSrc) {
			*pos = 0;
			return EINVAL;
		}
		if (nCount == _TRUNCATE) {
			nCount = sizeDest-1;
		} else if (sizeDest <= nCount) {
			*pos = 0;
			return ERANGE;
		}

		tchar const* posSrc = pszSrc;
		tchar* const pszMax = pszDest + nCount;	// not sizeDest
		for (; pos < pszMax; pos++, posSrc++) {
			*pos = *posSrc;
			if (!*posSrc)
				return 0;
		}
		*pos = 0;	// null terminator
		return 0;
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr inline std::errno_t tszncpy(tstring_buf &szDest, tchar const* pszSrc, size_t nCount) {
		return tszncpy(std::data(szDest), std::size(szDest), pszSrc, nCount);
	}

	// tszncpy (sv)
	template < concepts::string_elem tchar >
	constexpr errno_t tszncpy(tchar* pszDest, size_t sizeDest, std::basic_string_view<tchar> svSrc, size_t nCount) {
		if (!pszDest or !sizeDest or (sizeDest > RSIZE_MAX) or (svSrc.size() > RSIZE_MAX))
			return EINVAL;
		tchar* pos = pszDest;
		if (nCount == _TRUNCATE) {
			nCount = std::min(sizeDest-1, svSrc.size());
		} else if (sizeDest <= nCount) {	// svSrc might be smaller and it could be ok sometimes. but it's safer to make some noise, here.
			*pos = 0;
			return ERANGE;
		} else {
			nCount = std::min(nCount, svSrc.size());
		}

		tchar const* posSrc = svSrc.data();
		tchar* const MaxEnd = pszDest + nCount;	// not sizeDest
		for (; pos < MaxEnd; pos++, posSrc++) {
			*pos = *posSrc;
			if (!*pos)
				return 0;
		}
		*pos = 0;	// null terminator
		return 0;
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr inline errno_t tszncpy(tstring_buf &szDest, std::basic_string_view<tchar> svSrc, size_t nCount) {
		return tszncpy(std::data(szDest), std::size(szDest), svSrc, nCount);
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr inline errno_t tszncpy(tstring_buf &szDest, std::basic_string<tchar> const& strSrc, size_t nCount) {
		return tszncpy(std::data(szDest), std::size(szDest), std::basic_string_view<tchar>{strSrc}, nCount);
	}

	// tszcat
	template < concepts::string_elem tchar >
	constexpr BSC__DEPR_SEC errno_t tszcat(tchar* pszDest, size_t sizeDest, tchar const* pszSrc) {
		if (!pszDest or !sizeDest or (sizeDest > RSIZE_MAX) )
			return EINVAL;
		if (!pszSrc) {
			pszDest[0] = 0;
			return EINVAL;
		}
		auto const* const pszMax = pszDest + sizeDest;
		auto nCount = tszlen(pszDest, pszMax);
		if (nCount > RSIZE_MAX)
			return ERANGE;
		tchar* pos = pszDest + nCount;
		tchar const* posSrc = pszSrc;
		for (; pos < pszMax; pos++, posSrc++) {
			*pos = *posSrc;
			if (!*pos)
				return 0;
		}
		// pszSrc is longer
		pszDest[0] = 0;
		return ERANGE;
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr inline BSC__DEPR_SEC errno_t tszcat(tstring_buf &szDest, tchar const* pszSrc) {
#pragma warning(suppress:4996)
		return tszcat(std::data(szDest), std::size(szDest), pszSrc);
	}
	template < concepts::string_elem tchar >
	constexpr errno_t tszcat(tchar* pszDest, size_t sizeDest, std::basic_string_view<tchar> svSrc) {
		if (!pszDest or !sizeDest or (sizeDest > RSIZE_MAX) or (svSrc.size() > RSIZE_MAX))
			return EINVAL;
		auto const* const pszMax = pszDest + sizeDest;
		auto nCount = tszlen(pszDest, pszMax);
		if (nCount > RSIZE_MAX)
			return ERANGE;
		tchar* pos = pszDest + nCount;
		if (sizeDest <= nCount + svSrc.size() + 1) {
			*pos = 0;
			return ERANGE;
		}
		tchar const* posSrc = svSrc.data();
		tchar const* const pszSrcEnd = posSrc + svSrc.size();
		for (; (pos < pszMax) and (posSrc < pszSrcEnd); pos++, posSrc++) {
			*pos = *posSrc;
			if (!*pos)
				return 0;
		}
		*pos = 0;
		return 0;
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr errno_t tszcat(tstring_buf &szDest, std::basic_string_view<tchar> svSrc) {
		return tszcat(std::data(szDest), std::size(szDest), svSrc);
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr errno_t tszcat(tstring_buf &szDest, std::basic_string<tchar> const& strSrc) {
		return tszcat(std::data(szDest), std::size(szDest), std::basic_string_view{strSrc.data(), strSrc.size()});
	}

	template < concepts::string_elem tchar >
	constexpr BSC__DEPR_SEC size_t tszrmchar(tchar* const& psz, int chRemove) {
		if (!psz || !chRemove)
			return 0;
		tchar* pos = psz;
		tchar* p2 {};
		for (; *pos; pos++) {
			if (*pos == chRemove) {
				p2 = pos + 1;
				break;
			}
		}
		if (p2) {
			for (; ; p2++) {
				if (*p2 == chRemove)
					continue;
				if (!(*pos = *p2))
					break;
				pos++;
			}
		}
		return pos - psz;
	}
	template < concepts::string_elem tchar >
	constexpr size_t tszrmchar(tchar* const psz, tchar const* const pszMax, tchar chRemove) {
		if (!psz || !chRemove)
			return 0;
		tchar* pos = psz;
		tchar* p2 {};
		for (; (pos < pszMax) and *pos; pos++) {
			if (*pos == chRemove) {
				p2 = pos + 1;
				break;
			}
		}
		if (p2) {
			for (; (pos < pszMax) and (p2 < pszMax); p2++) {
				if (*p2 == chRemove)
					continue;
				if (!(*pos = *p2))
					break;
				pos++;
			}
		}
		return pos - psz;
	}
	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
	constexpr size_t tszrmchar(tstring_buf &sz, tchar chRemove) {
		return tszrmchar(std::data(sz), std::data(sz) + std::size(sz), chRemove);
	}


	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr BSC__DEPR_SEC int tszcmp(tchar const* pszA, tchar const* pszB) {
		return tszcmp<tchar, false>(pszA, pszB, {}, [](auto v) -> int { return (int)v; });
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr BSC__DEPR_SEC int tszncmp(tchar const* pszA, tchar const* pszB, size_t nCount) {
		return tszcmp<tchar, true>(pszA, pszB, nCount, [](auto v) -> int { return (int)v; });
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr BSC__DEPR_SEC int tszicmp(tchar const* pszA, tchar const* pszB) {
		return tszcmp<tchar, false>(pszA, pszB, {}, [](auto v) -> int { return std::tolower(v); });
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr BSC__DEPR_SEC int tsznicmp(tchar const* pszA, tchar const* pszB, size_t nCount) {
		return tszcmp<tchar, true>(pszA, pszB, nCount, [](auto v) -> int { return std::tolower(v); });
	}

	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr int tszcmp(std::basic_string_view<tchar> svA, std::basic_string_view<tchar> svB) {
		return tszcmp<tchar, false>(svA, svB, {}, [](auto v) -> int { return (int)v; });
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr int tszncmp(std::basic_string_view<tchar> svA, std::basic_string_view<tchar> svB, size_t nCount) {
		return tszcmp<tchar, true>(svA, svB, nCount, [](auto v) -> int { return (int)v; });
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr int tszicmp(std::basic_string_view<tchar> svA, std::basic_string_view<tchar> svB) {
		return tszcmp<tchar, false>(svA, svB, {}, [](auto v) -> int { return std::tolower(v); });
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr int tsznicmp(std::basic_string_view<tchar> svA, std::basic_string_view<tchar> svB, size_t nCount) {
		return tszcmp<tchar, true>(svA, svB, nCount, [](auto v) -> int { return std::tolower(v); });
	}


	template < concepts::contiguous_string_container tstring_buf >
	errno_t tszupr(tstring_buf& buf) {
		if (!std::data(buf))
			return EINVAL;
		if (std::size(buf) > RSIZE_MAX)
			return ERANGE;
		for (auto& c : buf) {
			if (!c)
				break;
			MakeUpper(c);
		}
		return 0;
	}
	template < concepts::string_elem tchar >
	errno_t tszupr(tchar* const psz, size_t size) {
		if (!psz)
			return EINVAL;
		if (size > RSIZE_MAX)
			return ERANGE;
		tchar* pos = psz;
		for (size_t i = 0; i < size; i++) {
			if (!*pos)
				break;
			MakeUpper(*pos);
		}
		return 0;
	}

	template < concepts::contiguous_string_container tstring_buf >
	errno_t tszlwr(tstring_buf& buf) {
		if (!std::data(buf))
			return EINVAL;
		if (std::size(buf) > RSIZE_MAX)
			return ERANGE;
		for (auto& c : buf) {
			if (!c)
				break;
			MakeLower(c);
		}
		return 0;
	}
	template < concepts::string_elem tchar >
	errno_t tszlwr(tchar* const& psz, size_t size) {
		if (!psz)
			return EINVAL;
		if (size > RSIZE_MAX)
			return ERANGE;
		tchar* pos = psz;
		for (size_t i = 0; i < size; i++) {
			if (!*pos)
				break;
			MakeLower(*pos);
		}
		return 0;
	}

	//template < concepts::string_elem tchar >
	//BSC__DEPR_SEC tchar* tszrev(tchar* psz) {
	//	if (!psz)
	//		return nullptr;
	//	std::reverse(psz, psz + tszlen(psz));
	//	return psz;
	//}
	template < concepts::string_elem tchar >
	tchar* tszrev(tchar* psz, tchar const* const pszEnd) {
		if (!psz)
			return nullptr;
		std::reverse(psz, pszEnd);
		return psz;
	}
	template < concepts::contiguous_string_container tstring_buf >
	std::remove_cvref_t<decltype(tstring_buf{}[0])>* tszrev(tstring_buf& buf) {
		std::reverse(std::data(buf), std::data(buf)+tszlen(buf));
		return std::data(buf);
	}



}	// namespace biscuit
