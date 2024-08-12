#include <fmt/core.h>
#include <fmt/std.h>
#include <catch.hpp>

import std;

int main(int argc, char* argv[]) {
	auto path = std::filesystem::current_path();
	fmt::print("Current path: {}\n", path);

	return Catch::Session().run(argc, argv);
}
