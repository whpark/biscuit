#include <catch.hpp>
#include "biscuit/biscuit.h"
#include "biscuit/glaze_helper.h"

import std;
import "biscuit/dependencies/fmt.hxx";
import biscuit;

using namespace std::literals;

constexpr static auto const ATTR = "[misc]";

namespace test_misc {

	TEST_CASE("rand", ATTR) {
		double from = 0.0;
		double to = std::nextafter(1.0, 2.0);
		auto t0 = std::chrono::steady_clock::now();
		biscuit::xRandUniI rand(from, to);
		int count = 10000;
		for (int i = 0; i < count; i++) {
			auto r = rand();
			REQUIRE(r >= from);
			REQUIRE(r < to);
		}
		auto t1 = std::chrono::steady_clock::now();
		auto ts = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
		fmt::println("rand: {:L}ns / {} items, {} / 1", ts.count(), count, ts / count);
	}

	// todo: smart_ptr_container, stop_watch

}	// namespace
