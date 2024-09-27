#include <catch.hpp>
#include "biscuit/biscuit.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_ctre.h"

import std;
import biscuit;

using namespace std::literals;
namespace concepts = biscuit::concepts;

constexpr static auto const ATTR = "[archive]";

template <typename tchar, typename tarchive >
std::vector<std::basic_string<tchar>> ReadFile(tarchive& ar) {
	std::vector<std::basic_string<tchar>> strs;
	for (std::optional<std::basic_string<tchar>> r; true; ) {
		r = ar.template ReadLine<tchar>();
		if (!r)
			break;
		strs.emplace_back(std::move(*r));
	}
	return strs;
};


namespace test_archive {

	using namespace biscuit;

	auto g_eCodepageMBCS = eCODEPAGE::KO_KR_949;

	TEST_CASE("archive read string", ATTR) { 

		std::vector<std::u8string> const strs0 {
			u8R"xx(#include <iostream>)xx",
			u8R"xx(#include <string>)xx",
			u8R"xx(#include <string_view>)xx",
			u8R"xx()xx",
			u8R"xx(using namespace std::literals;)xx",
			u8R"xx()xx",
			u8R"xx(int main() {)xx",
			u8R"xx(	std::cout << u8"가나다라마바사";)xx",
			u8R"xx(})xx",
		};

		std::error_code ec{};
		std::filesystem::create_directories(uR"x(.\stream_test\)x", ec);
		for (auto const& dir : std::filesystem::directory_iterator(uR"x(.\stream_test\)x", ec)) {
			if (dir.is_directory() or dir.is_other())
				continue;
			std::u16string strFilename(dir.path().filename().u16string());
			auto str2 = biscuit::ToUpper(strFilename);
			if (auto m = ctre::match<uR"xx(.*\.[Bb][Oo][Mm]\.[Cc][Xx][Xx]$)xx">(strFilename); !m)
				continue;
			//std::basic_regex<wchar_t> re{LR"xx(.*\.[Bb][Oo][Mm]\.[Cc][Xx][Xx]$)xx"};
			//if (!std::regex_match((std::wstring&)strFilename, re))
			//	continue;

			biscuit::xIFArchive ar(dir.path());

			ar.m_stream.seekg(0);
			[[maybe_unused]] auto codepage = ar.ReadCodepageBOM(eCODEPAGE::UTF8);

			auto strsA = ReadFile<char>(ar);
			REQUIRE(strs0.size() == strsA.size());
			for (int i = 0; i < strs0.size(); i++) {
				//REQUIRE(biscuit::Compare(strsA[i], strs0[i]) == 0);
				bool b = biscuit::Compare(strsA[i], strs0[i]) == 0;
				REQUIRE(b);
			}
		}
		REQUIRE(!ec);

		{
			xIFArchive ar(uR"x(.\stream_test\short file.txt)x");
			ar.ReadCodepageBOM(eCODEPAGE::DEFAULT);
			auto strs = ReadFile<char16_t>(ar);

			REQUIRE(strs.size() == 1);
			REQUIRE(strs[0] == u"n"sv);
		}

		{
			xIFArchive ar(uR"x(.\stream_test\cp949.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)949);
			auto strs = ReadFile<wchar_t>(ar);

			REQUIRE(strs.size() == 1);
			REQUIRE(strs[0] == L"가나다라마바사"sv);
		}

		{
			xIFArchive ar(uR"x(.\stream_test\cp949.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)949);
			auto strs = ReadFile<char32_t>(ar);

			REQUIRE(strs.size() == 1);
			REQUIRE(strs[0] == U"가나다라마바사"sv);
		}

	#if 1
		if (not std::filesystem::exists(uR"x(.\stream_test\cp1250.txt)x")) {
			static std::vector<unsigned char> const na1250 { 0x81, 0x83, 0x88, 0x90, 0x98, 0xA0, 0xAD };
			static std::vector<unsigned char> const na1251 { 0x98, 0xA0, 0xAD };
			static std::vector<unsigned char> const na1252 { 0x81, 0x8d, 0x8f, 0x90, 0x9d, 0xA0, 0xAD };

			struct {
				int eCodepage{};
				std::vector<unsigned char> const& na;
			} tbl[3] = {
				{1250, na1250},
				{1251, na1251},
				{1252, na1252},
			};

			for (auto const& [eCodepage, na] : tbl) {
				std::filesystem::path path = fmt::format(uR"x(.\stream_test\cp{}.txt)x", eCodepage);
				std::ofstream stream(path);
				for (unsigned char c = 0x80; c >= 0x80; c++) {
					if (c > 0x80 and !(c & 0x0f))
						stream.write("\n", 1);
					if (std::find(na.begin(), na.end(), c) != na.end())
						continue;
					stream.write(&(char&)c, 1);
				}
			}
		}

		// cp1250 -> char8_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1250.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1250);
			auto strs = ReadFile<char8_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == u8"€‚„…†‡‰Š‹ŚŤŽŹ"sv);
			REQUIRE(strs[1] == u8"‘’“”•–—™š›śťžź"sv);
			REQUIRE(strs[2] == u8"ˇ˘Ł¤Ą¦§¨©Ş«¬®Ż"sv);
			REQUIRE(strs[3] == u8"°±˛ł´µ¶·¸ąş»Ľ˝ľż"sv);
			REQUIRE(strs[4] == u8"ŔÁÂĂÄĹĆÇČÉĘËĚÍÎĎ"sv);
			REQUIRE(strs[5] == u8"ĐŃŇÓÔŐÖ×ŘŮÚŰÜÝŢß"sv);
			REQUIRE(strs[6] == u8"ŕáâăäĺćçčéęëěíîď"sv);
			REQUIRE(strs[7] == u8"đńňóôőö÷řůúűüýţ˙"sv);
		}

		// cp1250 -> char16_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1250.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1250);
			auto strs = ReadFile<char16_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == u"€‚„…†‡‰Š‹ŚŤŽŹ"sv);
			REQUIRE(strs[1] == u"‘’“”•–—™š›śťžź"sv);
			REQUIRE(strs[2] == u"ˇ˘Ł¤Ą¦§¨©Ş«¬®Ż"sv);
			REQUIRE(strs[3] == u"°±˛ł´µ¶·¸ąş»Ľ˝ľż"sv);
			REQUIRE(strs[4] == u"ŔÁÂĂÄĹĆÇČÉĘËĚÍÎĎ"sv);
			REQUIRE(strs[5] == u"ĐŃŇÓÔŐÖ×ŘŮÚŰÜÝŢß"sv);
			REQUIRE(strs[6] == u"ŕáâăäĺćçčéęëěíîď"sv);
			REQUIRE(strs[7] == u"đńňóôőö÷řůúűüýţ˙"sv);
		}
		// cp1250 -> char32_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1250.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1250);
			auto strs = ReadFile<char32_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == U"€‚„…†‡‰Š‹ŚŤŽŹ"sv);
			REQUIRE(strs[1] == U"‘’“”•–—™š›śťžź"sv);
			REQUIRE(strs[2] == U"ˇ˘Ł¤Ą¦§¨©Ş«¬®Ż"sv);
			REQUIRE(strs[3] == U"°±˛ł´µ¶·¸ąş»Ľ˝ľż"sv);
			REQUIRE(strs[4] == U"ŔÁÂĂÄĹĆÇČÉĘËĚÍÎĎ"sv);
			REQUIRE(strs[5] == U"ĐŃŇÓÔŐÖ×ŘŮÚŰÜÝŢß"sv);
			REQUIRE(strs[6] == U"ŕáâăäĺćçčéęëěíîď"sv);
			REQUIRE(strs[7] == U"đńňóôőö÷řůúűüýţ˙"sv);
		}
		// cp1250 -> wchar_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1250.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1250);
			auto strs = ReadFile<wchar_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == L"€‚„…†‡‰Š‹ŚŤŽŹ"sv);
			REQUIRE(strs[1] == L"‘’“”•–—™š›śťžź"sv);
			REQUIRE(strs[2] == L"ˇ˘Ł¤Ą¦§¨©Ş«¬®Ż"sv);
			REQUIRE(strs[3] == L"°±˛ł´µ¶·¸ąş»Ľ˝ľż"sv);
			REQUIRE(strs[4] == L"ŔÁÂĂÄĹĆÇČÉĘËĚÍÎĎ"sv);
			REQUIRE(strs[5] == L"ĐŃŇÓÔŐÖ×ŘŮÚŰÜÝŢß"sv);
			REQUIRE(strs[6] == L"ŕáâăäĺćçčéęëěíîď"sv);
			REQUIRE(strs[7] == L"đńňóôőö÷řůúűüýţ˙"sv);
		}

		// cp1251 -> char8_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1251.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1251);
			auto strs = ReadFile<char8_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == u8"ЂЃ‚ѓ„…†‡€‰Љ‹ЊЌЋЏ"sv);
			REQUIRE(strs[1] == u8"ђ‘’“”•–—™љ›њќћџ"sv);
			REQUIRE(strs[2] == u8"ЎўЈ¤Ґ¦§Ё©Є«¬®Ї"sv);
			REQUIRE(strs[3] == u8"°±Ііґµ¶·ё№є»јЅѕї"sv);
			REQUIRE(strs[4] == u8"АБВГДЕЖЗИЙКЛМНОП"sv);
			REQUIRE(strs[5] == u8"РСТУФХЦЧШЩЪЫЬЭЮЯ"sv);
			REQUIRE(strs[6] == u8"абвгдежзийклмноп"sv);
			REQUIRE(strs[7] == u8"рстуфхцчшщъыьэюя"sv);
		}
		// cp1251 -> char16_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1251.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1251);
			auto strs = ReadFile<char16_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == u"ЂЃ‚ѓ„…†‡€‰Љ‹ЊЌЋЏ"sv);
			REQUIRE(strs[1] == u"ђ‘’“”•–—™љ›њќћџ"sv);
			REQUIRE(strs[2] == u"ЎўЈ¤Ґ¦§Ё©Є«¬®Ї"sv);
			REQUIRE(strs[3] == u"°±Ііґµ¶·ё№є»јЅѕї"sv);
			REQUIRE(strs[4] == u"АБВГДЕЖЗИЙКЛМНОП"sv);
			REQUIRE(strs[5] == u"РСТУФХЦЧШЩЪЫЬЭЮЯ"sv);
			REQUIRE(strs[6] == u"абвгдежзийклмноп"sv);
			REQUIRE(strs[7] == u"рстуфхцчшщъыьэюя"sv);
		}
		// cp1251 -> char32_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1251.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1251);
			auto strs = ReadFile<char32_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == U"ЂЃ‚ѓ„…†‡€‰Љ‹ЊЌЋЏ"sv);
			REQUIRE(strs[1] == U"ђ‘’“”•–—™љ›њќћџ"sv);
			REQUIRE(strs[2] == U"ЎўЈ¤Ґ¦§Ё©Є«¬®Ї"sv);
			REQUIRE(strs[3] == U"°±Ііґµ¶·ё№є»јЅѕї"sv);
			REQUIRE(strs[4] == U"АБВГДЕЖЗИЙКЛМНОП"sv);
			REQUIRE(strs[5] == U"РСТУФХЦЧШЩЪЫЬЭЮЯ"sv);
			REQUIRE(strs[6] == U"абвгдежзийклмноп"sv);
			REQUIRE(strs[7] == U"рстуфхцчшщъыьэюя"sv);
		}
		// cp1251 -> wchar_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1251.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1251);
			auto strs = ReadFile<wchar_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == L"ЂЃ‚ѓ„…†‡€‰Љ‹ЊЌЋЏ"sv);
			REQUIRE(strs[1] == L"ђ‘’“”•–—™љ›њќћџ"sv);
			REQUIRE(strs[2] == L"ЎўЈ¤Ґ¦§Ё©Є«¬®Ї"sv);
			REQUIRE(strs[3] == L"°±Ііґµ¶·ё№є»јЅѕї"sv);
			REQUIRE(strs[4] == L"АБВГДЕЖЗИЙКЛМНОП"sv);
			REQUIRE(strs[5] == L"РСТУФХЦЧШЩЪЫЬЭЮЯ"sv);
			REQUIRE(strs[6] == L"абвгдежзийклмноп"sv);
			REQUIRE(strs[7] == L"рстуфхцчшщъыьэюя"sv);
		}

		// cp1252 -> char8_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1252.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1252);
			auto strs = ReadFile<char8_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == u8"€‚ƒ„…†‡ˆ‰Š‹ŒŽ"sv);
			REQUIRE(strs[1] == u8"‘’“”•–—˜™š›œžŸ"sv);
			REQUIRE(strs[2] == u8"¡¢£¤¥¦§¨©ª«¬®¯"sv);
			REQUIRE(strs[3] == u8"°±²³´µ¶·¸¹º»¼½¾¿"sv);
			REQUIRE(strs[4] == u8"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"sv);
			REQUIRE(strs[5] == u8"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"sv);
			REQUIRE(strs[6] == u8"àáâãäåæçèéêëìíîï"sv);
			REQUIRE(strs[7] == u8"ðñòóôõö÷øùúûüýþÿ"sv);
		}
		// cp1252 -> char16_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1252.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1252);
			auto strs = ReadFile<char16_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == u"€‚ƒ„…†‡ˆ‰Š‹ŒŽ"sv);
			REQUIRE(strs[1] == u"‘’“”•–—˜™š›œžŸ"sv);
			REQUIRE(strs[2] == u"¡¢£¤¥¦§¨©ª«¬®¯"sv);
			REQUIRE(strs[3] == u"°±²³´µ¶·¸¹º»¼½¾¿"sv);
			REQUIRE(strs[4] == u"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"sv);
			REQUIRE(strs[5] == u"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"sv);
			REQUIRE(strs[6] == u"àáâãäåæçèéêëìíîï"sv);
			REQUIRE(strs[7] == u"ðñòóôõö÷øùúûüýþÿ"sv);
		}
		// cp1252 -> char32_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1252.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1252);
			auto strs = ReadFile<char32_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == U"€‚ƒ„…†‡ˆ‰Š‹ŒŽ"sv);
			REQUIRE(strs[1] == U"‘’“”•–—˜™š›œžŸ"sv);
			REQUIRE(strs[2] == U"¡¢£¤¥¦§¨©ª«¬®¯"sv);
			REQUIRE(strs[3] == U"°±²³´µ¶·¸¹º»¼½¾¿"sv);
			REQUIRE(strs[4] == U"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"sv);
			REQUIRE(strs[5] == U"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"sv);
			REQUIRE(strs[6] == U"àáâãäåæçèéêëìíîï"sv);
			REQUIRE(strs[7] == U"ðñòóôõö÷øùúûüýþÿ"sv);
		}
		// cp1252 -> wchar_t
		{
			xIFArchive ar(uR"x(.\stream_test\cp1252.txt)x");
			ar.ReadCodepageBOM((eCODEPAGE)1252);
			auto strs = ReadFile<wchar_t>(ar);

			REQUIRE(strs.size() == 8);
			REQUIRE(strs[0] == L"€‚ƒ„…†‡ˆ‰Š‹ŒŽ"sv);
			REQUIRE(strs[1] == L"‘’“”•–—˜™š›œžŸ"sv);
			REQUIRE(strs[2] == L"¡¢£¤¥¦§¨©ª«¬®¯"sv);
			REQUIRE(strs[3] == L"°±²³´µ¶·¸¹º»¼½¾¿"sv);
			REQUIRE(strs[4] == L"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ"sv);
			REQUIRE(strs[5] == L"ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"sv);
			REQUIRE(strs[6] == L"àáâãäåæçèéêëìíîï"sv);
			REQUIRE(strs[7] == L"ðñòóôõö÷øùúûüýþÿ"sv);
		}
	#endif
	}


	TEST_CASE("WriteLine", ATTR) {
	#if 0
		using namespace std;
		using namespace biscuit;

		// WriteLine UTF16
		static vector<variant<u8string, u16string, u32string, wstring, string>> const strs1 { u8"가나다라", u"마바사", U"아자차카", L"타파하", };
		static vector<variant<u8string, u16string, u32string, wstring, string>> const strs2 { u8"ABCDEF", u"0123456789", U"abcdef", L"0123456789", "ABCDEF012345" };
		{
			std::ifstream st;
			st.close();
			xOFArchive ar(uR"x(.\stream_test\write_line_u16.txt)x");
			ar.WriteCodepageBOM(eCODEPAGE::UTF16);
			for (auto const& vstr : strs1) {
				std::visit([&ar](auto const& str) {
					using char_t = std::decay_t<decltype(str)>::value_type;
					ar.WriteLine("{}", str);
						   }, vstr);
			}
			for (auto const& vstr : strs2) {
				std::visit([&ar](auto const& str) {
					using char_t = std::decay_t<decltype(str)>::value_type;
					ar.WriteLine(TStringLiteral<char_t, "{}">(), str);
						   }, vstr);
			}
		}


	#endif
	}

	TEST_CASE("base64", ATTR) {
		std::string data = "HELLO, World!";
		std::string base64 = biscuit::ToBase64<char>({data.data(), data.size()});
		auto [r, data2] = biscuit::FromBase64(base64);
		REQUIRE(std::ranges::equal(data, data2));
	}

}	// namespace test_archive

