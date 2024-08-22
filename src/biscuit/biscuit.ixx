module;

export module biscuit;

import "biscuit/config.h";
import "dll.h";
import std.compat;
export import :aliases;
export import :concepts;
export import :iconv_wrapper;
export import :arithmetic;
export import :color;
export import :views;
export import :misc;
//export import :coord;

export namespace biscuit {
	BISCUIT_EXPORT_DLL bool Print() {
		std::println("hello, biscuit!\n");
		return true;
	}

}
