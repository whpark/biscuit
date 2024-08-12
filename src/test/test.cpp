#include <catch.hpp>
//#include "biscuit.h"

import std.compat;
import biscuit;

//namespace bscs = biscuit;

#pragma comment(lib, "biscuit.lib")

//template <typename T>
//void Func(T a) requires biscuit::concepts::template rect<T> {
//}

TEST_CASE("test", "[test]") {
	REQUIRE(biscuit::Print());
}
