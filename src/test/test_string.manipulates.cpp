#include <catch.hpp>

import std;
import "biscuit/dependencies/fmt.hxx";
import biscuit;

using namespace std::literals;

constexpr static auto const ATTR = "[string]";

TEST_CASE("string.manipulates", ATTR) {

	using namespace biscuit;

	SECTION("trim") {
		auto str0 = " \t\r\n  abc \t "s;
		auto str = str0;	
		str = str0; Trim(str); REQUIRE(str == "abc"s);
		str = str0; TrimLeft(str); REQUIRE(str == "abc \t "s);
		TrimRight(str, 'c'); REQUIRE(str == "abc \t "s);
		TrimRight(str, ' '); REQUIRE(str == "abc \t"s);
		TrimRight(str, "\t"); REQUIRE(str == "abc "s);
		TrimRight(str, "abc "); REQUIRE(str == ""s);

		str = str0; REQUIRE(TrimView<char>(str) == "abc"sv);
		str = str0; REQUIRE(TrimLeftView<char>(str) == "abc \t "sv);
		str = str0; REQUIRE(TrimRightView<char>(str) == " \t\r\n  abc"sv);
		str = str0; REQUIRE(TrimView<char>(str, ' ') == "\t\r\n  abc \t"sv);
	}

	SECTION("split") {
		auto str = "abc;1234;---;;;"s;
		auto v = Split(str, ';');
		REQUIRE(v.size() == 6);
		REQUIRE(v[0] == "abc");
		REQUIRE(v[1] == "1234");
		REQUIRE(v[2] == "---");
		REQUIRE(v[3] == "");
		REQUIRE(v[4] == "");
		REQUIRE(v[5] == "");

		REQUIRE(Split("", 'a').size() == 1);
		str = "     ";
		v = Split(str, ' ');
		REQUIRE(v.size() == 6);
		for (auto const& i : v) {
			REQUIRE(i == "");
		}

		auto view = SplitView("a,b,c,d", ',');
		REQUIRE(view.size() == 4);
		REQUIRE(view[0] == "a"sv);
		REQUIRE(view[1] == "b"sv);
		REQUIRE(view[2] == "c"sv);
		REQUIRE(view[3] == "d"sv);

		wchar_t const buf[] = L";123;abc;defghijk\0asdf;asd;fa;sdf";
		auto wview = SplitView(buf, L';');
		REQUIRE(wview.size() == 4);
		REQUIRE(wview[0] == L""sv);
		REQUIRE(wview[1] == L"123"sv);
		REQUIRE(wview[2] == L"abc"sv);
		REQUIRE(wview[3] == L"defghijk"sv);

	}

}
