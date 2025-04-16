#include <catch.hpp>

import std;
import "biscuit/dependencies_fmt.hxx";
import biscuit;

using namespace std::literals;

constexpr static auto const ATTR = "[string]";

namespace {

	template < typename T >
	struct A {
		std::wstring str;
		A() : str (T().value) {}
	};

	TEST_CASE("string misc", ATTR) {

		using namespace biscuit;

		TStringLiteral<char, "abcd"> s;
		TStringLiteral<char, "abcd"> s2;
		TStringLiteral<char16_t, "abcd"> su;

		A<TStringLiteral<wchar_t, "abcd">> a;

		REQUIRE("abcd"sv == s.value);
		//REQUIRE(&s.value == &s2.value);
		REQUIRE(a.str == L"abcd");
	}
}

TEST_CASE("Format", ATTR) {
	REQUIRE(biscuit::FormatToTString<wchar_t, "abc{}def{}ghi{}">(1, 2, 3) == L"abc1def2ghi3");

}

TEST_CASE("string tsz tpsz", ATTR) {
	using namespace biscuit;
	char legacy_buffer[1024] = "abcdef";
	char* legacy_ptr = legacy_buffer;
	wchar_t small_buffer[4] = L"123";
	auto ptr_buffer = "abcdef";
	char str_non_zero_terminated[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
	wchar_t buf[32] = L"";

	SECTION("tszlen") {
		REQUIRE(tszlen(legacy_buffer) == 6);
	#pragma warning(suppress: 4996)
		REQUIRE(tpszlen(ptr_buffer) == 6);	// DEPRECATED WARNING.
		REQUIRE(tszlen("abcdef") == 6);
		REQUIRE(tszlen("abcdef"sv) == 6);
		REQUIRE(tszlen(str_non_zero_terminated) == 6);
	}

	SECTION("tszcmp") {
		REQUIRE(tszcmp(legacy_buffer, "abcdef") == 0);
		REQUIRE(tszcmp(legacy_buffer, "abcde") > 0);
		REQUIRE(tszcmp(legacy_buffer, "abcdefg") < 0);
		REQUIRE(tszcmp(legacy_buffer, "abcdef"sv) == 0);
		REQUIRE(tszcmp(legacy_buffer, "abcde"sv) > 0);
		REQUIRE(tszcmp(legacy_buffer, "abcdefg"sv) < 0);
		REQUIRE(tszcmp(legacy_buffer, "abcdef"s) == 0);
		REQUIRE(tszcmp(legacy_buffer, "abcde"s) > 0);
		REQUIRE(tszcmp(legacy_buffer, "abcdefg"s) < 0);

	#pragma warning(push)
	#pragma warning(disable: 4996)
		REQUIRE(tpszcmp(legacy_ptr, "abcdef") == 0);
		REQUIRE(tpszcmp(legacy_ptr, "abcde") > 0);
		REQUIRE(tpszcmp(legacy_ptr, "abcdefg") < 0);
		REQUIRE(tpszcmp(legacy_ptr, "abcdef"sv) == 0);
		REQUIRE(tpszcmp(legacy_ptr, "abcde"sv) > 0);
		REQUIRE(tpszcmp(legacy_ptr, "abcdefg"sv) < 0);
		REQUIRE(tpszcmp(legacy_ptr, "abcdef"s) == 0);
		REQUIRE(tpszcmp(legacy_ptr, "abcde"s) > 0);
		REQUIRE(tpszcmp(legacy_ptr, "abcdefg"s) < 0);
	#pragma warning(pop)

		REQUIRE(tszicmp(legacy_buffer, "ABCDEF") == 0);
		REQUIRE(tszicmp(legacy_buffer, "ABCDE") > 0);
		REQUIRE(tszicmp(legacy_buffer, "ABCDEFG") < 0);
		REQUIRE(tszicmp(legacy_buffer, "ABCDEF"sv) == 0);
		REQUIRE(tszicmp(legacy_buffer, "ABCDE"sv) > 0);
		REQUIRE(tszicmp(legacy_buffer, "ABCDEFG"sv) < 0);
		REQUIRE(tszicmp(legacy_buffer, "ABCDEF"s) == 0);
		REQUIRE(tszicmp(legacy_buffer, "ABCDE"s) > 0);
		REQUIRE(tszicmp(legacy_buffer, "ABCDEFG"s) < 0);

		REQUIRE(tszncmp(legacy_buffer, "abcdef", 0) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef", 1) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef", 2) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef", 3) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef", 4) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef", 5) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef", 6) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef", 7) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef", 8) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 0) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 1) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 2) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 3) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 4) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 5) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 6) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 7) < 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdefg", 8) < 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 0) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 1) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 2) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 3) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 4) > 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 5) > 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 6) > 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 7) > 0);
		REQUIRE(tszncmp(legacy_buffer, "abc", 8) > 0);

		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 0) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 1) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 2) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 3) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 4) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 5) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 6) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 7) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abcdef"sv, 8) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 0) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 1) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 2) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 3) == 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 4) > 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 5) > 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 6) > 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 7) > 0);
		REQUIRE(tszncmp(legacy_buffer, "abc"sv, 8) > 0);

		using typeA = wchar_t;
		using typeB = char32_t;
		if constexpr (sizeof(wchar_t) == sizeof(char16_t)) {
			static_assert(sizeof(wchar_t) == 2);
			static_assert(sizeof(char32_t) == 4);
			REQUIRE(sizeof(wchar_t)== 2);

			REQUIRE(tszcmp(L"가나다", u"가나다") == 0);
		}
		//else {
		//	static_assert(sizeof(wchar_t) == 4);
		//	static_assert(sizeof(char32_t) == 4);
		//	REQUIRE(sizeof(wchar_t) == 4);
		//	REQUIRE(tszcmp(L"가나다", U"가나다") == 0);
		//}
	}

	SECTION("tszcpy") {
		wchar_t* ptr = nullptr;
		wchar_t small_buffer2[4] {};
	#pragma warning(suppress: 4996)
		REQUIRE(tpszcpy(ptr, 0, ptr) == std::unexpected(std::make_error_code(std::errc::invalid_argument)));
	#pragma warning(suppress: 4996)
		REQUIRE(tpszcpy(ptr, 1, L"abcdef") == std::unexpected(std::make_error_code(std::errc::invalid_argument)));
		//REQUIRE(tszcpy(ptr, "abcdef") == std::errc::invalid_argument);	// NOT Compiling.
		std::vector<wchar_t> vec(1024, 0);
		std::array<wchar_t, 1024> arr;
	#pragma warning(suppress: 4996)
		REQUIRE(tszcpy(arr, L"abcdef"));

		REQUIRE(tszcpy(small_buffer, L"abcdef") == std::unexpected(std::make_error_code(std::errc::result_out_of_range)));
	//#pragma warning(suppress: 4996)
		REQUIRE(tszcmp(small_buffer, L"abc") == 0);
		REQUIRE(tszcmp(small_buffer, L"abc"s) == 0);
		REQUIRE(tszcmp(small_buffer, L"abc"sv) == 0);
		REQUIRE(tszcmp(u"def", L"def") == 0);
		REQUIRE(tszcpy(small_buffer, L"def"s));
		REQUIRE(tszcmp(small_buffer, L"def") == 0);
		REQUIRE(tszcpy(small_buffer, L"1234"s) == std::unexpected(std::make_error_code(std::errc::result_out_of_range)));
		REQUIRE(tszcpy(small_buffer, L"567"sv));
		REQUIRE(tszcpy(small_buffer, L"890A"sv) == std::unexpected(std::make_error_code(std::errc::result_out_of_range)));
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
	}

	SECTION("tszcat") {
		tszcpy(buf, L"123");
		tszcat(buf, L"abc");

		for (auto& c : small_buffer)
			c = 'F';
		REQUIRE(tszcat(small_buffer, L"abc") == std::unexpected(std::make_error_code(std::errc::invalid_argument)));
		REQUIRE(tszcmp(small_buffer, L"FFFF") == 0);
	}



	SECTION("tszrmchar") {
		tszcpy(small_buffer, L"114");
		REQUIRE(tszrmchar(small_buffer, L'0') == 3);
		REQUIRE(tszrmchar(small_buffer, L'4') == 2);
		REQUIRE(tszcmp(small_buffer, L"11") == 0);

		tszcpy(small_buffer, L"144");
		REQUIRE(tszrmchar(small_buffer, L'4') == 1);
		REQUIRE(tszcmp(small_buffer, L"1") == 0);

		tszcpy(small_buffer, L"423");
		REQUIRE(tszrmchar(small_buffer, L'4') == 2);
		REQUIRE(tszcmp(small_buffer, L"23") == 0);
		
		REQUIRE(tszrmchar(str_non_zero_terminated, '?') == 6);
		REQUIRE(tszrmchar(str_non_zero_terminated, 'f') == 5);
		REQUIRE(tszcmp(str_non_zero_terminated, "abcde") == 0);
	}


	SECTION("tszrev") {
		tszrev(small_buffer);
		REQUIRE(tszcmp(small_buffer, L"321") == 0);
	}


}
