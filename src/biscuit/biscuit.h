#pragma once

#include <version>
#include <cstdint>
#if defined(__cpp_lib_source_location)
#	include <source_location>
#else
// TODO: Reference additional headers your program requires here.
// until c++20 source_location
#	ifdef __cpp_lib_source_location
#		define BSC__FUNCSIG std::string(std::source_location::current().function_name()) + " : "
#	else
#		if defined (_MSC_VER)
#			define BSC__FUNCSIG __FUNCSIG__ " : "
#		elif defined(__GNUC__)
#			define BSC__FUNCSIG __PRETTY_FUNCTION__ " : "
#		endif
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


// NUM_ARGS(...) macro arg 갯수
#define NUM_ARGS_64(                            _64,_63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,_50,_49,_48,_47,_46,_45,_44,_43,_42,_41,_40,_39,_38,_37,_36,_35,_34,_33,_32,_31,_30,_29,_28,_27,_26,_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1, N, ...) N
#define NUM_ARGS(...) NUM_ARGS_64( __VA_ARGS__ , 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
static_assert(NUM_ARGS(1, 2, 3) == 3, "for MSVC, add compiler option /Zc:preprocessor");

#define I_BSC__RECURSIVE_MACRO(macro, var)								macro(var)
#define I_BSC__RECURSIVE_MACRO_EX_COMMA(macro, var, expr)				macro(var), expr
#define I_BSC__RECURSIVE_MACRO_EX_SPACE(macro, var, expr)				macro(var)  expr
#define I_BSC__RECURSIVE_MACRO_1(separator, macro, var)					macro(var)
#define I_BSC__RECURSIVE_MACRO_2(separator, macro, var, var2)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_1(separator, macro, var2))
#define I_BSC__RECURSIVE_MACRO_3(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_2(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_4(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_3(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_5(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_4(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_6(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_5(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_7(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_6(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_8(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_7(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_9(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_8(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_10(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_9(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_11(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_10(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_12(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_11(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_13(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_12(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_14(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_13(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_15(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_14(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_16(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_15(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_17(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_16(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_18(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_17(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_19(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_18(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_20(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_19(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_21(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_20(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_22(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_21(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_23(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_22(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_24(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_23(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_25(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_24(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_26(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_25(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_27(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_26(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_28(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_27(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_29(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_28(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_30(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_29(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_31(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_30(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_32(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_31(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_33(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_32(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_34(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_33(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_35(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_34(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_36(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_35(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_37(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_36(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_38(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_37(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_39(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_38(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_40(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_39(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_41(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_40(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_42(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_41(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_43(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_42(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_44(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_43(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_45(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_44(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_46(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_45(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_47(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_46(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_48(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_47(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_49(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_48(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_50(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_49(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_51(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_50(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_52(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_51(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_53(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_52(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_54(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_53(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_55(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_54(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_56(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_55(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_57(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_56(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_58(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_57(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_59(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_58(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_60(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_59(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_61(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_60(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_62(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_61(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_63(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_62(separator, macro, __VA_ARGS__))
#define I_BSC__RECURSIVE_MACRO_64(separator, macro, var, ...)			I_BSC__RECURSIVE_MACRO_EX_##separator(macro, var, I_BSC__RECURSIVE_MACRO_63(separator, macro, __VA_ARGS__))

#define I_BSC__RECURSIVE_MACRO_C(separator, macro, N, ...)				I_BSC__RECURSIVE_MACRO_##N(separator, macro, __VA_ARGS__)
#define I_BSC__RECURSIVE_MACRO_F(separator, macro, N, ...)				I_BSC__RECURSIVE_MACRO_C(separator, macro, N, __VA_ARGS__)
#define BSC__RECURSIVE_MACRO_COMMA(macro, ...)							I_BSC__RECURSIVE_MACRO_F(COMMA, macro, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)
#define BSC__RECURSIVE_MACRO_SPACE(macro, ...)							I_BSC__RECURSIVE_MACRO_F(SPACE, macro, NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

namespace biscuit {

	inline namespace literals {
	};

	// aliases
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

#if (BSC__STRING_SUPPORT_CODEPAGE_KSSM)
	using charKSSM_t = uint16_t;
#endif

}
