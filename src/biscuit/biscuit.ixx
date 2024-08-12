module;

#include "biscuit/config.h"
#include "dll.h"

export module biscuit;
export import :concepts;
export import :aliases;
export import :views;
import std.compat;


export namespace biscuit {
	BISCUIT_EXPORT_DLL bool Print() {
		std::println("hello, biscuit!\n");
		return true;
	}

}
