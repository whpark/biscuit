#include <catch.hpp>

int main(int argc, char* argv[]) {
	Catch::Session session;
	session.configData().skipBenchmarks = true;
	return session.run(argc, argv);
}
