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

export module biscuit:misc;
import :concepts;
import std.compat;

namespace concepts = biscuit::concepts;

export namespace biscuit {
#pragma pack(push, 8)

	template < typename tTargetDuration, typename tSourceClock, typename tSourceDuration >
	std::chrono::time_point<tSourceClock, tTargetDuration> ToLocalTime(std::chrono::time_point<tSourceClock, tSourceDuration>& const t) {
		auto lt = std::chrono::current_zone()->to_local(t);
		return std::chrono::time_point_cast<tTargetDuration, tSourceClock, tSourceDuration>(lt);
	}
//	template < typename tTargetDuration = std::chrono::seconds, typename tSourceClock = std::chrono::system_clock, typename tSourceDuration = typename tSourceClock::duration >
//	std::chrono::time_point<tSourceClock, tTargetDuration> ToLocalTime(std::chrono::time_point<tSourceClock, tSourceDuration> const t) {
//		auto lt = std::chrono::current_zone()->to_local(t);
//		return std::chrono::time_point_cast<ttime_point>(lt);
//	}
//	auto ToLocalTime_sec(auto t) { return ToLocalTime<std::chrono::seconds>(t); }
//	auto ToLocalTime_ms(auto t) { return ToLocalTime<std::chrono::milliseconds>(t); }
//	auto ToLocalTime_us(auto t) { return ToLocalTime<std::chrono::microseconds>(t); }

	template < concepts::string_elem tchar, size_t N >
	struct xStringLiteral {
		tchar str[N];
		constexpr xStringLiteral(tchar const (&sz)[N]) {
			std::copy_n(sz, N, str);
		}
	};

	template < concepts::string_elem tchar_to, xStringLiteral literal >
	struct TStringLiteral {
		tchar_to value[std::size(literal.str)];
		constexpr TStringLiteral() {
			for (size_t i{}; i < std::size(literal.str); i++) {
				//static_assert(literal.str[i] > 0 and literal.str[i] < 127);
				if (literal.str[i] < 0 or literal.str[i] > 127)
					throw std::logic_error("invalid character");
				value[i] = static_cast<tchar_to>(literal.str[i]);
			}
		}
	};

//	//! @brief call back function for progress dialog or something like...
//	using callback_progress_t = std::function<bool (int iPercent, bool bDone, bool bError)>;
//
//	//-----------------------------------------------------------------------------
//	// defaulted Copy Constructor/Assignment 에서 제외할 멤버 변수용
//	template < typename T >
//	class TCopyTransparent : public T {
//	public:
//		using base_t = T;
//
//		using T::T;
//		//TCopyTransparent() : T() {}
//		TCopyTransparent(const TCopyTransparent& B) { }
//		TCopyTransparent(TCopyTransparent&& B) = default;
//		TCopyTransparent& operator = (const TCopyTransparent& B) { return *this; }
//		TCopyTransparent& operator = (TCopyTransparent&& B) = default;
//	};
//
//
//	//-----------------------------------------------------------------------------
//	//
//	template <typename T>
//	struct equal_to_value {
//		T value;
//		constexpr bool operator()(T const& v) const { return value == v; }
//	};
//	struct equal_to_zero {
//		template <typename T>
//		constexpr bool operator()(T const& v) const { return v == T{}; }
//	};
//	struct non_zero {
//		template <typename T>
//		constexpr bool operator()(T const& v) const { return v != T{}; }
//	};
//
//	/// @brief RAI helper
//	// xTrigger -> xFinalAction (naming from gsl::final_action)
//	struct xFinalAction {
//		std::function<void()> m_action;
//		[[nodiscard]] xFinalAction(std::function<void()> action) noexcept : m_action(action) {}
//
//		~xFinalAction() {
//			DoAction();
//		}
//		void DoAction() {
//			if (auto action = std::exchange(m_action, nullptr)) {
//				action();
//			}
//		}
//		void Release() {
//			m_action = nullptr;
//		}
//	};
//
//	// Boolean
//	template < typename ... Args > constexpr bool IsAllTrue(Args&& ... args)						{ return (args && ...); }
//	template < typename ... Args > constexpr bool IsNoneTrue(Args&& ... args)						{ return (!args && ...); }
//	template < typename ... Args > constexpr bool IsAllFalse(Args&& ... args)						{ return ((false == args) && ...); }
//	template < typename ... Args > constexpr bool IsAnyTrue(Args&& ... args)						{ return (args ||...); }
//	template < typename ... Args > constexpr bool IsAllSame(Args&& ... args)						{ return IsAllTrue(args...) || IsNoneTrue(args...); }
//
//	template < typename T, typename ... Args > constexpr bool IsValueOneOf(T v, Args&& ... args)	{ return ((v == args) || ...); }
//	template < typename T, typename ... Args > constexpr bool IsValueNoneOf(T v, Args&& ... args)	{ return ((v != args) && ...); }
//
//	template < typename T >
//	constexpr bool IsInside(T const& v, T const& left, T const& right)								{ return (left <= v) && (v < right); }
//
//	// Bit Set
//	namespace detail {
//		template < typename T = std::uint32_t >		constexpr T bit_single(int b)					{ return ((T)1 << b); }
//		template < typename T, typename ... Bits >	constexpr T bit_multi(Bits ... bits)			{ return (bit_single<T>(bits) | ...); }
//	}
//	template < typename ... Bits >				constexpr std::uint32_t Bit32(Bits ... bits)		{ return detail::bit_multi<std::uint32_t>(bits...); }
//	template < typename ... Bits >				constexpr std::uint64_t Bit64(Bits ... bits)		{ return detail::bit_multi<std::uint64_t>(bits...); }
//
//	template < typename ... Bits >				constexpr std::bitset<32> BitSet32(Bits ... bits)	{ return std::bitset<32>(bit32(bits...)); }
//	template < typename ... Bits >				constexpr std::bitset<64> BitSet64(Bits ... bits)	{ return std::bitset<64>(bit64(bits...)); }
//
//
//	// Word Align Position
//	template < std::integral T > constexpr inline T AdjustAlign128(T w) { return ((w+15)/16*16); }	//	((w+15)>>4)<<4
//	template < std::integral T > constexpr inline T AdjustAlign64(T w) { return ((w+7)/8*8); }		//	((w+ 7)>>3)<<3
//	template < std::integral T > constexpr inline T AdjustAlign32(T w) { return ((w+3)/4*4); }		//	((w+ 3)>>2)<<2
//	template < std::integral T > constexpr inline T AdjustAlign16(T w) { return ((w+1)/2*2); }		//	((w+ 1)>>1)<<1
//
//
//	// byte swap
//#if (GTL__STRING_PRIMITIVES__WINDOWS_FRIENDLY)
//	template < std::integral type >
//	[[nodiscard]] inline auto GetByteSwap(type v) {
//		if constexpr (sizeof(v) == sizeof(std::uint8_t)) {
//			return v;
//		} else if constexpr (sizeof(v) == sizeof(std::uint16_t)) {
//			return _byteswap_ushort(v);
//		} else if constexpr (sizeof(v) == sizeof(std::uint32_t)) {
//			return _byteswap_ulong(v);
//		} else if constexpr (sizeof(v) == sizeof(std::uint64_t)) {
//			return _byteswap_uint64(v);
//		} else {
//			static_assert(concepts::dependent_false_v, "not supported data type.");
//		}
//	}
//	template < std::integral type >
//	inline void ByteSwap(type& v) {
//		v = GetByteSwap(v);
//	}
//#else
//	template < std::integral type >
//	[[nodiscard]] constexpr inline type GetByteSwap(type const v) {
//		if constexpr (sizeof(v) == 1) {
//			return v;
//		}
//		else {
//			type r{};
//			std::reverse_copy((uint8_t const*)&v, (uint8_t const*)&v+sizeof(v), (uint8_t*)&r);
//			return r;
//		}
//	}
//	template < std::integral type >
//	constexpr inline void ByteSwap(type& v) {
//		if constexpr (sizeof(v) == 1) {
//			return ;
//		}
//		else {
//			std::reverse((uint8_t*)&v, (uint8_t*)&v+sizeof(v));
//		}
//	}
//#endif
//
//	template < std::integral type >
//	inline type GetNetworkByteOrder(type const v) {
//		if constexpr (std::endian::native == std::endian::little) {
//			return GetByteSwap(v);
//		}
//		else
//			return v;
//	}
//
//
//
//	namespace detail {
//
//		// tuple has special constructor for std::pair. converting pair -> T1, T2...
//		template < typename T1, typename T2 >
//		class pair : public std::pair<T1, T2> {
//		public:
//			using base_t = std::pair<T1,T2>;
//			using base_t::base_t;
//
//		};
//
//
//		//// non-const function call in const function. -----> std::remove_cvref_t
//		//template < typename T >
//		//using remove_ref_const_t = std::remove_const_t<std::remove_reference_t<T>>;
//
//
//	}
//
//	/// color
//
//
//	//-----------------------------------------------------------------------------
//	// enum helper
//	template < typename eT, typename T = std::underlying_type<eT> >	constexpr T& enum_as(eT& e)		{ static_assert(sizeof(eT) == sizeof(T)); return (T&)e; }
//	template < typename eT, typename T = std::underlying_type<eT> >	constexpr eT enum_inc(eT& e)	{ static_assert(sizeof(eT) == sizeof(T)); return (eT)++((T&)e); }
//	template < typename eT, typename T = std::underlying_type<eT> >	constexpr eT enum_dec(eT& e)	{ static_assert(sizeof(eT) == sizeof(T)); return (eT)--((T&)e); }
//	//template < typename eT, typename T = std::underlying_type<eT> >	constexpr eT enum_next(eT e)	{ static_assert(sizeof(eT) == sizeof(T)); return (eT)((T&)e+1); }
//	//template < typename eT, typename T = std::underlying_type<eT> >	constexpr eT enum_prev(eT e)	{ static_assert(sizeof(eT) == sizeof(T)); return (eT)((T&)e-1); }
//
//
//	//-----------------------------------------------------------------------------
//	// axis
//	enum class eAXIS : int {
//		//eAXIS::NW = -4, eAXIS::NZ, eAXIS::NY, eAXIS::NX, eAXIS::NONE = 0, eAXIS::X, eAXIS::Y, eAXIS::Z, eAXIS::W,
//
//		NONE = 0,
//		X =   1,	NX = -X,
//		Y = X+1,	NY = -Y,
//		Z = Y+1,	NZ = -Z,
//		W = Z+1,	NW = -W,
//
//		PX = X,
//		PY = Y,
//		PZ = Z,
//		PW = W,
//	};
//
//	inline bool IsValidCoordSystem(eAXIS eDX, eAXIS eDY) {
//		int iDX = std::abs(static_cast<int>(eDX)), iDY = std::abs(static_cast<int>(eDY));
//		return static_cast<int>(eDX) && static_cast<int>(eDY) && (static_cast<int>(iDX) != static_cast<int>(iDY));
//	}
//	inline bool IsValidCoordSystem(eAXIS eDX, eAXIS eDY, eAXIS eDZ) {
//		int iDX = std::abs(static_cast<int>(eDX)), iDY = std::abs(static_cast<int>(eDY)), iDZ = std::abs(static_cast<int>(eDZ));
//		return static_cast<int>(eDX) && static_cast<int>(eDY) && static_cast<int>(eDZ)
//			&& (static_cast<int>(iDX) != static_cast<int>(iDY))
//			&& (static_cast<int>(iDY) != static_cast<int>(iDZ))
//			&& (static_cast<int>(iDZ) != static_cast<int>(iDX));
//	}
//
//	inline eAXIS GetOppositeDirection(eAXIS eDirection) { return (eAXIS)(-static_cast<int>(eDirection)); }
//
//	template < concepts::string_elem tchar >
//	constexpr inline eAXIS GetDirection(std::basic_string_view<tchar> svName) {
//		signed char eNegative = tszsearch(svName, '-') ? -1 : 1;
//		if (svName.find('x') != svName.npos or svName.find('X') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::X)*eNegative);
//		if (svName.find('y') != svName.npos or svName.find('Y') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::Y)*eNegative);
//		if (svName.find('z') != svName.npos or svName.find('Z') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::Z)*eNegative);
//		return eAXIS::NONE;
//	}
//	constexpr inline std::basic_string_view<char> GetDirectionName(eAXIS eDirection) {
//		using namespace std::literals;
//		switch (eDirection) {
//		case eAXIS::NONE : return ""sv;
//		case eAXIS::X  : return  "X"sv; case  eAXIS::Y : return  "Y"sv; case eAXIS::Z  : return  "Z"sv;
//		case eAXIS::NX : return "-X"sv; case eAXIS::NY : return "-Y"sv; case eAXIS::NZ : return "-Z"sv;
//		}
//		return ""sv;
//	}
//
//	//-----------------------------------------------------------------------------
//
//	inline bool IsChildPath(std::filesystem::path const& a, std::filesystem::path const& base) {
//		auto rel = std::filesystem::relative(a, base);
//		return !rel.empty() && rel.is_relative() and !rel.string().starts_with("..");
//	};
//
//	inline bool HasParentPath(std::filesystem::path const& path) {
//		return path.has_parent_path() and path != path.parent_path();
//	}
//
//	/// @brief Get Project Name from source file path
//	/// @param l : don't touch.
//	/// @return 
//	/*constexpr*/ std::wstring GetProjectName(std::source_location const& l = std::source_location::current()) {
//		std::filesystem::path path = l.file_name();
//		if (path.extension() == ".h")	// CANNOT get project name from header file
//			return {};
//		while (HasParentPath(path)) {
//			auto filename = path.filename();
//			path = path.parent_path();
//			if (path.filename() == L"src")
//				return filename;
//		}
//		return {};
//	}
//
//	std::filesystem::path GetProjectRootFolder(std::wstring const& strProjectNameToBeRemoved, std::filesystem::path path = std::filesystem::current_path()) {
//		// Init Temp Folder Names (ex, "Debug", "Release", ...)
//		static auto const strsTempFolder = []{
//			std::vector<std::wstring> folders;
//			#ifdef _DEBUG
//				folders.push_back(L"Debug");
//				folders.push_back(L"Debug64");
//				folders.push_back(L"Debug.v142");
//				folders.push_back(L"Debug.v143");
//			#else
//				folders.push_back(L"Release");
//				folders.push_back(L"Release64");
//				folders.push_back(L"Release.v142");
//				folders.push_back(L"Release.v143");
//			#endif
//				if constexpr (sizeof(size_t) == sizeof(uint64_t)) {
//					folders.push_back(L"x64");
//				} else if constexpr (sizeof(size_t) == sizeof(uint32_t)) {
//					folders.push_back(L"x86");
//					folders.push_back(L"Win32");
//				}
//				folders.push_back(L"Temp");
//			return folders;
//		}();
//
//		// Remove Project Folder ( ex, [src]/[ProjectName] )
//		if ( HasParentPath(path) and (path.filename() == strProjectNameToBeRemoved) ) {
//			path = path.parent_path();
//			if (HasParentPath(path) and path.filename() == L"src") {
//				path = path.parent_path();
//			}
//		}
//
//		// Remove Output Dir Folder Name ( ex, Temp/x64/Debug/ )
//		for (auto const& strTempFolder : strsTempFolder) {
//			if (!HasParentPath(path))
//				break;
//			if (path.filename() == strTempFolder) {
//				path = path.parent_path();
//			}
//		}
//		return path;
//	}
//
//	std::optional<std::filesystem::path> SetCurrentPath_ProjectFolder(std::filesystem::path const& pathRelToProjectRoot, std::source_location const& l = std::source_location::current()) {
//		// first, Get Root Folder, and then attach [pathRel]
//		auto projectName = biscuit::GetProjectName(l);
//		auto path = biscuit::GetProjectRootFolder(projectName);
//		if (!pathRelToProjectRoot.empty()) {
//			if (pathRelToProjectRoot.filename() != path.filename())
//				path /= pathRelToProjectRoot;
//		}
//		std::error_code ec{};
//		std::filesystem::create_directories(path, ec);
//		ec = {};
//		std::filesystem::current_path(path, ec);
//		if (ec)
//			return {};
//		return path;
//	}
//	inline auto SetCurrentPath_ProjectFolder(std::source_location const& l = std::source_location::current()) {
//		auto projectName = biscuit::GetProjectName(l);
//		return SetCurrentPath_ProjectFolder(std::filesystem::path(L"src") / projectName, l);
//	}
//	inline auto SetCurrentPath_BinFolder(std::source_location const& l = std::source_location::current()) {
//		return SetCurrentPath_ProjectFolder(L"bin", l);
//	}

#pragma pack(pop)
}	// namespac biscuit
