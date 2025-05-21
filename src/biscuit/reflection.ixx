module;

//////////////////////////////////////////////////////////////////////
//
// reflection.ixx:
//
// PWH
// 2025-05-21
//
//	boost pfr like functions (boost::pfr is not available in c++ module - ICE). till c++26
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"

export module biscuit.reflection;
import std;
import biscuit.aliases;
import biscuit.concepts;

using namespace std::literals;
using namespace biscuit::literals;

namespace biscuit::detail {

	// can convert to anything implicitly.
	template<std::size_t>
	struct any {
		template<class T>
		constexpr operator T() const noexcept;
	};

	// true when T can be brace-initialized with N 'any' arguments
	template<class T, std::size_t... Is>
	constexpr bool brace_constructible(std::index_sequence<Is...>) noexcept {
		return concepts::constructible<T, any<Is>...>;
	}

	template<class T, std::size_t N>
	constexpr bool brace_constructible_n =
		brace_constructible<T>(std::make_index_sequence<N>{});

	// -----------------------------------------------------------------
	// constexpr function that returns the number of *public* data members
	// (works for aggregates only)
	template<class T, std::size_t N>
	constexpr std::size_t CountStructMember() noexcept {
		if constexpr (brace_constructible_n<T, N + 1>)
			return CountStructMember<T, N + 1>();			// tail recursion, folds at compile time
		else
			return N;										// N is the first size that fails
	}

	template < class T >
	inline constexpr auto tie(T& v) {
		static constexpr size_t N = CountStructMember<T, 0>();
		if constexpr (N == 0) { return std::tuple{}; }
		else if constexpr (N == 1) {
			auto& [a1] = v;
			return std::tuple{&a1};
		}
		else if constexpr (N == 2) {
			auto& [a1, a2] = v;
			return std::tuple{&a1, &a2};
		}
		else if constexpr (N == 3) {
			auto& [a1, a2, a3] = v;
			return std::tuple{&a1, &a2, &a3};
		}
		else if constexpr (N == 4) {
			auto& [a1, a2, a3, a4] = v;
			return std::tuple{&a1, &a2, &a3, &a4};
		}
		else if constexpr (N == 5) {
			auto& [a1, a2, a3, a4, a5] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5};
		}
		else if constexpr (N == 6) {
			auto& [a1, a2, a3, a4, a5, a6] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6};
		}
		else if constexpr (N == 7) {
			auto& [a1, a2, a3, a4, a5, a6, a7] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7};
		}
		else if constexpr (N == 8) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8};
		}
		else if constexpr (N == 9) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9};
		}
		else if constexpr (N == 10) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10};
		}
		else if constexpr (N == 11) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11};
		}
		else if constexpr (N == 12) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12};
		}
		else if constexpr (N == 13) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13};
		}
		else if constexpr (N == 14) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14};
		}
		else if constexpr (N == 15) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15};
		}
		else if constexpr (N == 16) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16};
		}
		else if constexpr (N == 17) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17};
		}
		else if constexpr (N == 18) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18};
		}
		else if constexpr (N == 19) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19};
		}
		else if constexpr (N == 20) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20};
		}
		else if constexpr (N == 21) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21};
		}
		else if constexpr (N == 22) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22};
		}
		else if constexpr (N == 23) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23};
		}
		else if constexpr (N == 24) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24};
		}
		else if constexpr (N == 25) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25};
		}
		else if constexpr (N == 26) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26};
		}
		else if constexpr (N == 27) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27};
		}
		else if constexpr (N == 28) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28};
		}
		else if constexpr (N == 29) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29};
		}
		else if constexpr (N == 30) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30};
		}
		else if constexpr (N == 31) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31};
		}
		else if constexpr (N == 32) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32};
		}
		else if constexpr (N == 33) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33};
		}
		else if constexpr (N == 34) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34};
		}
		else if constexpr (N == 35) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35};
		}
		else if constexpr (N == 36) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36};
		}
		else if constexpr (N == 37) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37};
		}
		else if constexpr (N == 38) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38};
		}
		else if constexpr (N == 39) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39};
		}
		else if constexpr (N == 40) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40};
		}
		else if constexpr (N == 41) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41};
		}
		else if constexpr (N == 42) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42};
		}
		else if constexpr (N == 43) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43};
		}
		else if constexpr (N == 44) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44};
		}
		else if constexpr (N == 45) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45};
		}
		else if constexpr (N == 46) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46};
		}
		else if constexpr (N == 47) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47};
		}
		else if constexpr (N == 48) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48};
		}
		else if constexpr (N == 49) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49};
		}
		else if constexpr (N == 50) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50};
		}
		else if constexpr (N == 51) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51};
		}
		else if constexpr (N == 52) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52};
		}
		else if constexpr (N == 53) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53};
		}
		else if constexpr (N == 54) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54};
		}
		else if constexpr (N == 55) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55};
		}
		else if constexpr (N == 56) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56};
		}
		else if constexpr (N == 57) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57};
		}
		else if constexpr (N == 58) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58};
		}
		else if constexpr (N == 59) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59};
		}
		else if constexpr (N == 60) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60};
		}
		else if constexpr (N == 61) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61};
		}
		else if constexpr (N == 62) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62};
		}
		else if constexpr (N == 63) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63};
		}
		else if constexpr (N == 64) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64};
		}
		else if constexpr (N == 65) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65};
		}
		else if constexpr (N == 66) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66};
		}
		else if constexpr (N == 67) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67};
		}
		else if constexpr (N == 68) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68};
		}
		else if constexpr (N == 69) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69};
		}
		else if constexpr (N == 70) {
			auto& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70] = v;
			return std::tuple{&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12, &a13, &a14, &a15, &a16, &a17, &a18, &a19, &a20, &a21, &a22, &a23, &a24, &a25, &a26, &a27, &a28, &a29, &a30, &a31, &a32, &a33, &a34, &a35, &a36, &a37, &a38, &a39, &a40, &a41, &a42, &a43, &a44, &a45, &a46, &a47, &a48, &a49, &a50, &a51, &a52, &a53, &a54, &a55, &a56, &a57, &a58, &a59, &a60, &a61, &a62, &a63, &a64, &a65, &a66, &a67, &a68, &a69, &a70};
		}
		else {
			static_assert(false, "CANNOT handle more than 64 fields");
		}
	}

} // namespace detail

export namespace biscuit {

	template < typename T >
		requires (std::is_aggregate_v<T>)
	constexpr std::size_t CountStructMember() noexcept {
		return detail::CountStructMember<T, 0>();
	}

	template < size_t I, typename T >
		requires (std::is_aggregate_v<T>)
	auto& GetStructMember(T& v) {
		return *std::get<I>(detail::tie<T>(v));
	}

}

namespace biscuit {
	struct sTest {
		int a;
		std::unique_ptr<int> b;
	};

	static_assert(CountStructMember<sTest>() == 2);

}