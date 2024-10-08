#pragma once

#include <version>
#include "biscuit/config.h"

import std.compat;

// export aliases;
namespace biscuit::inline literals { };

// aliases for std::intXX_t
namespace biscuit::inline aliases {
	using int8  = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;
	using uint8  = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;

	using uchar = unsigned char;
	using uint = uint32_t;	// conflicts with cv::
	using charKSSM_t = uint16_t;
}


// Temporary workaround for C++23's std::ssize_t
#ifndef __cpp_size_t_suffix
#	pragma warning(disable: 4455)
namespace biscuit::inline aliases {
	export using ssize_t = ptrdiff_t;
}
namespace biscuit::inline literals {
	export constexpr biscuit::ssize_t operator "" z ( unsigned long long n ) { return n; }
	export constexpr std::size_t operator "" uz ( unsigned long long n ) { return n; }
}
#endif
