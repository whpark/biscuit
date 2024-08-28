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

export module biscuit.string.basic;
import std;
import biscuit.aliases;
import biscuit.concepts;

namespace conecpts = biscuit::concepts;

namespace biscuit {

}

export namespace biscuit {

	//=============================================================================================================================
	// tszlen String Length
	namespace detail {
		template < concepts::string_elem tchar >
		BSC__NODISCARD constexpr size_t tszlen(std::span<tchar const> sz) {
			if (!sz.data())
				return 0;
			auto const* end = sz.data() + sz.size();
			for (auto const* pos = sz.data(); pos < end; pos++) {
				if (!*pos)
					return std::distance(sz.data(), pos);
			}
			return sz.size();	// max size
		}
	}

	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr size_t tpszlen(tchar const* psz) {
		if (!psz) return 0;
		tchar const* pos = psz;
		while (*pos) { pos++; }
		return pos-psz;
	}

	template < concepts::tchar_string_like tstring >
	BSC__NODISCARD constexpr size_t tszlen(tstring const& v) {
		return detail::tszlen(std::span{std::data(v), std::size(v)});
	}


	//=============================================================================================================================
	// tszcpy
	namespace detail {
		// tszcpy
		template < concepts::string_elem tcharA, concepts::string_elem tcharB >
		constexpr auto tszcpy(std::span<tcharA> dst, std::span<tcharB const> src) -> std::expected<std::span<tcharA>, errno_t> {
			auto len = std::size(src);
			if (std::size(dst) == 0 or std::data(dst) == nullptr or std::data(src) == nullptr) {
				return std::unexpected(EINVAL);
			}
			errno_t err = {};
			//std::fill(std::begin(dst), std::end(dst), (tcharA)0);	// do I need this?
			if (len == 0) {
				std::data(dst)[0] = 0;
			}
			else {
				//static_assert(std::is_same_v<tcharA, tcharB>);
				if (len >= std::size(dst)) {
					len = std::size(dst)-1;
					err = ERANGE;
				}
				std::copy(std::data(src), std::data(src)+len, std::data(dst));
				std::data(dst)[len] = 0;
			}
			if (err)
				return std::unexpected(err);
			return std::span(dst.data()+len, dst.size()-len);
		}
	}

	/// @brief copy string
	/// @return 0 if success, EINVAL if pszDest is nullptr or sizeDest is 0, ERANGE if sizeDest is too small.
	template < concepts::string_elem tchar >
	BSC__DEPR_SEC constexpr auto tpszcpy(tchar* pszDest, size_t sizeDest, tchar const* pszSrc) -> std::expected<tchar*, errno_t> {
		if (!pszDest or !sizeDest or (sizeDest > RSIZE_MAX))
			return std::unexpected(EINVAL);
		if (!pszSrc) {
			*pszDest = 0;
			return pszDest;
		}
		tchar* const pszMax = pszDest+sizeDest-1;	// for null-terminator
		for (; pszDest < pszMax; pszDest++, pszSrc++) {
			*pszDest = *pszSrc;
			if (!*pszSrc)
				return pszDest;
		}
		*pszDest = 0;
		return std::unexpected(ERANGE);
	}


	template < concepts::tchar_string_like tstringA, concepts::tchar_string_like tstringB >
		requires (std::is_trivially_copyable_v<tstringA> && concepts::have_same_tchar<tstringA, tstringB>)
	constexpr auto tszcpy(tstringA& dst, tstringB const& src) {
		using tcharA = std::ranges::range_value_t<std::remove_cvref_t<tstringA>>;
		using tcharB = std::ranges::range_value_t<std::remove_cvref_t<tstringB>>;
		return detail::tszcpy<tcharA, tcharB>(std::span{std::data(dst), std::size(dst)}, std::span{std::data(src), std::size(src)});
	}

	//=============================================================================================================================
	// tpszcat
	template < concepts::string_elem tchar >
	BSC__DEPR_SEC constexpr auto tpszcat(tchar* pszDest, size_t sizeDest, tchar const* pszSrc) {
		if (!pszSrc || !*pszSrc)
			return 0;
		if (!pszDest || !sizeDest || (sizeDest > RSIZE_MAX))
			return EINVAL;
		// jump to end of pszDest
		while (*pszDest && sizeDest) {
			pszDest++; sizeDest--;
		}
	#pragma warning(suppress:4996)
		return tpszcpy<tchar>(pszDest, sizeDest, pszSrc);
	}

	template < concepts::tchar_string_like tstringA, concepts::tchar_string_like tstringB >
		requires (concepts::have_same_tchar<tstringA, tstringB>)
	constexpr auto tszcat(tstringA& dst, tstringB const& src) {
		using tcharA = std::ranges::range_value_t<std::remove_cvref_t<tstringA>>;
		using tcharB = std::ranges::range_value_t<std::remove_cvref_t<tstringB>>;

		auto len = tszlen(dst);
		std::span<tcharA> buf{std::data(dst)+len, std::size(dst)-len};

		return detail::tszcpy<tcharA, tcharB>(buf, std::span{std::data(src), std::size(src)});
	}

//	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
//	constexpr inline BSC__DEPR_SEC errno_t tszcat(tstring_buf &szDest, tchar const* pszSrc) {
//#pragma warning(suppress:4996)
//		return tszcat(std::data(szDest), std::size(szDest), pszSrc);
//	}
//	template < concepts::string_elem tchar >
//	constexpr errno_t tszcat(tchar* pszDest, size_t sizeDest, std::basic_string_view<tchar> svSrc) {
//		if (!pszDest or !sizeDest or (sizeDest > RSIZE_MAX) or (svSrc.size() > RSIZE_MAX))
//			return EINVAL;
//		auto const* const pszMax = pszDest + sizeDest;
//		auto nCount = tszlen(pszDest, pszMax);
//		if (nCount > RSIZE_MAX)
//			return ERANGE;
//		tchar* pos = pszDest + nCount;
//		if (sizeDest <= nCount + svSrc.size() + 1) {
//			*pos = 0;
//			return ERANGE;
//		}
//		tchar const* posSrc = svSrc.data();
//		tchar const* const pszSrcEnd = posSrc + svSrc.size();
//		for (; (pos < pszMax) and (posSrc < pszSrcEnd); pos++, posSrc++) {
//			*pos = *posSrc;
//			if (!*pos)
//				return 0;
//		}
//		*pos = 0;
//		return 0;
//	}
//	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
//	constexpr errno_t tszcat(tstring_buf &szDest, std::basic_string_view<tchar> svSrc) {
//		return tszcat(std::data(szDest), std::size(szDest), svSrc);
//	}
//	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
//	constexpr errno_t tszcat(tstring_buf &szDest, std::basic_string<tchar> const& strSrc) {
//		return tszcat(std::data(szDest), std::size(szDest), std::basic_string_view{strSrc.data(), strSrc.size()});
//	}
//
//	template < concepts::string_elem tchar >
//	constexpr BSC__DEPR_SEC size_t tszrmchar(tchar* const& psz, int chRemove) {
//		if (!psz || !chRemove)
//			return 0;
//		tchar* pos = psz;
//		tchar* p2 {};
//		for (; *pos; pos++) {
//			if (*pos == chRemove) {
//				p2 = pos + 1;
//				break;
//			}
//		}
//		if (p2) {
//			for (; ; p2++) {
//				if (*p2 == chRemove)
//					continue;
//				if (!(*pos = *p2))
//					break;
//				pos++;
//			}
//		}
//		return pos - psz;
//	}
//	template < concepts::string_elem tchar >
//	constexpr size_t tszrmchar(tchar* const psz, tchar const* const pszMax, tchar chRemove) {
//		if (!psz || !chRemove)
//			return 0;
//		tchar* pos = psz;
//		tchar* p2 {};
//		for (; (pos < pszMax) and *pos; pos++) {
//			if (*pos == chRemove) {
//				p2 = pos + 1;
//				break;
//			}
//		}
//		if (p2) {
//			for (; (pos < pszMax) and (p2 < pszMax); p2++) {
//				if (*p2 == chRemove)
//					continue;
//				if (!(*pos = *p2))
//					break;
//				pos++;
//			}
//		}
//		return pos - psz;
//	}
//	template < concepts::string_elem tchar, concepts::contiguous_type_string_container<tchar> tstring_buf >
//	constexpr size_t tszrmchar(tstring_buf &sz, tchar chRemove) {
//		return tszrmchar(std::data(sz), std::data(sz) + std::size(sz), chRemove);
//	}
//
//

//	template < concepts::contiguous_string_container tstring_buf >
//	errno_t tszupr(tstring_buf& buf) {
//		if (!std::data(buf))
//			return EINVAL;
//		if (std::size(buf) > RSIZE_MAX)
//			return ERANGE;
//		for (auto& c : buf) {
//			if (!c)
//				break;
//			MakeUpper(c);
//		}
//		return 0;
//	}
//	template < concepts::string_elem tchar >
//	errno_t tszupr(tchar* const psz, size_t size) {
//		if (!psz)
//			return EINVAL;
//		if (size > RSIZE_MAX)
//			return ERANGE;
//		tchar* pos = psz;
//		for (size_t i = 0; i < size; i++) {
//			if (!*pos)
//				break;
//			MakeUpper(*pos);
//		}
//		return 0;
//	}
//
//	template < concepts::contiguous_string_container tstring_buf >
//	errno_t tszlwr(tstring_buf& buf) {
//		if (!std::data(buf))
//			return EINVAL;
//		if (std::size(buf) > RSIZE_MAX)
//			return ERANGE;
//		for (auto& c : buf) {
//			if (!c)
//				break;
//			MakeLower(c);
//		}
//		return 0;
//	}
//	template < concepts::string_elem tchar >
//	errno_t tszlwr(tchar* const& psz, size_t size) {
//		if (!psz)
//			return EINVAL;
//		if (size > RSIZE_MAX)
//			return ERANGE;
//		tchar* pos = psz;
//		for (size_t i = 0; i < size; i++) {
//			if (!*pos)
//				break;
//			MakeLower(*pos);
//		}
//		return 0;
//	}
//
//	//template < concepts::string_elem tchar >
//	//BSC__DEPR_SEC tchar* tszrev(tchar* psz) {
//	//	if (!psz)
//	//		return nullptr;
//	//	std::reverse(psz, psz + tszlen(psz));
//	//	return psz;
//	//}
//	template < concepts::string_elem tchar >
//	tchar* tszrev(tchar* psz, tchar const* const pszEnd) {
//		if (!psz)
//			return nullptr;
//		std::reverse(psz, pszEnd);
//		return psz;
//	}
//	template < concepts::contiguous_string_container tstring_buf >
//	std::remove_cvref_t<decltype(tstring_buf{}[0])>* tszrev(tstring_buf& buf) {
//		std::reverse(std::data(buf), std::data(buf)+tszlen(buf));
//		return std::data(buf);
//	}



}	// namespace biscuit
