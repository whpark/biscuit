module;

//////////////////////////////////////////////////////////////////////
//
// stop_watch.ixx (for debugging)
//
// PWH
// 2021.10.31. misc.h에서 분리
// 2024-09-27. biscuit
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"

#include <Windows.h>

export module biscuit.stop_watch;
import std;
import "biscuit/dependencies/fmt.hxx";
import biscuit.aliases;
import biscuit.concepts;
import biscuit.string;

using namespace std::literals;
using namespace biscuit::literals;

namespace concepts = biscuit::concepts;

export namespace biscuit {

	//-------------------------------------------------------------------------
	/// @brief StopWatch
	/// @tparam tclock 
	template < concepts::string_elem tchar, typename tresolution = std::chrono::duration<double>, typename tclock = std::chrono::steady_clock >
	class TStopWatch {
	protected:
		tclock::time_point t0 { tclock::now() };
		std::basic_ostream<tchar>& os;
		thread_local static inline int depth {-1};

		//struct item {
		//	tclock::time_point t0;
		//	tclock::time_point t1;
		//	std::string str;
		//};
		//std::deque<item> laps;
		constexpr static auto& GetDefaultOutStream() {
			if constexpr (concepts::is_one_of<tchar, char, char8_t>) {
				return std::cout;
			} else if constexpr (concepts::is_one_of<tchar, wchar_t, char16_t>) {
				return std::wcout;
			}
		}

	public:
		TStopWatch(std::basic_ostream<tchar>& os = GetDefaultOutStream()) : os(os) {
			depth++;
		}
		~TStopWatch() {
			//Lap(RuntimeFormatString(ToExoticString<tchar>("end")));
			Lap(FormatToTString<tchar, "end">());
			os.flush();
			depth--;
		}

		tclock::time_point GetLastTick() const { return t0; };

	public:
		void Start() {
			t0 = tclock::now();
		}
		void Stop() {}

	public:
		template < typename ... targs >
		void Lap(fmt::basic_format_string<tchar, targs...> const& fmt, targs&& ... args) {
			auto t = tclock::now();
			os << FormatToTString<tchar, "STOP_WATCH {:{}}[ ">(' ', depth * 4);

			os << Format(fmt, std::forward<targs>(args)...);

			os << FormatToTString<tchar, " ] {}\n">(std::chrono::duration_cast<tresolution>(t-t0));
			//os << (tchar)'\n';
			t0 = tclock::now();
		}
	};


	//-------------------------------------------------------------------------
	/// @brief DurationReport
	class xDurationWatch {
	public:
		std::source_location m_loc;
		std::chrono::duration<double> m_durMin;
		std::chrono::steady_clock::time_point m_t0 = std::chrono::steady_clock::now();
	public:
		xDurationWatch(std::chrono::duration<double> durMin = std::chrono::milliseconds(100), std::source_location loc = std::source_location::current()) : m_durMin(durMin), m_loc(loc) {
		}
	#ifdef _DEBUG
		~xDurationWatch() {
			auto t1 = std::chrono::steady_clock::now();
			auto d = std::chrono::duration_cast<std::chrono::microseconds>(t1 - m_t0);
			if (d < m_durMin)
				return;
			std::string msg = fmt::format("Duration: {} ({})\n", d, m_loc.function_name());
		#ifdef _WINDOWS
			OutputDebugStringA(msg.c_str());
		#else
			fmt::print("{}", msg);
		#endif
		}
	#endif
	};


}	// namespace biscuit

