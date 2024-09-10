#include <catch.hpp>

import std;
import biscuit;

int main(int , char* []) {
	biscuit::SetCurrentPath_BinFolder();
	auto path = std::filesystem::current_path();
	std::print("Current path: {}\n", path.string());

	Catch::Session session;
	//session.configData().skipBenchmarks = true;
	return session.run();
}

