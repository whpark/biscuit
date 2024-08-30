module;

//////////////////////////////////////////////////////////////////////
//
// iconv_wrapper.ixx
//
// PWH
//
// 2021.05.07. libiconv wrapper
// 
//   https://www.gnu.org/software/libiconv/
//
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/biscuit.h"
#include "biscuit/macro.h"

#pragma warning(push)
#pragma warning(disable: 4819)	// codepage
#include "iconv.h"
#pragma warning(pop)

export module biscuit.iconv_wrapper;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.misc;
import std;

using namespace std::literals;

/*******************************************


		European languages
			ASCII, ISO-8859-{1,2,3,4,5,7,9,10,13,14,15,16}, KOI8-R, KOI8-U, KOI8-RU, CP{1250,1251,1252,1253,1254,1257}, CP{850,866,1131}, Mac{Roman,CentralEurope,Iceland,Croatian,Romania}, Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh
		Semitic languages
			ISO-8859-{6,8}, CP{1255,1256}, CP862, Mac{Hebrew,Arabic}
		Japanese
			EUC-JP, SHIFT_JIS, CP932, ISO-2022-JP, ISO-2022-JP-2, ISO-2022-JP-1, ISO-2022-JP-MS
		Chinese
			EUC-CN, HZ, GBK, CP936, GB18030, EUC-TW, BIG5, CP950, BIG5-HKSCS, BIG5-HKSCS:2004, BIG5-HKSCS:2001, BIG5-HKSCS:1999, ISO-2022-CN, ISO-2022-CN-EXT
		Korean
			EUC-KR, CP949, ISO-2022-KR, JOHAB
		Armenian
			ARMSCII-8
		Georgian
			Georgian-Academy, Georgian-PS
		Tajik
			KOI8-T
		Kazakh
			PT154, RK1048
		Thai
			ISO-8859-11, TIS-620, CP874, MacThai
		Laotian
			MuleLao-1, CP1133
		Vietnamese
			VISCII, TCVN, CP1258
		Platform specifics
			HP-ROMAN8, NEXTSTEP
		Full Unicode
			UTF-8
			UCS-2, UCS-2BE, UCS-2LE
			UCS-4, UCS-4BE, UCS-4LE
			UTF-16, UTF-16BE, UTF-16LE
			UTF-32, UTF-32BE, UTF-32LE
			UTF-7
			C99, JAVA
		Full Unicode, in terms of uint16_t or uint32_t (with machine dependent endianness and alignment)
			UCS-2-INTERNAL, UCS-4-INTERNAL
		Locale dependent, in terms of `char' or `wchar_t' (with machine dependent endianness and alignment, and with OS and locale dependent semantics)
			char, wchar_t
			The empty encoding name "" is equivalent to "char": it denotes the locale dependent character encoding.

		When configured with the option --enable-extra-encodings, it also provides support for a few extra encodings:

		European languages
			CP{437,737,775,852,853,855,857,858,860,861,863,865,869,1125}
		Semitic languages
			CP864
		Japanese
			EUC-JISX0213, Shift_JISX0213, ISO-2022-JP-3
		Chinese
			BIG5-2003 (experimental)
		Turkmen
			TDS565
		Platform specifics
			ATARIST, RISCOS-LATIN1


*******************************************/

namespace concepts = biscuit::concepts;

export namespace biscuit {
#pragma pack(push, 8)

	template < concepts::string_elem tchar_to, concepts::string_elem tchar_from, size_t initial_dst_buf_size = 1024 >
	class Ticonv {
		libiconv_t m_cd{ (iconv_t)-1 };

	public:
		Ticonv(char const* to = nullptr, char const* from = nullptr) {
			if (!to or !*to)
				to = GuessCodeFromType<tchar_to>();
			if (!from or !*from)
				from = GuessCodeFromType<tchar_from>();
			m_cd = iconv_open(to, from);
		}
		Ticonv(char const* to, char const* from, bool bTransliterate, bool bDiscardIlseq) : Ticonv(to, from) {
			SetTransliterate(bTransliterate);
			SetDiscardIsseq(bDiscardIlseq);
		}
		~Ticonv() {
			if (IsOpen())
				iconv_close(m_cd);
		}

		BSC__NODISCARD bool IsOpen() const {
			return m_cd != (iconv_t)-1;
		}

		BSC__NODISCARD bool IsTrivial() const {
			return IsFlag(ICONV_TRIVIALP);
		}
		BSC__NODISCARD bool IsTransliterate() const {
			return IsFlag(ICONV_GET_TRANSLITERATE);
		}
		BSC__NODISCARD bool IsDiscardIlseq() const {
			return IsFlag(ICONV_SET_DISCARD_ILSEQ);
		}
		void SetTransliterate(bool bOn = true) {
			SetFlag(ICONV_SET_TRANSLITERATE, bOn);
		}
		void SetDiscardIsseq(bool bOn = true) {
			SetFlag(ICONV_SET_DISCARD_ILSEQ, bOn);
		}

	protected:
		template < typename tchar >
		BSC__NODISCARD bool ContinueConvert(std::basic_string<tchar>& strTo, char** psrc, size_t& remnant) const {
			while (remnant) {
				auto size_out = strTo.size();
				strTo.resize(strTo.size() + std::max(4uz, remnant)*sizeof(tchar), 0);
				size_t outbytesleft = (strTo.size() - size_out)*sizeof(tchar);
				char* out = reinterpret_cast<char*>(strTo.data() + size_out);
				size_t converted = iconv(m_cd, psrc, &remnant, &out, &outbytesleft);
				if constexpr (sizeof(tchar) >= 2) {
					if (outbytesleft % sizeof(tchar) != 0) {
						throw std::runtime_error("invalid multibyte sequence");
					}
				}
				if (converted == (size_t)-1) {
					if (auto e = errno; e == E2BIG) {
						strTo.resize(strTo.size() - outbytesleft/sizeof(tchar));
						continue;
					}
					return false;
				}
				else {
					strTo.resize(strTo.size() - outbytesleft/sizeof(tchar));
					return true;
				}
			}
			return true;
		}

	public:
		//std::optional<std::basic_string<tchar_to>> Convert(std::basic_string<tchar_from> const& strFrom) {
		//	return Convert(std::basic_string_view<tchar_from>{strFrom});
		//}
		template < concepts::tstring_like tstring >
		BSC__NODISCARD auto Convert(tstring const& svFrom) -> std::optional<std::basic_string<tchar_to>> {
			std::basic_string<tchar_to> strTo;

			using tchar_from2 = std::ranges::range_value_t<tstring>;
			static_assert(std::is_same_v<tchar_from, tchar_from2>);
			if (!IsOpen())
				return {};

			// source
			using char_source_buf_t = char;	// or char const

			char_source_buf_t* src = (char_source_buf_t*)(svFrom.data());
			size_t remnant_src = svFrom.size()*sizeof(tchar_from);

			bool bResult = false;
			if constexpr (std::is_same_v<tchar_from, charKSSM_t>) {
				std::string strMBCS;
				strMBCS.reserve(svFrom.size() * sizeof(tchar_from));
				for (auto c : svFrom) {
					if (c & 0xff00)
						strMBCS.push_back(c >> 8);
					strMBCS.push_back(c & 0xff);
				}
				src = strMBCS.data();
				remnant_src = strMBCS.size();
				bResult = ContinueConvert(strTo, &src, remnant_src);
			}
			else if constexpr (std::is_same_v<tchar_to, charKSSM_t>) {
				std::string strMBCS;
				if (!ContinueConvert(strMBCS, &src, remnant_src))
					return {};
				size_t len_kssm{};
				for (auto p = strMBCS.begin(), e = strMBCS.end(); p < e; p++) {
					if (*p & 0x80) {
						len_kssm++;
						p++;
					}
				}
				strTo.assign(strMBCS.size() - len_kssm, 0);
				auto q = strTo.begin();
				for (auto p = strMBCS.begin(), e = strMBCS.end(); p < e; p++, q++) {
					*q = (*p & 0xff);
					if (*p & 0x80) {
						p++;
						*q = ((*q << 8) | (*p & 0xff));
					}
				}
				bResult = true;
			}
			else if constexpr (initial_dst_buf_size != 0) {
				std::array<tchar_to, initial_dst_buf_size> initial_dst_buf{};
				size_t remnant_dst = sizeof(initial_dst_buf);
				char* out = (char*)initial_dst_buf.data();
				if (auto converted = iconv(m_cd, &src, &remnant_src, &out, &remnant_dst); converted == (size_t)-1) {
					auto e = errno;
					if (e == E2BIG) {
						strTo.assign(initial_dst_buf.data(), initial_dst_buf.size()-remnant_dst/sizeof(tchar_to));
						bResult = ContinueConvert(strTo, &src, remnant_src);
					}
				}
				else {
					bResult = true;
					strTo.assign(initial_dst_buf.data(), initial_dst_buf.size()-remnant_dst/sizeof(tchar_to));
				}
			} else {
				//strTo.resize(svFrom.size());
				//size_t remnant_dst = strTo.size()*sizeof(tchar_to);

				//bResult = Conv(strTo, &src, remnant_src, remnant_dst);
				bResult = ContinueConvert(strTo, &src, remnant_src);
			}

			if (bResult)
				return std::move(strTo);
			return {};
		}

	public:
		template < concepts::string_elem tchar >
		BSC__NODISCARD constexpr static auto GuessCodeFromType() -> char const* {
			using namespace std::literals;
			if constexpr (std::is_same_v<tchar, char8_t> || std::is_same_v<tchar, char>) {
				return "UTF-8";
			}
			else if constexpr (std::is_same_v<tchar, char16_t>) {
				if constexpr (std::endian::native == std::endian::little) {
					return "UTF-16LE";
				} else {
					return "UTF-16BE";
				}
			}
			else if constexpr (std::is_same_v<tchar, char32_t>) {
				if constexpr (std::endian::native == std::endian::little) {
					return "UTF-32LE";
				} else {
					return "UTF-32BE";
				}
			}
			else if constexpr (std::is_same_v<tchar, wchar_t>) {
				return "wchar_t";
			} else if (std::is_same_v<tchar, char>) {
				return "char";
			}
			else if constexpr (std::is_same_v<tchar, charKSSM_t>) {
				return "JOHAB";
			}
			else {
				static_assert(false, "Unsupported type");
			}
		};

	protected:
		BSC__NODISCARD bool IsFlag(int flag) const {
			int value {};
			iconvctl(m_cd, flag, &value);
			return value != 0;
		}
		void SetFlag(int flag, bool bOn) {
			int bValue { bOn };
			iconvctl(m_cd, flag, &bValue);
		}
	};

#pragma pack(pop)
}
