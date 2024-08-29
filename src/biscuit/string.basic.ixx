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

export namespace biscuit {

	//=============================================================================================================================
	// tpsz... : for pointer string
	// tsz...  : for array string

	//-----------------------------------------------------------------------------------------------------------------------------
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


	//-----------------------------------------------------------------------------------------------------------------------------
	// tszcpy
	namespace detail {
		// tszcpy
		template < concepts::string_elem tcharA, concepts::string_elem tcharB >
		constexpr auto tszcpy(std::span<tcharA> dst, std::span<tcharB const> src) -> std::expected<std::span<tcharA>, std::error_code> {
			auto len = std::size(src);
			if (std::size(dst) == 0 or std::data(dst) == nullptr or std::data(src) == nullptr) {
				return std::unexpected(std::make_error_code(std::errc::invalid_argument));
			}
			std::error_code err = {};
			//std::fill(std::begin(dst), std::end(dst), (tcharA)0);	// do I need this?
			if (len == 0) {
				std::data(dst)[0] = 0;
			}
			else {
				//static_assert(std::is_same_v<tcharA, tcharB>);
				if (len >= std::size(dst)) {
					len = std::size(dst)-1;
					err = std::make_error_code(std::errc::result_out_of_range);
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
	BSC__DEPR_SEC constexpr auto tpszcpy(tchar* pszDest, size_t sizeDest, tchar const* pszSrc) -> std::expected<tchar*, std::error_code> {
		if (!pszDest or !sizeDest or (sizeDest > RSIZE_MAX))
			return std::unexpected(std::make_error_code(std::errc::invalid_argument));
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
		return std::unexpected(std::make_error_code(std::errc::result_out_of_range));
	}


	template < concepts::tchar_string_like tstringA, concepts::tchar_string_like tstringB >
		requires (std::is_trivially_copyable_v<tstringA> && concepts::have_same_tchar<tstringA, tstringB>)
	constexpr auto tszcpy(tstringA& dst, tstringB const& src) {
		using tcharA = std::ranges::range_value_t<std::remove_cvref_t<tstringA>>;
		using tcharB = std::ranges::range_value_t<std::remove_cvref_t<tstringB>>;
		return detail::tszcpy<tcharA, tcharB>(std::span{std::data(dst), std::size(dst)}, std::span{std::data(src), std::size(src)});
	}

	//-----------------------------------------------------------------------------------------------------------------------------
	// tpszcat
	template < concepts::string_elem tchar >
	BSC__DEPR_SEC constexpr auto tpszcat(tchar* pszDest, size_t sizeDest, tchar const* pszSrc) {
		if (!pszSrc || !*pszSrc)
			return 0;
		if (!pszDest || !sizeDest || (sizeDest > RSIZE_MAX))
			return std::unexpected(std::make_error_code(std::errc::invalid_argument));
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

	//-----------------------------------------------------------------------------------------------------------------------------
	// tszsearch - Search Char. for advanced search, use string or string_view

	/// @brief searches character.
	/// @return !!! CAUTION !!! nullptr if not found.
	template < concepts::string_elem tchar >
	BSC__NODISCARD BSC__DEPR_SEC constexpr tchar* tpszsearch(tchar* beg, tchar ch) {
		if (!beg)
			return nullptr;
		for (; *beg; beg++) {
			if (*beg == ch)
				return beg;
		}
		return nullptr;
	}

	/// @brief searches character.
	/// @return !!! CAUTION !!! nullptr if not found.
	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr tchar* tszsearch(tchar* beg, tchar const* end, tchar ch) {
		if (!beg)
			return nullptr;
		if (end - beg > RSIZE_MAX)
			return nullptr;
		for (; beg < end and *beg; beg++) {
			if (*beg == ch)
				return beg;
		}
		return nullptr;
	}

	/// @brief searches character.
	/// @return !!! CAUTION !!! 'end' value if not found.
	template < concepts::tchar_string_like tstring >
	BSC__NODISCARD constexpr auto* tszsearch(tstring const& str, concepts::value_t<tstring> ch) {
		return tszsearch(std::data(str), std::data(str)+std::size(str), ch);
	}
	template < concepts::tchar_string_like tstring >
	BSC__NODISCARD constexpr auto* tszsearch(tstring&& str, concepts::value_t<tstring> ch) = delete;

	//-----------------------------------------------------------------------------------------------------------------------------
	// tszrmchar - Remove Char

	/// @brief remove character from string.
	/// @return length of string after removing character.
	template < concepts::string_elem tchar >
	BSC__DEPR_SEC constexpr size_t tpszrmchar(tchar* psz, int chRemove) {
		if (!psz || !chRemove)
			return 0;
		tchar* dst = psz;
		tchar const* src {};

		// find first chRemove
		for (; *dst; dst++) {
			if (*dst == chRemove) {
				src = dst + 1;
				break;
			}
		}
		if (!src)
			return dst - psz;

		// remove chRemove
		for (src++; ; src++) {
			if (*src == chRemove)
				continue;
			if (!(*dst = *src))
				break;
			dst++;
		}
		return dst - psz;
	}

	/// @brief remove character from string.
	/// @return length of string after removing character.
	template < concepts::tchar_string_like tstring >
		requires (std::is_trivially_copyable_v<tstring>)
	constexpr size_t tszrmchar(tstring& sz, concepts::value_t<tstring> chRemove) {
		//auto iter = std::remove(std::begin(sz), std::end(sz), chRemove);
		//return std::distance(std::begin(sz), iter-1);

		using tchar = concepts::value_t<tstring>;

		auto* const begin = std::data(sz);
		if (!begin || !chRemove)
			return 0;

		auto const* const end = begin + std::size(sz);
		auto* dst = begin;
		tchar const* src = {};

		// find first chRemove
		for (; dst < end and *dst; dst++) {
			if (*dst == chRemove) {
				src = dst;
				break;
			}
		}

		// remove chRemove
		if (!src)
			return dst - begin;
		for (src++; src < end; src++) {
			if (*src == chRemove)
				continue;
			if (!(*dst = *src))
				break;
			dst++;
		}
		if (src == end and dst < end)
			*dst = 0;	// !!! remove it anyway, here. if it's not removed, no null-terminator could be appended.

		return dst - begin;
	}

	//-----------------------------------------------------------------------------------------------------------------------------
	// ToLower, ToUpper

	template < concepts::string_elem tchar >
	BSC__NODISCARD inline tchar ToLower(tchar c) {
		if constexpr (std::is_same_v<tchar, char>) { return (tchar)std::tolower(c); } else { return (tchar)std::towlower(c); }
	}
	template < concepts::string_elem tchar >
	BSC__NODISCARD inline tchar ToUpper(tchar c) {
		if constexpr (std::is_same_v<tchar, char>) { return (tchar)std::toupper(c); } else { return (tchar)std::towupper(c); }
	}

	template < concepts::tchar_string_like tstring, typename tchar = concepts::value_t<tstring> >
	BSC__NODISCARD auto ToLower(tstring const& sv) -> std::basic_string<tchar> {
		std::basic_string<tchar> str;
		str.reserve(sv.size());
		for (auto c : sv)
			str += ToLower(c);
		return str;
	}
	template < concepts::tchar_string_like tstring, typename tchar = concepts::value_t<tstring> >
	BSC__NODISCARD auto ToUpper(tstring const& sv) -> std::basic_string<tchar> {
		std::basic_string<tchar> str;
		str.reserve(sv.size());
		for (auto c : sv)
			str += ToUpper(c);
		return str;
	}

	template < concepts::string_elem tchar > inline void MakeLower(std::basic_string<tchar>& str) { for (auto& c : str) MakeLower(c); }
	template < concepts::string_elem tchar > inline void MakeUpper(std::basic_string<tchar>& str) { for (auto& c : str) MakeUpper(c); }

	template < concepts::string_elem tchar > inline void MakeLower(tchar& c) { c = ToLower(c); }
	template < concepts::string_elem tchar > inline void MakeUpper(tchar& c) { c = ToUpper(c); }

	//-----------------------------------------------------------------------------------------------------------------------------
	// Upper / Lower helper class
	namespace detail {

		template < typename TReturn >
		struct TToTransparent {
			inline TReturn operator () (auto&& v) const { return v; }
		};

		template < typename TReturn >
		struct TToUpper {
			inline TReturn operator () (auto&& v) const { return ToUpper(v); }
		};

		template < typename TReturn >
		struct TToLower {
			inline TReturn operator () (auto&& v) const { return ToLower(v); }
		};

	}

	//-----------------------------------------------------------------------------------------------------------------------------
	// tszupr, tszlwr - Make Upper/Lower

	namespace detail {
		template < concepts::string_elem tchar, typename TEvaluator >
		constexpr std::error_code tpsz_transform(tchar* psz, size_t size) {
			constexpr static TEvaluator const func{};
			if (!psz)
				return EINVAL;
			if (size > RSIZE_MAX)
				return ERANGE;
			for (; *psz and size; psz++, size--) {
				*psz = func(*psz);
			}
			return 0;
		}

		template < concepts::tchar_string_like tstring, typename TEvaluator >
			requires (std::is_trivially_copyable_v<tstring>)
		constexpr std::error_code tsz_transform(tstring& buf) {
			constexpr static TEvaluator const func{};
			if (!std::data(buf))
				return EINVAL;
			if (std::size(buf) > RSIZE_MAX)
				return ERANGE;
			for (auto& c : buf) {
				if (!c)
					break;
				c = func(c);
			}
			return 0;
		}
	}

	template < concepts::string_elem tchar >
	BSC__DEPR_SEC constexpr std::error_code tpszupr(tchar* psz, size_t size) {
		return detail::tpsz_transform<tchar, detail::TToUpper<tchar>>(psz, size);
	}
	template < concepts::tchar_string_like tstring >
		requires (std::is_trivially_copyable_v<tstring>)
	std::error_code tszupr(tstring& buf) {
		using tchar = concepts::value_t<tstring>;
		return detail::tsz_transform<tchar, detail::TToUpper<tchar>>(buf);
	}

	//-----------------------------------------------------------------------------------------------------------------------------
	// tszupr, tszlwr - Make Upper/Lower
	template < concepts::string_elem tchar >
	BSC__DEPR_SEC std::error_code tpszlwr(tchar* psz, size_t size) {
		return detail::tpsz_transform<tchar, detail::TToLower<tchar>>(psz, size);
	}
	template < concepts::tchar_string_like tstring >
		requires (std::is_trivially_copyable_v<tstring>)
	std::error_code tszlwr(tstring& buf) {
		using tchar = concepts::value_t<tstring>;
		return detail::tsz_transform<tchar, detail::TToLower<tchar>>(buf);
	}

	template < concepts::string_elem tchar >
	BSC__DEPR_SEC size_t tpszrev(tchar* psz) {
		if (!psz)
			return nullptr;
	#pragma warning(suppress:4996)
		auto len = tpszlen(psz);
		std::reverse(psz, psz+len);
		return len;
	}
	template < concepts::string_elem tchar >
	size_t tszrev(tchar* psz, tchar const* pszMax) {
		if (!psz)
			return 0;
		if (psz >= pszMax)
			return 0;
		auto len = detail::tszlen({psz, pszMax});
		std::reverse(psz, psz+len);
		return len;
	}
	template < concepts::tchar_string_like tstring >
		requires (std::is_trivially_copyable_v<tstring>)
	size_t tszrev(tstring& buf) {
		auto len = tszlen(buf);
		std::reverse(std::data(buf), std::data(buf)+len);
		return len;
	}


}	// namespace biscuit

