module;

export module biscuit;

import "biscuit/config.h";
import "dll.h";
import std;
export import biscuit.aliases;
export import biscuit.concepts;
export import biscuit.arithmetic;
export import biscuit.color;
export import biscuit.views;
export import biscuit.misc;
export import biscuit.codepage;
export import biscuit.iconv_wrapper;
export import biscuit.charset_kssm;
export import biscuit.convert_codepage;
export import biscuit.string;
//export import :coord;

export namespace biscuit {
	BISCUIT_EXPORT_DLL auto Print() -> bool {
		std::println("hello, biscuit!\n");
		return true;
	}

}
