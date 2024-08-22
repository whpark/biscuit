#include <catch.hpp>
#include <fmt/core.h>
#include "biscuit/biscuit.h"

import std.compat;
import biscuit;

TEST_CASE("test") {
	REQUIRE(biscuit::Print());
}
