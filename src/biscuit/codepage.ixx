module;

#include "biscuit/macro.h"

export module biscuit.codepage;
import std;
import biscuit.aliases;

export namespace biscuit {


	//------------------------------------------------------------------------------------------
	/// @brief Converts Codepage (Unicode <-> MBCS ...)
	enum class eCODEPAGE : int {
		DEFAULT__OR_USE_MBCS_CODEPAGE = 0,	// Default or use MBCS Codepage (eCODEPAGE g_eCodepageMBCS;)
		DEFAULT = 0,

		ANSI_WINDOWS = 1252,

		UTF7 = 65000,
		UTF8 = 65001,

		UTF16LE = 1200,
		UTF16BE = 1201,
		UTF16 = (std::endian::native == std::endian::little) ? UTF16LE : UTF16BE,
		_UTF16_other = (UTF16 == UTF16BE) ? UTF16LE : UTF16BE,

		UTF32LE = 12000,
		UTF32BE = 12001,
		UTF32 = (std::endian::native == std::endian::little) ? UTF32LE : UTF32BE,
		_UTF32_other = (UTF32 == UTF32BE) ? UTF32LE : UTF32BE,

		UCS2LE = sizeof(wchar_t) == sizeof(char16_t) ? UTF16LE : UTF32LE,
		UCS2BE = sizeof(wchar_t) == sizeof(char16_t) ? UTF16BE : UTF32BE,
		UCS2 = (std::endian::native == std::endian::little) ? UCS2LE : UCS2BE,
		_UCS2_other = (UCS2 == UCS2BE) ? UCS2LE : UCS2BE,


		//------------------------

		KO_KR_949 = 949,
		KO_KR_JOHAB_KSSM_1361 = 1361,	// 조합형 KSSM

	};

	template <typename tchar>	constexpr eCODEPAGE const eCODEPAGE_DEFAULT = eCODEPAGE::DEFAULT;
	template <>					constexpr eCODEPAGE const eCODEPAGE_DEFAULT<char> = eCODEPAGE::DEFAULT;
	template <>					constexpr eCODEPAGE const eCODEPAGE_DEFAULT<char8_t> = eCODEPAGE::UTF8;
	template <>					constexpr eCODEPAGE const eCODEPAGE_DEFAULT<char16_t> = eCODEPAGE::UTF16;
	template <>					constexpr eCODEPAGE const eCODEPAGE_DEFAULT<char32_t> = eCODEPAGE::UTF32;
	template <>					constexpr eCODEPAGE const eCODEPAGE_DEFAULT<wchar_t> = eCODEPAGE::UCS2;

	template <typename tchar>	constexpr eCODEPAGE const eCODEPAGE_OTHER_ENDIAN = eCODEPAGE::DEFAULT;
	template <>					constexpr eCODEPAGE const eCODEPAGE_OTHER_ENDIAN<char16_t> = eCODEPAGE::_UTF16_other;
	template <>					constexpr eCODEPAGE const eCODEPAGE_OTHER_ENDIAN<char32_t> = eCODEPAGE::_UTF32_other;
	template <>					constexpr eCODEPAGE const eCODEPAGE_OTHER_ENDIAN<wchar_t> = eCODEPAGE::_UCS2_other;

	template < eCODEPAGE eCodepage > struct char_type_from {
		using char_type = void;
	};
	template <> struct char_type_from<eCODEPAGE::DEFAULT>				{ using char_type = char; };
	template <> struct char_type_from<eCODEPAGE::UTF8>					{ using char_type = char8_t; };
	template <> struct char_type_from<eCODEPAGE::UTF16LE>				{ using char_type = char16_t; };
	template <> struct char_type_from<eCODEPAGE::UTF16BE>				{ using char_type = char16_t; };
	template <> struct char_type_from<eCODEPAGE::UTF32LE>				{ using char_type = char32_t; };
	template <> struct char_type_from<eCODEPAGE::UTF32BE>				{ using char_type = char32_t; };
	template <> struct char_type_from<eCODEPAGE::KO_KR_949>				{ using char_type = char; };
	template <> struct char_type_from<eCODEPAGE::KO_KR_JOHAB_KSSM_1361>	{ using char_type = charKSSM_t; };

	BSC__NODISCARD constexpr inline std::string_view GetCodepageBOM(eCODEPAGE eCodepage) {
		using namespace std::literals;
		switch (eCodepage) {
			using enum eCODEPAGE;
		case DEFAULT : return {};
		case UTF8 : return "\xEF\xBB\xBF"sv;
		case UTF16LE : return "\xFF\xFE"sv;
		case UTF16BE : return "\xFE\xFF"sv;
		case UTF32LE : return "\xFF\xFE\x00\x00"sv;
		case UTF32BE : return "\x00\x00\xFE\xFF"sv;
		}
		return {};
	}


	//constexpr static inline eCODEPAGE GetHostCodepage() {	// Source File Encoding
	//	constexpr std::array strGA = {"\xa1\xb0"};	// 가 in CP949
	//	constexpr std::array strGA_UTF8 = {u8"가"};
	//	constexpr std::array strGA_KSC5601 = {0xa1, 0xb0, 0x00};	// '가' == 0xb0a1 in KSC5601 // CP949
	//	if (strGA.size() == strGA_UTF8.size() and (memcmp(strGA.data(), strGA_UTF8.data(), strGA.size()) == 0))
	//		return eCODEPAGE::UTF8;

	//	if (strGA.size() == strGA_KSC5601.size() and (memcmp(strGA.data(), strGA_KSC5601.data(), strGA.size()) == 0))
	//		return eCODEPAGE::KO_KR_949;

	//	return eCODEPAGE::DEFAULT;
	//}

	constexpr inline char const* GetCodepageName(eCODEPAGE eCodepage) {
		switch (eCodepage) {
			using enum eCODEPAGE;
		case UTF7 :		return "UTF-7";
		case UTF8 :		return "UTF-8";
		case UTF16LE :	return "UTF-16LE";
		case UTF16BE :	return "UTF-16BE";
		case UTF32LE :	return "UTF-32LE";
		case UTF32BE :	return "UTF-32BE";
		default :
			if (/*std::to_underlying*/(int)eCodepage) {
				thread_local static char szCodepage[32]{0};
				std::format_to(szCodepage, "CP{}", /*std::to_underlying(eCodepage)*/(int)eCodepage);
				return szCodepage;
			}
		}
		return nullptr;
	}
	template < typename tchar >
	constexpr char const* GetCodepageName() {
		return GetCodepageName(eCODEPAGE_DEFAULT<tchar>);
	}

}