﻿#include <catch.hpp>
#include "biscuit/biscuit.h"
#include "biscuit/glaze_helper.h"

import std;
import "biscuit/dependencies_fmt.hxx";
import biscuit;

using namespace std::literals;

constexpr static auto const ATTR = "[misc]";

namespace test_misc {

	TEST_CASE("rand", ATTR) {
		biscuit::xRandUniI rand(10, 20);
		[[ maybe_unused ]] auto r = rand();
		REQUIRE(r >= 10);
		REQUIRE(r <= 20);
	}

	// todo: smart_ptr_container, stop_watch

}	// namespace

