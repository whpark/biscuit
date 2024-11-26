#include <catch.hpp>
//#include <fmt/core.h>
//#include <fmt/std.h>
//#include <fmt/xchar.h>
#include <simdutf.h>

import std;
import fmt;
import biscuit;

using namespace std::literals;

constexpr static auto const ATTR = "[string]";


TEST_CASE("convert_codepage", ATTR) {
#define SRC_TEXT "가a나b다c라d마바사똠"
#define COMPOSE3(x, y, z) x##y##z
#define COMPOSE2(x, y) COMPOSE3(x, y, s)
#define SRC(sig) COMPOSE2(sig, SRC_TEXT)

	SECTION("iconv_wrapper") {
		biscuit::Ticonv<char8_t, char> iconv("", "CP949");
		unsigned char str[] = { 0xb0, 0xa1, 'a', 0xb3, 0xaa, 'b', 0xb4, 0xd9, 'c', 0xb6, 0xf3, 'd', 0xb8, 0xb6, 0xb9, 0xd9, 0xbb, 0xe7, 0x8c, 0x63 };
		auto r = iconv.Convert(std::string_view((char*)str, std::size(str)));
		REQUIRE((bool)r);
		REQUIRE(*r == SRC(u8));
		auto str2 = biscuit::ConvertStringFromCP949<wchar_t>({(char*)str, std::size(str)});
		REQUIRE(str2 == SRC(L));
		auto str3 = biscuit::ConvertString_iconv<wchar_t>(u8"asdfsdf");
		REQUIRE(str3);
		REQUIRE(*str3 == L"asdfsdf");

		auto str4 = biscuit::ConvertString_iconv<wchar_t>(std::string_view((char const*)str, std::size(str)), "", "CP949");
		REQUIRE(str4);
		REQUIRE(*str4 == L"가a나b다c라d마바사똠");
	}
	SECTION("iconv_wrapper") {
		biscuit::Ticonv<biscuit::charKSSM_t, wchar_t> iconv;
		auto str0 = SRC(L);
		auto r = iconv.Convert(str0);
		REQUIRE((bool)r);
		{
			using namespace biscuit::charset;
			using enum biscuit::charset::eKSSM;
			auto str = *r;
			REQUIRE(str.size() >= 12);
			REQUIRE(str[ 0] == ComposeKSSM(Ini_G, Mid_R, Fin_Fill));	// '가'
			REQUIRE(str[ 1] == 'a');
			REQUIRE(str[ 2] == ComposeKSSM(Ini_N, Mid_R, Fin_Fill));	// '나'
			REQUIRE(str[ 3] == 'b');
			REQUIRE(str[ 4] == ComposeKSSM(Ini_D, Mid_R, Fin_Fill));	// '다'
			REQUIRE(str[ 5] == 'c');
			REQUIRE(str[ 6] == ComposeKSSM(Ini_L, Mid_R, Fin_Fill));	// '라'
			REQUIRE(str[ 7] == 'd');
			REQUIRE(str[ 8] == ComposeKSSM(Ini_M, Mid_R, Fin_Fill));	// '마'
			REQUIRE(str[ 9] == ComposeKSSM(Ini_B, Mid_R, Fin_Fill));	// '바'
			REQUIRE(str[10] == ComposeKSSM(Ini_S, Mid_R, Fin_Fill));	// '사'
			REQUIRE(str[11] == ComposeKSSM(Ini_DD, Mid_U, Fin_M));	// '똠'
			REQUIRE(str[ 0] == ComposeKSSM(L'ㄱ', L'ㅏ'));	// '가'
			REQUIRE(str[ 2] == ComposeKSSM(L'ㄴ', L'ㅏ'));	// '나'
			REQUIRE(str[ 4] == ComposeKSSM(L'ㄷ', L'ㅏ'));	// '다'
			REQUIRE(str[ 6] == ComposeKSSM(L'ㄹ', L'ㅏ'));	// '라'
			REQUIRE(str[ 8] == ComposeKSSM(L'ㅁ', L'ㅏ'));	// '마'
			REQUIRE(str[ 9] == ComposeKSSM(L'ㅂ', L'ㅏ'));	// '바'
			REQUIRE(str[10] == ComposeKSSM(L'ㅅ', L'ㅏ'));	// '사'
			REQUIRE(str[11] == ComposeKSSM(L'ㄸ', L'ㅗ', L'ㅁ'));	// '똠'
		}
		auto str2 = biscuit::ConvertStringFromKSSM<wchar_t>(*r);
		REQUIRE(str2 == SRC(L));
	}

	SECTION("utf from u8") {
		auto org = SRC(u8);
		auto u8 = biscuit::ConvertString<char8_t>(org);
		auto u16 = biscuit::ConvertString<char16_t>(org);
		auto u32 = biscuit::ConvertString<char32_t>(org);
		auto w = biscuit::ConvertString<wchar_t>(org);
		REQUIRE(u8 == SRC(u8));
		REQUIRE(u16 == SRC(u));
		REQUIRE(u32 == SRC(U));
		REQUIRE(w == SRC(L));
	}
	SECTION("utf from u") {
		auto org = SRC(u);
		auto u8 = biscuit::ConvertString<char8_t>(org);
		auto u16 = biscuit::ConvertString<char16_t>(org);
		auto u32 = biscuit::ConvertString<char32_t>(org);
		auto w = biscuit::ConvertString<wchar_t>(org);
		REQUIRE(u8 == SRC(u8));
		REQUIRE(u16 == SRC(u));
		REQUIRE(u32 == SRC(U));
		REQUIRE(w == SRC(L));
	}
	SECTION("utf from U") {
		auto org = SRC(U);
		auto u8 = biscuit::ConvertString<char8_t>(org);
		auto u16 = biscuit::ConvertString<char16_t>(org);
		auto u32 = biscuit::ConvertString<char32_t>(org);
		auto w = biscuit::ConvertString<wchar_t>(org);
		REQUIRE(u8 == SRC(u8));
		REQUIRE(u16 == SRC(u));
		REQUIRE(u32 == SRC(U));
		REQUIRE(w == SRC(L));
	}
	SECTION("utf from L") {
		auto org = SRC(L);
		auto u8 = biscuit::ConvertString<char8_t>(org);
		auto u16 = biscuit::ConvertString<char16_t>(org);
		auto u32 = biscuit::ConvertString<char32_t>(org);
		auto w = biscuit::ConvertString<wchar_t>(org);
		REQUIRE(u8 == SRC(u8));
		REQUIRE(u16 == SRC(u));
		REQUIRE(u32 == SRC(U));
		REQUIRE(w == SRC(L));
	}
}
