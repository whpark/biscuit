module;

//////////////////////////////////////////////////////////////////////
//
// misc.ixx
//
// PWH
// 2019.01.03.
// define macro ==> template class / function
// 2019.07.24. QL -> GTL
// 2021.01.04.
// 2024-08-12. biscuit.
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"

export module biscuit.misc;
import std;
import biscuit.concepts;

namespace concepts = biscuit::concepts;

namespace biscuit {
	namespace detail {
		template < typename T = std::uint32_t >		constexpr T bit_single(int b)					{ return ((T)1 << b); }
		template < typename T, typename ... Bits >	constexpr T bit_multi(Bits ... bits)			{ return (bit_single<T>(bits) | ...); }
	}
}

export namespace biscuit {

	template < typename tTargetDuration = std::chrono::seconds,
		typename tSourceClock = std::chrono::system_clock,
		typename tSourceDuration = typename tSourceClock::duration >
	BSC__NODISCARD constexpr auto ToLocalTime(std::chrono::time_point<tSourceClock, tSourceDuration> const& t)
		-> std::chrono::time_point<tSourceClock, tTargetDuration>
	{
		auto lt = std::chrono::current_zone()->to_local(t);
		return std::chrono::time_point_cast<tTargetDuration, tSourceClock, tSourceDuration>(lt);
	}
	BSC__NODISCARD constexpr auto ToLocalTime_sec(auto t) { return ToLocalTime<std::chrono::seconds>(t); }
	BSC__NODISCARD constexpr auto ToLocalTime_ms(auto t) { return ToLocalTime<std::chrono::milliseconds>(t); }
	BSC__NODISCARD constexpr auto ToLocalTime_us(auto t) { return ToLocalTime<std::chrono::microseconds>(t); }

	BSC__NODISCARD constexpr auto To_s(auto t) { return std::chrono::duration_cast<std::chrono::seconds>(t); }
	BSC__NODISCARD constexpr auto To_ms(auto t) { return std::chrono::duration_cast<std::chrono::milliseconds>(t); }
	BSC__NODISCARD constexpr auto To_us(auto t) { return std::chrono::duration_cast<std::chrono::microseconds>(t); }


	template < concepts::string_elem tchar, size_t N >
	struct xStringLiteral {
		tchar str[N];
		constexpr xStringLiteral(tchar const (&sz)[N]) {
			std::copy_n(sz, N, str);
		}
	};

	template < concepts::string_elem tchar_to, xStringLiteral literal, bool SWAP_BYTE = false >
	struct TStringLiteral {
		tchar_to value[std::size(literal.str)];
		constexpr TStringLiteral() {
			for (size_t i{}; i < std::size(literal.str); i++) {
				//static_assert(literal.str[i] > 0 and literal.str[i] < 127);
				if (literal.str[i] < 0 or literal.str[i] > 127)
					throw std::logic_error("invalid character");
				if constexpr (SWAP_BYTE) {
					value[i] = std::byteswap(static_cast<tchar_to>(literal.str[i]));
				}
				else {
					value[i] = static_cast<tchar_to>(literal.str[i]);
				}
			}
		}
	};

	//! @brief call back function for progress dialog or something like...
	using callback_progress_t = std::function<bool (int iPercent, bool bDone, bool bError)>;

	//-----------------------------------------------------------------------------
	// defaulted Copy Constructor/Assignment 에서 제외할 멤버 변수용
	template < typename T >
	class TCopyTransparent : public T {
	public:
		using target_t = T;

		using T::T;
		//TCopyTransparent() : T() {}
		TCopyTransparent(const TCopyTransparent& B) { }
		TCopyTransparent(TCopyTransparent&& B) = default;
		TCopyTransparent& operator = (const TCopyTransparent& B) { return *this; }
		TCopyTransparent& operator = (TCopyTransparent&& B) = default;
	};


	//-----------------------------------------------------------------------------
	//
	template <typename T>
	struct equal_to_value {
		T value;
		constexpr bool operator()(T const& v) const { return value == v; }
	};
	struct equal_to_zero {
		template <typename T>
		constexpr bool operator()(T const& v) const { return v == T{}; }
	};
	struct non_zero {
		template <typename T>
		constexpr bool operator()(T const& v) const { return v != T{}; }
	};

	/// @brief RAI helper
	// xTrigger -> xFinalAction (naming from gsl::final_action)
	struct xFinalAction {
		std::function<void()> m_action;
		BSC__NODISCARD xFinalAction(std::function<void()> action) noexcept : m_action(action) {}

		~xFinalAction() {
			DoAction();
		}
		void DoAction() {
			if (auto action = std::exchange(m_action, nullptr)) {
				action();
			}
		}
		void Release() {
			m_action = nullptr;
		}
	};

	// Boolean
	template < typename ... Args >	BSC__NODISCARD constexpr bool IsAllTrue(Args&& ... args)			{ return (args && ...); }
	template < typename ... Args >	BSC__NODISCARD constexpr bool IsNoneTrue(Args&& ... args)			{ return (!args && ...); }
	template < typename ... Args >	BSC__NODISCARD constexpr bool IsAllFalse(Args&& ... args)			{ return ((false == args) && ...); }
	template < typename ... Args >	BSC__NODISCARD constexpr bool IsAnyOneTrue(Args&& ... args)			{ return (args ||...); }
	template < typename ... Args >	BSC__NODISCARD constexpr bool IsAllSame(Args&& ... args)			{ return IsAllTrue(args...) || IsNoneTrue(args...); }

	template < typename T, typename ... Args >	BSC__NODISCARD constexpr bool IsOneOf(T v, Args&& ... args)	{ return ((v == args) || ...); }
	template < typename T, typename ... Args >	BSC__NODISCARD constexpr bool IsNoneOf(T v, Args&& ... args)	{ return ((v != args) && ...); }

	template < typename T >			BSC__NODISCARD constexpr bool IsBetween(T const& v, T const& left, T const& right)	{ return (left <= v) && (v < right); }

	// Bit Set
	template < typename ... Bits >	BSC__NODISCARD constexpr std::uint32_t Bit32(Bits ... bits)			{ return detail::bit_multi<std::uint32_t>(bits...); }
	template < typename ... Bits >	BSC__NODISCARD constexpr std::uint64_t Bit64(Bits ... bits)			{ return detail::bit_multi<std::uint64_t>(bits...); }

	template < typename ... Bits >	BSC__NODISCARD constexpr std::bitset<32> BitSet32(Bits ... bits)	{ return std::bitset<32>(bit32(bits...)); }
	template < typename ... Bits >	BSC__NODISCARD constexpr std::bitset<64> BitSet64(Bits ... bits)	{ return std::bitset<64>(bit64(bits...)); }


	// Word Align Position
	template < std::integral T >	BSC__NODISCARD constexpr inline T AdjustAlign128(T w)				{ return ((w+15)/16*16); }	//	((w+15)>>4)<<4
	template < std::integral T >	BSC__NODISCARD constexpr inline T AdjustAlign64(T w)				{ return ((w+7)/8*8); }		//	((w+ 7)>>3)<<3
	template < std::integral T >	BSC__NODISCARD constexpr inline T AdjustAlign32(T w)				{ return ((w+3)/4*4); }		//	((w+ 3)>>2)<<2
	template < std::integral T >	BSC__NODISCARD constexpr inline T AdjustAlign16(T w)				{ return ((w+1)/2*2); }		//	((w+ 1)>>1)<<1

	template < std::integral type >
	BSC__NODISCARD inline type GetNetworkByteOrder(type const v) {
		if constexpr (std::endian::native == std::endian::little) {
			return std::byteswap(v);
		}
		else
			return v;
	}

	namespace detail {

		// tuple has special constructor for std::pair. converting pair -> T1, T2...
		template < typename T1, typename T2 >
		class pair : public std::pair<T1, T2> {
		public:
			using target_t = std::pair<T1,T2>;
			using target_t::target_t;
		};

		//// non-const function call in const function. -----> std::remove_cvref_t
		//template < typename T >
		//using remove_ref_const_t = std::remove_const_t<std::remove_reference_t<T>>;

	}


	namespace detail {
		template < size_t N, size_t ... I, class Func >
		constexpr bool ForEachIntSeq(Func&& func, std::integer_sequence<size_t, I...>) {
			if constexpr (sizeof...(I) == 0) 
				return false;
			else { 
				if (func.template operator()<N-(sizeof...(I))>())
					return true;
				if constexpr (sizeof...(I) > 1)
					return ForEachIntSeq<N>(std::move(func), std::make_integer_sequence<size_t, sizeof...(I)-1>{});
				else
					return false;
			}
		}
	}
	template < size_t N, class Func >
	constexpr bool ForEachIntSeq(Func&& func) {
		return ::biscuit::detail::ForEachIntSeq<N>(std::move(func), std::make_integer_sequence<size_t, N>{});
	}


}	// namespac biscuit

