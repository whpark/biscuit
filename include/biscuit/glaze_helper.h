//module;

#include "biscuit/dependencies/glaze.h"

//export module biscuit.glaze_helper;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.convert_codepage;

/*export*/ namespace biscuit {

	template <typename T>
	T& ValueOr(glz::json_t const& j, T const& def) {
		if (auto* v = j.get_if<T>()) {
			return *v;
		}
		return def;
	};


}	// namespace biscuit;

// std::basic_string
/*export*/ namespace glz {

	// std::u8string
	template <>
	struct from<JSON, std::u8string> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) {
			glz::parse<JSON>::op<Opts>(value, std::forward<Args>(args)...);
		}
	};
	template <>
	struct to<JSON, std::u8string> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) noexcept {
			glz::serialize<JSON>::op<Opts>(value, std::forward<Args>(args)...);
		}
	};

	// std::wstring
	template <>
	struct from<JSON, std::wstring> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) {
			std::u8string str;
			glz::parse<JSON>::op<Opts>(str, std::forward<Args>(args)...);
			value = biscuit::ConvertString<wchar_t>(str).value_or(L"");
		}
	};
	template <>
	struct to<JSON, std::wstring> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) noexcept {
			std::u8string str = biscuit::ConvertString<char8_t>(value).value_or(u8"");
			glz::serialize<JSON>::op<Opts>(str, std::forward<Args>(args)...);
		}
	};

	//std::u16string
	template <>
	struct from<JSON, std::u16string> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) {
			std::u8string str;
			glz::parse<JSON>::op<Opts>(str, std::forward<Args>(args)...);
			value = biscuit::ConvertString<char16_t>(str).value_or(u"");
		}
	};
	template <>
	struct to<JSON, std::u16string> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) noexcept {
			auto str = biscuit::ConvertString<char8_t>(value).value_or(L"");
			glz::serialize<JSON>::op<Opts>(str, std::forward<Args>(args)...);
		}
	};

	// std::u32string
	template <>
	struct from<JSON, std::u32string> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) {
			std::u8string str;
			glz::parse<JSON>::op<Opts>(str, std::forward<Args>(args)...);
			value = biscuit::ConvertString<char32_t>(str).value_or(U"");
		}
	};
	template <>
	struct to<JSON, std::u32string> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) noexcept {
			auto str = biscuit::ConvertString<char8_t>(value).value_or(U"");
			glz::serialize<JSON>::op<Opts>(str, std::forward<Args>(args)...);
		}
	};

	// std::filesystem::path
	template <>
	struct from<JSON, std::filesystem::path> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) {
			std::u8string str;
			glz::parse<JSON>::op<Opts>(str, std::forward<Args>(args)...);
			value = str;
		}
	};
	template <>
	struct to<JSON, std::filesystem::path> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) noexcept {
			auto str = value.u8string();
			glz::serialize<JSON>::op<Opts>(str, std::forward<Args>(args)...);
		}
	};

	// std::chrono::duration
	template <class _Rep, class _Period>
	struct from<JSON, std::chrono::duration<_Rep, _Period>> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) {
			_Rep rep;
			glz::parse<JSON>::op<Opts>(rep, std::forward<Args>(args)...);
			value = std::chrono::duration<_Rep, _Period>(rep);
		}
	};
	template <class _Rep, class _Period>
	struct to<JSON, std::chrono::duration<_Rep, _Period>> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) noexcept {
			glz::serialize<JSON>::op<Opts>(value.count(), std::forward<Args>(args)...);
		}
	};

	// std::chrono::time_point
	template <class _Clock, class _Duration>
	struct from<JSON, std::chrono::time_point<_Clock, _Duration>> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) {
			_Duration duration;
			glz::parse<JSON>::op<Opts>(duration, std::forward<Args>(args)...);
			value = std::chrono::time_point<_Clock, _Duration>(duration);
		}
	};
	template <class _Clock, class _Duration>
	struct to<JSON, std::chrono::time_point<_Clock, _Duration>> {
		template <auto Opts, typename ... Args>
		static void op(auto&& value, Args&& ... args) noexcept {
			glz::serialize<JSON>::op<Opts>(value.time_since_epoch(), std::forward<Args>(args)...);
		}
	};


}

