#include <fmt/core.h>
#include <fmt/std.h>
#include <catch.hpp>

import std;
import biscuit;

int main(int , char* []) {
	biscuit::SetCurrentPath_BinFolder();
	auto path = std::filesystem::current_path();
	fmt::print("Current path: {}\n", path);

	Catch::Session session;
	//session.configData().skipBenchmarks = true;
	return session.run();
}

