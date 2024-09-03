#include <catch.hpp>
#include <fmt/core.h>
#include "biscuit/biscuit.h"

import std;
import biscuit;

using namespace std::literals;

TEST_CASE("coord") {
	//using namespace biscuit;

	biscuit::sRect2i rect2i{0, 0, 1, 2};
	biscuit::sPoint2i pt2i{100, 200};
	biscuit::sSize2i size2i;
	size2i = pt2i;

	REQUIRE(size2i.width == 100);
	REQUIRE(size2i.height == 200);
}

