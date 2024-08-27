#pragma once

import <version>;

#ifdef __cpp_lib_source_location
#	include <source_location>
#	define BSC__FUNCSIG std::string(std::source_location::current().function_name()) + " : "
#else
// TODO: Reference additional headers your program requires here.
// until c++20 source_location
#	if defined (_MSC_VER)
#		define BSC__FUNCSIG __FUNCSIG__ " : "
#	elif defined(__GNUC__)
#		define BSC__FUNCSIG __PRETTY_FUNCTION__ " : "
#	endif
#endif


#ifndef NOMINMAX
#	define NOMINMAX	// disable Windows::min/max
#endif
#if defined(min) || defined(max)
//#	error min/max must not be defined. turn it off using '#define NOMINMAX' before '#include <Windows.h>''
#	undef min
#	undef max
#endif

//#ifndef BSC__NODISCARD
#define BSC__NODISCARD [[nodiscard]]
//#endif

#define BSC__DEPR_SEC [[deprecated("NOT Secure. use 'sv' instead.")]]