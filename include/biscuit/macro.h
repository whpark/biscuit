﻿#pragma once

#include <version>

#ifdef __cpp_lib_source_location
#	include <source_location>
#	define BSC__FUNCSIG std::string(std::source_location::current().function_name()) + " : "
#else
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

#ifndef BSC__NODISCARD
#	define BSC__NODISCARD [[nodiscard]]
#endif

#ifndef BSC__DEPR_SEC
#	define BSC__DEPR_SEC [[deprecated("NOT Secure Function. specify - __pargma warning(disable:4996)")]]
#endif

//=================================================================================================================================
// MACRO FOR_EACH
// https://www.scs.stanford.edu/~dm/blog/va-opt.html

#define I_BSC__PARENS ()

#define I_BSC__EXPAND(...)								I_BSC__EXPAND4(I_BSC__EXPAND4(I_BSC__EXPAND4(I_BSC__EXPAND4(__VA_ARGS__))))
#define I_BSC__EXPAND4(...)								I_BSC__EXPAND3(I_BSC__EXPAND3(I_BSC__EXPAND3(I_BSC__EXPAND3(__VA_ARGS__))))
#define I_BSC__EXPAND3(...)								I_BSC__EXPAND2(I_BSC__EXPAND2(I_BSC__EXPAND2(I_BSC__EXPAND2(__VA_ARGS__))))
#define I_BSC__EXPAND2(...)								I_BSC__EXPAND1(I_BSC__EXPAND1(I_BSC__EXPAND1(I_BSC__EXPAND1(__VA_ARGS__))))
#define I_BSC__EXPAND1(...)								__VA_ARGS__

#define BSC__FOR_EACH_COMMA(macro, ...)					__VA_OPT__(I_BSC__EXPAND(I_BSC__FOR_EACH_HELPER_COMMA(macro, __VA_ARGS__)))
#define BSC__FOR_EACH_SPACE(macro, ...)					__VA_OPT__(I_BSC__EXPAND(I_BSC__FOR_EACH_HELPER_SPACE(macro, __VA_ARGS__)))
#define I_BSC__FOR_EACH_HELPER_COMMA(macro, a1, ...)	macro(a1) __VA_OPT__(,) __VA_OPT__(I_BSC__FOR_EACH_AGAIN_COMMA I_BSC__PARENS (macro, __VA_ARGS__))
#define I_BSC__FOR_EACH_HELPER_SPACE(macro, a1, ...)	macro(a1)               __VA_OPT__(I_BSC__FOR_EACH_AGAIN_SPACE I_BSC__PARENS (macro, __VA_ARGS__))
#define I_BSC__FOR_EACH_AGAIN_COMMA()					I_BSC__FOR_EACH_HELPER_COMMA
#define I_BSC__FOR_EACH_AGAIN_SPACE()					I_BSC__FOR_EACH_HELPER_SPACE

// NUM_ARGS(...) macro arg 갯수
#define NUM_ARGS_64(                            _64,_63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,_50,_49,_48,_47,_46,_45,_44,_43,_42,_41,_40,_39,_38,_37,_36,_35,_34,_33,_32,_31,_30,_29,_28,_27,_26,_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1, N, ...) N
#define NUM_ARGS(...) NUM_ARGS_64( __VA_ARGS__ , 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
static_assert(NUM_ARGS(1, 2, 3) == 3, "for MSVC, add compiler option /Zc:preprocessor");

