#include <catch.hpp>

import std;
import biscuit;

using namespace std::literals;

constexpr static auto const ATTR = "[string]";

TEST_CASE("string.arithmetic", ATTR) {
	using namespace biscuit;

	SECTION("default") {
		char16_t sz[30];
		char16_t szS[]{u"가나다라마바사"};
		tszcpy(sz, std::basic_string_view{szS});
		static_assert(std::size(szS) == 7+1);
	}

	SECTION("tszdcmp") {
		REQUIRE(CompareNumericStrings(u""sv, u""sv) == 0);
		REQUIRE(CompareNumericStrings(u"0"sv, u""sv) > 0);
		REQUIRE(CompareNumericStrings(u""sv, u"0"sv) < 0);
		REQUIRE(CompareNumericStrings(u"1"sv, u"1"sv) == 0);
		REQUIRE(CompareNumericStrings(u"1"sv, u"2"sv) < 0);
		REQUIRE(CompareNumericStrings(u"2"sv, u"1"sv) > 0);
		REQUIRE(CompareNumericStrings(u"01"sv, u"001"sv) < 0);	// 같을 경우, 길이가 길수록 큰 값
		REQUIRE(CompareNumericStrings(u"10"sv, u"100"sv) < 0);
		REQUIRE(CompareNumericStrings(u"abcdef0123456789aaaa"sv, u"abcdef0000123456789aaaa"sv) < 0);
		REQUIRE(CompareNumericStrings(u"abcdef0123456789aaaa"sv, u"abcdef000012345678aaaa"sv) > 0);
		REQUIRE(CompareNumericStrings(u"abcdef0123456789aaaa"sv, u"abcdef00001234567891aaaa"sv) < 0);
		REQUIRE(CompareNumericStrings(u"abcdef0123456789aaaa"sv, u"abcdef0000123456788aaaa"sv) > 0);
		REQUIRE(CompareNumericStrings(u"0123456789"sv, u"0123456789"sv) == 0);
		REQUIRE(CompareNumericStrings(u"0123456789"sv, u"012345678"sv) > 0);
		REQUIRE(CompareNumericStrings(u"0123456789"sv, u"01234567891"sv) < 0);
		REQUIRE(CompareNumericStrings(u"0123456789"sv, u"0123456788"sv) > 0);

		REQUIRE(tszdcmp(u""sv, u""sv) == 0);
		REQUIRE(tszdcmp(u"0"sv, u""sv) > 0);
		REQUIRE(tszdcmp(u""sv, u"0"sv) < 0);
		REQUIRE(tszdcmp(u"1"sv, u"1"sv) == 0);
		REQUIRE(tszdcmp(u"1"sv, u"2"sv) < 0);
		REQUIRE(tszdcmp(u"2"sv, u"1"sv) > 0);
		REQUIRE(tszdcmp(u"01"sv, u"001"sv) < 0);	// 같을 경우, 길이가 길수록 큰 값
		REQUIRE(tszdcmp(u"10"sv, u"100"sv) < 0);
		REQUIRE(tszdcmp(u"abcdef0123456789aaaa"sv, u"abcdef0000123456789aaaa"sv) < 0);
		REQUIRE(tszdcmp(u"abcdef0123456789aaaa"sv, u"abcdef000012345678aaaa"sv) > 0);
		REQUIRE(tszdcmp(u"abcdef0123456789aaaa"sv, u"abcdef00001234567891aaaa"sv) < 0);
		REQUIRE(tszdcmp(u"abcdef0123456789aaaa"sv, u"abcdef0000123456788aaaa"sv) > 0);
		REQUIRE(tszdcmp(u"0123456789"sv, u"0123456789"sv) == 0);
		REQUIRE(tszdcmp(u"0123456789"sv, u"012345678"sv) > 0);
		REQUIRE(tszdcmp(u"0123456789"sv, u"01234567891"sv) < 0);
		REQUIRE(tszdcmp(u"0123456789"sv, u"0123456788"sv) > 0);
	}

	SECTION("tszto") {
		REQUIRE(tsztoi("   123456")   == 123456);
		REQUIRE(tsztoi("   123456"s)  == 123456);
		REQUIRE(tsztoi("   123456"sv) == 123456);
		REQUIRE(tsztoi("  +123456")   == 123456);
		REQUIRE(tsztoi("  +123456"s)  == 123456);
		REQUIRE(tsztoi("  +123456"sv) == 123456);
		REQUIRE(tszto<uint64_t>("0xABCD'EFab'1234'9853", 0, '\'') == 0xABCD'EFab'1234'9853llu);
		REQUIRE(tszto<int64_t>("0xABCD'EFab'1234'9853", 0, '\'') == 0);	// too big for int64_t
		REQUIRE(tszto<int64_t>("0xABCD'EFab'1234'9853", 0) == 0xABCD);	// no separator

		REQUIRE(tsztod("1.1")		== 1.1);
		REQUIRE(tsztod("1.1"s)		== 1.1);
		REQUIRE(tsztod("1.1"sv)		== 1.1);
		REQUIRE(tsztod("+1.1")		== 1.1);
		REQUIRE(tsztod("1.3e3")		== 1.3e3);
		REQUIRE(tsztod("-1.1")		== -1.1);
		REQUIRE(tsztod("-1.3e3")	== -1.3e3);
		REQUIRE(tsztod("1.3e-3")	== 1.3e-3);
		REQUIRE(tsztod("-1.3e-3")	== -1.3e-3);
	}

	SECTION("AddThousandComma") {

	#define EXPECT_EQ(a, b) REQUIRE(a == b)
		//std::locale::global(std::locale("en_US.UTF-8"));
		auto str = std::format(std::locale("en_US.UTF-8"), "{:L}", 123'456'789);
		EXPECT_EQ(str, "123,456,789");

		EXPECT_EQ(AddThousandComma(L"1"sv),											L"1"sv);
		EXPECT_EQ(AddThousandComma(L"12"sv),										L"12"sv);
		EXPECT_EQ(AddThousandComma(L"123"sv),										L"123"sv);
		EXPECT_EQ(AddThousandComma(L"1234"sv),										L"1,234"sv);
		EXPECT_EQ(AddThousandComma(L"12345"sv),										L"12,345"sv);
		EXPECT_EQ(AddThousandComma(L"123456"sv),									L"123,456"sv);
		EXPECT_EQ(AddThousandComma(L"1234567"sv),									L"1,234,567"sv);
		EXPECT_EQ(AddThousandComma(L"12345678"sv),									L"12,345,678"sv);
		EXPECT_EQ(AddThousandComma(L"123456789"sv),									L"123,456,789"sv);
		EXPECT_EQ(AddThousandComma(L"1234567890"sv),								L"1,234,567,890"sv);
		EXPECT_EQ(AddThousandComma(L"12345678901"sv),								L"12,345,678,901"sv);
		EXPECT_EQ(AddThousandComma(L"123456789012"sv),								L"123,456,789,012"sv);
		EXPECT_EQ(AddThousandComma(L"1234567890123"sv),								L"1,234,567,890,123"sv);
		EXPECT_EQ(AddThousandComma(L"12345678901234"sv),							L"12,345,678,901,234"sv);
		EXPECT_EQ(AddThousandComma(L"123456789012345"sv),							L"123,456,789,012,345"sv);
		EXPECT_EQ(AddThousandComma(L"1234567890123456"sv),							L"1,234,567,890,123,456"sv);
		EXPECT_EQ(AddThousandComma(L"12345678901234567"sv),							L"12,345,678,901,234,567"sv);
		EXPECT_EQ(AddThousandComma(L"123456789012345678"sv),						L"123,456,789,012,345,678"sv);
		EXPECT_EQ(AddThousandComma(L"1234567890123456789"sv),						L"1,234,567,890,123,456,789"sv);
		EXPECT_EQ(AddThousandComma(L"12345678901234567890"sv),						L"12,345,678,901,234,567,890"sv);
		EXPECT_EQ(AddThousandComma(L"   -12345678901234567.123332342738491234"sv),	L"   -12,345,678,901,234,567.123332342738491234"sv);
		EXPECT_EQ(AddThousandComma(L"aslkdfjasld;kfgjasd;fn3232asdfasd"sv),			L"aslkdfjasld;kfgjasd;fn3,232asdfasd"sv);
		EXPECT_EQ(AddThousandComma(L"aslkdfjasld;kfgjasd;fn323asdfasd"sv),			L"aslkdfjasld;kfgjasd;fn323asdfasd"sv);
		EXPECT_EQ(AddThousandComma(L"aslkdfjasld;kfgjasd;fn32asdfasd"sv),			L"aslkdfjasld;kfgjasd;fn32asdfasd"sv);
		EXPECT_EQ(AddThousandComma(L"aslkdfjasld;kfgjasd;fn3asdfasd"sv),			L"aslkdfjasld;kfgjasd;fn3asdfasd"sv);
		EXPECT_EQ(AddThousandComma(L"aslkdfjasld;kfgjasd;fn3232"sv),				L"aslkdfjasld;kfgjasd;fn3,232"sv);
		EXPECT_EQ(AddThousandComma(L"aslkdfjasld;kfgjasd;fn323"sv),					L"aslkdfjasld;kfgjasd;fn323"sv);
		EXPECT_EQ(AddThousandComma(L"aslkdfjasld;kfgjasd;fn32"sv),					L"aslkdfjasld;kfgjasd;fn32"sv);
		EXPECT_EQ(AddThousandComma(L"aslkdfjasld;kfgjasd;fn3"sv),					L"aslkdfjasld;kfgjasd;fn3"sv);
		EXPECT_EQ(AddThousandComma(L"3232asdfasd"sv),								L"3,232asdfasd"sv);
		EXPECT_EQ(AddThousandComma(L"323asdfasd"sv),								L"323asdfasd"sv);
		EXPECT_EQ(AddThousandComma(L"32asdfasd"sv),									L"32asdfasd"sv);
		EXPECT_EQ(AddThousandComma(L"3asdfasd"sv),									L"3asdfasd"sv);

		EXPECT_EQ(AddThousandComma("1"sv),											"1"sv);
		EXPECT_EQ(AddThousandComma("12"sv),											"12"sv);
		EXPECT_EQ(AddThousandComma("123"sv),										"123"sv);
		EXPECT_EQ(AddThousandComma("1234"sv),										"1,234"sv);
		EXPECT_EQ(AddThousandComma("12345"sv),										"12,345"sv);
		EXPECT_EQ(AddThousandComma("123456"sv),										"123,456"sv);
		EXPECT_EQ(AddThousandComma("1234567"sv),									"1,234,567"sv);
		EXPECT_EQ(AddThousandComma("12345678"sv),									"12,345,678"sv);
		EXPECT_EQ(AddThousandComma("123456789"sv),									"123,456,789"sv);
		EXPECT_EQ(AddThousandComma("1234567890"sv),									"1,234,567,890"sv);
		EXPECT_EQ(AddThousandComma("12345678901"sv),								"12,345,678,901"sv);
		EXPECT_EQ(AddThousandComma("123456789012"sv),								"123,456,789,012"sv);
		EXPECT_EQ(AddThousandComma("1234567890123"sv),								"1,234,567,890,123"sv);
		EXPECT_EQ(AddThousandComma("12345678901234"sv),								"12,345,678,901,234"sv);
		EXPECT_EQ(AddThousandComma("123456789012345"sv),							"123,456,789,012,345"sv);
		EXPECT_EQ(AddThousandComma("1234567890123456"sv),							"1,234,567,890,123,456"sv);
		EXPECT_EQ(AddThousandComma("12345678901234567"sv),							"12,345,678,901,234,567"sv);
		EXPECT_EQ(AddThousandComma("123456789012345678"sv),							"123,456,789,012,345,678"sv);
		EXPECT_EQ(AddThousandComma("1234567890123456789"sv),						"1,234,567,890,123,456,789"sv);
		EXPECT_EQ(AddThousandComma("12345678901234567890"sv),						"12,345,678,901,234,567,890"sv);
		EXPECT_EQ(AddThousandComma("   -12345678901234567.123332342738491234"sv),	"   -12,345,678,901,234,567.123332342738491234"sv);
		EXPECT_EQ(AddThousandComma("aslkdfjasld;kfgjasd;fn3232asdfasd"sv),			"aslkdfjasld;kfgjasd;fn3,232asdfasd"sv);
		EXPECT_EQ(AddThousandComma("aslkdfjasld;kfgjasd;fn323asdfasd"sv),			"aslkdfjasld;kfgjasd;fn323asdfasd"sv);
		EXPECT_EQ(AddThousandComma("aslkdfjasld;kfgjasd;fn32asdfasd"sv),			"aslkdfjasld;kfgjasd;fn32asdfasd"sv);
		EXPECT_EQ(AddThousandComma("aslkdfjasld;kfgjasd;fn3asdfasd"sv),				"aslkdfjasld;kfgjasd;fn3asdfasd"sv);
		EXPECT_EQ(AddThousandComma("aslkdfjasld;kfgjasd;fn3232"sv),					"aslkdfjasld;kfgjasd;fn3,232"sv);
		EXPECT_EQ(AddThousandComma("aslkdfjasld;kfgjasd;fn323"sv),					"aslkdfjasld;kfgjasd;fn323"sv);
		EXPECT_EQ(AddThousandComma("aslkdfjasld;kfgjasd;fn32"sv),					"aslkdfjasld;kfgjasd;fn32"sv);
		EXPECT_EQ(AddThousandComma("aslkdfjasld;kfgjasd;fn3"sv),					"aslkdfjasld;kfgjasd;fn3"sv);
		EXPECT_EQ(AddThousandComma("3232asdfasd"sv),								"3,232asdfasd"sv);
		EXPECT_EQ(AddThousandComma("323asdfasd"sv),									"323asdfasd"sv);
		EXPECT_EQ(AddThousandComma("32asdfasd"sv),									"32asdfasd"sv);
		EXPECT_EQ(AddThousandComma("3asdfasd"sv),									"3asdfasd"sv);

	}

}

