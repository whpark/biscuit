#include <fmt/core.h>
#include <fmt/std.h>
#include <catch.hpp>

import std;
import biscuit;

int main(int argc, char* argv[]) {
	biscuit::SetCurrentPath_BinFolder();
	auto path = std::filesystem::current_path();
	fmt::print("Current path: {}\n", path);

	return Catch::Session().run(argc, argv);
}
