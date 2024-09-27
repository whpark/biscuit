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
#pragma pack(push, 8)

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

	//-----------------------------------------------------------------------------
	// axis
	enum class eAXIS : int {
		//eAXIS::NW = -4, eAXIS::NZ, eAXIS::NY, eAXIS::NX, eAXIS::NONE = 0, eAXIS::X, eAXIS::Y, eAXIS::Z, eAXIS::W,

		NONE = 0,
		X =   1,	NX = -X,
		Y = X+1,	NY = -Y,
		Z = Y+1,	NZ = -Z,
		W = Z+1,	NW = -W,

		PX = X,
		PY = Y,
		PZ = Z,
		PW = W,
	};

	BSC__NODISCARD inline bool IsValidCoordSystem(eAXIS eDX, eAXIS eDY) {
		int iDX = std::abs(static_cast<int>(eDX)), iDY = std::abs(static_cast<int>(eDY));
		return static_cast<int>(eDX) && static_cast<int>(eDY) && (static_cast<int>(iDX) != static_cast<int>(iDY));
	}
	BSC__NODISCARD inline bool IsValidCoordSystem(eAXIS eDX, eAXIS eDY, eAXIS eDZ) {
		int iDX = std::abs(static_cast<int>(eDX)), iDY = std::abs(static_cast<int>(eDY)), iDZ = std::abs(static_cast<int>(eDZ));
		return static_cast<int>(eDX) && static_cast<int>(eDY) && static_cast<int>(eDZ)
			&& (static_cast<int>(iDX) != static_cast<int>(iDY))
			&& (static_cast<int>(iDY) != static_cast<int>(iDZ))
			&& (static_cast<int>(iDZ) != static_cast<int>(iDX));
	}

	BSC__NODISCARD inline eAXIS GetOppositeDirection(eAXIS eDirection) { return (eAXIS)(-static_cast<int>(eDirection)); }

	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr inline eAXIS GetDirection(std::basic_string_view<tchar> svName) {
		signed char eNegative = tszsearch(svName, '-') ? -1 : 1;
		if (svName.find('x') != svName.npos or svName.find('X') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::X)*eNegative);
		if (svName.find('y') != svName.npos or svName.find('Y') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::Y)*eNegative);
		if (svName.find('z') != svName.npos or svName.find('Z') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::Z)*eNegative);
		return eAXIS::NONE;
	}
	BSC__NODISCARD constexpr inline auto GetDirectionName(eAXIS eDirection) -> std::basic_string_view<char> {
		using namespace std::literals;
		switch (eDirection) {
		case eAXIS::NONE : return ""sv;
		case eAXIS::X  : return  "X"sv; case  eAXIS::Y : return  "Y"sv; case eAXIS::Z  : return  "Z"sv;
		case eAXIS::NX : return "-X"sv; case eAXIS::NY : return "-Y"sv; case eAXIS::NZ : return "-Z"sv;
		}
		return ""sv;
	}

	//-----------------------------------------------------------------------------

	BSC__NODISCARD inline bool IsChildPath(std::filesystem::path const& a, std::filesystem::path const& base) {
		auto rel = std::filesystem::relative(a, base);
		return !rel.empty() && rel.is_relative() and !rel.string().starts_with("..");
	};

	BSC__NODISCARD inline bool HasParentPath(std::filesystem::path const& path) {
		return path.has_parent_path() and path != path.parent_path();
	}

	/// @brief Get Project Name from source file path
	/// @param l : don't touch.
	/// @return 
	BSC__NODISCARD /*constexpr*/ auto GetProjectName(std::source_location const& l = std::source_location::current())
		-> std::wstring
	{
		std::filesystem::path path = l.file_name();
		if (path.extension() == ".h")	// CANNOT get project name from header file
			return {};
		while (HasParentPath(path)) {
			auto filename = path.filename();
			path = path.parent_path();
			if (path.filename() == L"src")
				return filename;
		}
		return {};
	}

	BSC__NODISCARD auto GetProjectRootFolder(std::wstring const& strProjectNameToBeRemoved,
											 std::filesystem::path path = std::filesystem::current_path()) 
		-> std::filesystem::path
	{
		// Init Temp Folder Names (ex, "Debug", "Release", ...)
		static auto const strsTempFolder = []{
			std::vector<std::wstring> folders;
			#ifdef _DEBUG
				folders.push_back(L"Debug");
				folders.push_back(L"Debug64");
				folders.push_back(L"Debug.v142");
				folders.push_back(L"Debug.v143");
			#else
				folders.push_back(L"Release");
				folders.push_back(L"Release64");
				folders.push_back(L"Release.v142");
				folders.push_back(L"Release.v143");
			#endif
				if constexpr (sizeof(size_t) == sizeof(uint64_t)) {
					folders.push_back(L"x64");
				} else if constexpr (sizeof(size_t) == sizeof(uint32_t)) {
					folders.push_back(L"x86");
					folders.push_back(L"Win32");
				}
				folders.push_back(L"Temp");
			return folders;
		}();

		// Remove Project Folder ( ex, [src]/[ProjectName] )
		if ( HasParentPath(path) and (path.filename() == strProjectNameToBeRemoved) ) {
			path = path.parent_path();
			if (HasParentPath(path) and path.filename() == L"src") {
				path = path.parent_path();
			}
		}

		// Remove Output Dir Folder Name ( ex, Temp/x64/Debug/ )
		for (auto const& strTempFolder : strsTempFolder) {
			if (!HasParentPath(path))
				break;
			if (path.filename() == strTempFolder) {
				path = path.parent_path();
			}
		}
		return path;
	}

	auto SetCurrentPath_ProjectFolder(std::filesystem::path const& pathRelToProjectRoot,
									  std::source_location const& l = std::source_location::current())
		-> std::optional<std::filesystem::path>
	{
		// first, Get Root Folder, and then attach [pathRel]
		auto projectName = biscuit::GetProjectName(l);
		auto path = biscuit::GetProjectRootFolder(projectName);
		if (!pathRelToProjectRoot.empty()) {
			if (pathRelToProjectRoot.filename() != path.filename())
				path /= pathRelToProjectRoot;
		}
		std::error_code ec{};
		std::filesystem::create_directories(path, ec);
		ec = {};
		std::filesystem::current_path(path, ec);
		if (ec)
			return {};
		return path;
	}
	inline auto SetCurrentPath_ProjectFolder(std::source_location const& l = std::source_location::current()) {
		auto projectName = biscuit::GetProjectName(l);
		return SetCurrentPath_ProjectFolder(std::filesystem::path(L"src") / projectName, l);
	}
	inline auto SetCurrentPath_BinFolder(std::source_location const& l = std::source_location::current()) {
		return SetCurrentPath_ProjectFolder(L"bin", l);
	}

#pragma pack(pop)
}	// namespac biscuit
