#include <catch.hpp>

import std;
import biscuit;

int main(int , char* []) {
	biscuit::SetCurrentPath_ProjectFolder();
	auto path = std::filesystem::current_path();
	std::print("Current path: {}\n", path.string());

	Catch::Session session;
	//session.configData().skipBenchmarks = true;
	session.configData().testsOrTags.push_back("~[sequence]~[sequence_map]~[sequence_tReturn]");
	return session.run();
}

