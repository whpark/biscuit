#include <catch.hpp>
#include "biscuit/biscuit.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_ctre.h"

import std;
import fmt;
import biscuit;

using namespace std::literals;
namespace concepts = biscuit::concepts;

constexpr static auto const ATTR = "[lazy_profile]";

namespace test_lazy_profile {

	using namespace biscuit;

	TEST_CASE("lazy_profile", ATTR) {

		auto sv3 = xStringLiteral{"asdfsdff"};
		//auto sv = TStringLiteral<wchar_t, "asdf">().value;
		static auto sv4 = TStringLiteral<wchar_t, "{}">{};
		auto wstr = fmt::format(L"{}", L"abcd");
		REQUIRE(wstr == L"abcd"s);
		auto str = fmt::format("{}", "abcdef");
		REQUIRE(str == "abcdef"s);
		auto item = L"  asdf = 1234  ; asdfasdfasdfasdfasdfasdfasdf";
		auto [all, key, value, comment] = ctre::match<R"xxx(\s*(.*[^\s]+)\s*=\s*([^;\n]*|"[^\n]*")\s*((?:;|#).*)?)xxx">(item);
		//bool ok = (bool)all;
		std::wstring_view sv2 = key;

		std::string strLazyProfile =
			R"xxx(; comment 1
; comment 2
# comment 3

[Test]
abcdef                   = 1                            ; Comments "" 3290 [] asdjfklasd "" \" 23r2390
path                     = 1                            ; Comments "" 3290 [] asdjfklasd "" \" 23r2390
LogTranslatorEvents      = TRUE                         ; COMMENTS asdfas djfkl;asd jfkla;s dfjkla;sdf
;String                   = "\r\n skld\" fjaskdld;f"  garbage here ; comment3


[ key having punctuation()- and spaces ]
; =============
; comments
; =============
ABC                      = 0.1234e4                     ; 1234
   BooleanVar            = TRUE                         ; 
   BooleanVar2           = false                        ; 
   BooleanVar3           = true                         ; 
; another comments
doubleVar                = 3.1415926535897932384626433832795028 ; pi

   path                  = "C:\ABC\DEF\가나다라.txt"       ; path

[ another values ]
    list                 = 1, 2, 3, 4, 5, 6, 7, 8, 9     ; treat as string

)xxx";
		{
			TLazyProfile<char8_t> profile;
			auto r = std::filesystem::current_path();
			std::istringstream stream(strLazyProfile);
			auto t0 = std::chrono::steady_clock::now();
			profile.Load(stream);
			auto t1 = std::chrono::steady_clock::now();
			auto v = profile[u8"Test"](u8"LogTranslatorEvents", false);
			REQUIRE(v == true);
			profile[u8"Test"].SetItemValue(u8"path", 2);
			profile[u8"Test"].SetItemValue(u8"LogTranslatorEvents", false);
			//profile[u8"Test"].SetItemValue(u8"String", u8"\" 바바바바바");	// error.
			profile[u8"Test"].SetItemValue(u8"newValue", 3.1415, u8"comment ___ new");

			profile[u8"key having punctuation()- and spaces"].SetItemValue(u8"path", u8"C:\\ABC\\DEF\\가나다라.txt");

			auto t2 = std::chrono::steady_clock::now();

			profile.Save("ini/lazy_profileU8.ini");

			auto t3 = std::chrono::steady_clock::now();

			auto GetDuration = [](auto t0, auto t1) {
				return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
				};
			fmt::print("Load profile : {}\n", GetDuration(t0, t1));
			fmt::print("Setting 4 items to profile : {}\n", GetDuration(t1, t2));
			fmt::print("Save profile : {}\n", GetDuration(t2, t3));
		}

		{
			TLazyProfile<char> profile;
			auto r = std::filesystem::current_path();
			std::istringstream stream(strLazyProfile);
			auto t0 = std::chrono::steady_clock::now();
			profile.Load(stream);
			auto t1 = std::chrono::steady_clock::now();
			profile["Test"]("path") = 2;
			profile["Test"]("LogTranslatorEvents") = false;
			//profile[u8"Test"].SetItemValue(u8"String", u8"\" 바바바바바");	// error.
			profile["Test"].SetItemValue("newValue", 3.1415, "comment ___ new");

			profile["key having punctuation()- and spaces"]("path") = (char const*)u8"C:\\ABC\\DEF\\가나다라.txt";

			auto t2 = std::chrono::steady_clock::now();

			profile.Save("ini/lazy_profile.ini");

			auto t3 = std::chrono::steady_clock::now();

			auto GetDuration = [](auto t0, auto t1) {
				return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
				};
			fmt::print("Load profile : {}\n", GetDuration(t0, t1));
			fmt::print("Setting 4 items to profile : {}\n", GetDuration(t1, t2));
			fmt::print("Save profile : {}\n", GetDuration(t2, t3));
		}

		{
			TLazyProfile<wchar_t> profile;
			auto r = std::filesystem::current_path();
			std::istringstream stream(strLazyProfile);
			profile.Load(stream);

			profile.Save("ini/lazy_profileW.ini");
		}

	}

}	// namespace test_archive

