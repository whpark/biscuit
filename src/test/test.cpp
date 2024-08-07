#include <catch.hpp>
//#include "biscuit.h"

import biscuit;

#pragma comment(lib, "biscuit.lib")

TEST_CASE("test", "[test]") {

	REQUIRE(biscuit::Print());

}
