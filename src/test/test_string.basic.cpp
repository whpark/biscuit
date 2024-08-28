#include <catch.hpp>
#include <fmt/core.h>
#include <fmt/std.h>
#include <fmt/xchar.h>

import std;
import biscuit;

using namespace std::literals;

TEST_CASE("string.basic") {
	using namespace biscuit;
	char legacy_buffer[1024] = "abcdef";
	auto ptr_buffer = "abcdef";
	char str_non_zero_terminated[] = { 'a', 'b', 'c', 'd', 'e', 'f' };

	SECTION("tszlen") {
		REQUIRE(tszlen(legacy_buffer) == 6);
	#pragma warning(suppress: 4996)
		REQUIRE(tpszlen(ptr_buffer) == 6);	// DEPRECATED WARNING.
		REQUIRE(tszlen("abcdef") == 6);
		REQUIRE(tszlen("abcdef"sv) == 6);
		REQUIRE(tszlen(str_non_zero_terminated) == 6);
	}

	SECTION("tszcpy") {
		wchar_t* ptr = nullptr;
		wchar_t small_buffer[4];
		wchar_t small_buffer2[4] {};
	#pragma warning(suppress: 4996)
		REQUIRE(tpszcpy(ptr, 0, ptr) == std::unexpected(EINVAL));
	#pragma warning(suppress: 4996)
		REQUIRE(tpszcpy(ptr, 1, L"abcdef") == std::unexpected(EINVAL));
		//REQUIRE(tszcpy(ptr, "abcdef") == EINVAL);	// NOT Compiling.
		std::vector<wchar_t> buf(1024, 0);
		std::array<wchar_t, 1024> arr;
	#pragma warning(suppress: 4996)
		REQUIRE(tszcpy(arr, L"abcdef"));

		REQUIRE(tszcpy(small_buffer, L"abcdef") == std::unexpected(ERANGE));
	//#pragma warning(suppress: 4996)
		REQUIRE(tszcmp(small_buffer, L"abc") == 0);
		REQUIRE(tszcmp(small_buffer, L"abc"s) == 0);
		REQUIRE(tszcmp(small_buffer, L"abc"sv) == 0);
		REQUIRE(tszcmp(u"def", L"def") == 0);
		REQUIRE(tszcpy(small_buffer, L"def"s));
		REQUIRE(tszcmp(small_buffer, L"def") == 0);
		REQUIRE(tszcpy(small_buffer, L"1234"s) == std::unexpected(ERANGE));
		REQUIRE(tszcpy(small_buffer, L"567"sv));
		REQUIRE(tszcpy(small_buffer, L"890A"sv) == std::unexpected(ERANGE));
		REQUIRE(tszcpy(small_buffer, L"11"sv));

		// both nullptr or empty
	#pragma warning(suppress: 4996)
		REQUIRE(tpszcmp(ptr, ptr) == 0);

	#pragma warning(suppress: 4996)
		REQUIRE(tpszcmp(ptr, L"") == 0);
	#pragma warning(suppress: 4996)
		REQUIRE(tpszcmp(L"", ptr) == 0);

		REQUIRE(tszcmp(L""s, std::wstring_view(ptr, 0)) == 0);
		REQUIRE(tszcmp(std::wstring_view(ptr, 0), L""s) == 0);

		REQUIRE(tszcmp(L""s, L"") == 0);
		REQUIRE(tszcmp(L"", L""s) == 0);

		REQUIRE(tszcmp(L"1", L"2") < 0);
		REQUIRE(tszcmp(L"1", L"0") > 0);
		REQUIRE(tszcmp(L"1", L"1") == 0);
		REQUIRE(tszcmp(L"123", L"1230") < 0);
		REQUIRE(tszcmp(L"1230", L"123") > 0);

		REQUIRE(tszcmp(small_buffer, small_buffer2) != 0);

		tszcat(small_buffer, L"4567");

		// todo : tpszncmp, tpszicmp, tpsznicmp, tszncmp, tszicmp, tsznicmp,  
		// tszcat...kkk
		
	}
}
