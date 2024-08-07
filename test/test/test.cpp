#include <catch.hpp>
//#include "biscuit.h"

import biscuit;

#pragma comment(lib, "biscuit.x64D.lib")

TEST_CASE("test", "[test]") {

	REQUIRE(biscuit::Print());

}
