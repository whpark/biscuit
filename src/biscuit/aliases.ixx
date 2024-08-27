module;

#include "biscuit/config.h"

export module biscuit.aliases;
import std;

// export aliases;
export namespace biscuit::inline literals { };

// export aliases;
export namespace biscuit::inline literals { };

// aliases for std::intXX_t
export namespace biscuit::inline aliases {
	using int8  = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;
	using uint8  = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;

	using uchar = unsigned char;
	using uint = uint32_t;	// conflicts with cv::
	using charKSSM_t = uint16_t;
}

// Temporary workaround for C++23's std::ssize_t
#ifndef __cpp_size_t_suffix
#	pragma warning(disable: 4455)
export namespace biscuit::inline aliases {
	using ssize_t = std::ptrdiff_t;

	constexpr size_t const SIZE_MAX = std::numeric_limits<size_t>::max();
	constexpr size_t const RSIZE_MAX = SIZE_MAX >> 1;
}
export namespace biscuit::inline literals {
	constexpr biscuit::ssize_t operator "" z ( unsigned long long n ) { return n; }
	constexpr std::size_t operator "" uz ( unsigned long long n ) { return n; }
}
#endif
