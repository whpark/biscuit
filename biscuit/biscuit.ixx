module;

#include <fmt/core.h>

export module biscuit;

export namespace biscuit::inline v01 {

	bool print() {
		fmt::print("Hello, biscuit!\n");
		return true;
	}

}
