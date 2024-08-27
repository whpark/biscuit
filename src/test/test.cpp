#include <catch.hpp>
#include <fmt/core.h>
#include "biscuit/biscuit.h"

import std;
import biscuit;

using namespace std::literals;

TEST_CASE("test") {
	//REQUIRE(biscuit::Print());

	auto str0 = " \t\r\n  abc \t "s;
	auto str = str0;	
	str = str0; biscuit::Trim(str); REQUIRE(str == "abc"s);
	str = str0; biscuit::TrimLeft(str); REQUIRE(str == "abc \t "s);
	biscuit::TrimRight(str, " "sv); REQUIRE(str == "abc \t"s);
	biscuit::TrimRight(str, "\t"sv); REQUIRE(str == "abc "s);

}
