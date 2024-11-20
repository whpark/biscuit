module;

#include "biscuit/macro.h"

export module biscuit.string.arithmetic;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.convert_codepage;
import biscuit.string.basic;


export namespace biscuit {

	//-----------------------------------------------------------------------------------------------------------------------------
	/// @brief string to int/double ...
	/// @return 
	template < concepts::arithmetic tvalue, concepts::tstring_like tstring >
	BSC__NODISCARD tvalue ToNumber(tstring const& sv, int base = 10, char separator = 0, std::from_chars_result* result = nullptr) {
		if (std::size(sv) > BSC_RSIZE_MAX)
			return 0;
		tvalue value{};
		auto* b = std::data(sv);
		auto* e = std::data(sv) + std::size(sv);

		thread_local static std::string body;

		// skip leading spaces
		while (b < e and std::iswspace(*b)) b++;

		// skip '+' sign
		if (b < e and (*b == '+')) {
			b++;
			while (b < e and std::iswspace(*b)) b++;
		}

		// base prefix
		if constexpr (std::is_integral_v<tvalue>) {
			if (base == 0) {
				std::string_view prefix(b, e);
				if (prefix.starts_with("0x")) {
					b += 2;
					base = 16;
				}
				else if (prefix.starts_with("0b")) {
					b += 2;
					base = 2;
				}
				else if (prefix.starts_with("0")) {
					b += 1;
					base = 8;
				}
				else {
					base = 10;
				}
			}
		}

		if (separator != 0) {
			body = tszrmcharTo(std::string_view(b, e), separator);
			b = body.data();
			e = b + body.size();
		}
		
		std::from_chars_result r{};
		if constexpr (std::is_integral_v<tvalue>) {
			r = std::from_chars(b, e, value, base);
		}
		else {
			r = std::from_chars(b, e, value);
		}
		if (result)
			*result = r;

		return value;
	}

	//-----------------------------------------------------------------------------------------------------------------------------
	// tszto
	template < concepts::arithmetic tvalue, concepts::tstring_like tstring >
	BSC__NODISCARD constexpr inline tvalue tszto(tstring const& sv, int base = 10, char separator = 0) {
		using tchar = concepts::value_t<tstring>;
		if constexpr (std::is_same_v<tchar, char>) {
			return ToNumber<tvalue>(sv, base, separator);
		}
		else if constexpr (std::is_same_v<tchar, char8_t>) {
			return ToNumber<tvalue>(u8A(sv), base, separator);
		}
		else {
			if (auto r = ConvertString<char>(sv))
				return ToNumber<tvalue>(*r, base, separator);
			return {};
		}
	}

	template < concepts::tstring_like tstring >
	BSC__NODISCARD constexpr inline auto tsztoi(tstring const& sv, int base = 10, char separator = 0) { return tszto<int>(sv,    base, separator); }
	template < concepts::tstring_like tstring >
	BSC__NODISCARD constexpr inline auto tsztou(tstring const& sv, int base = 10, char separator = 0) { return tszto<uint>(sv,   base, separator); }
	template < concepts::tstring_like tstring >
	BSC__NODISCARD constexpr inline auto tsztod(tstring const& sv, int base = 10, char separator = 0) { return tszto<double>(sv, base, separator); }


	//-----------------------------------------------------------------------------------------------------------------------------
	/// @brief adds comma into number string
	/// @param separator 
	template < concepts::tstring_like tstring, int interval = 3 >
	BSC__NODISCARD constexpr std::basic_string<concepts::value_t<tstring>> AddThousandComma(tstring const& sv, concepts::value_t<tstring> separator = ',') {
		static_assert(interval > 0);
		using namespace std::literals;
		using tchar = concepts::value_t<tstring>;
		using view_t = std::basic_string_view<tchar>;
		using string_t = std::basic_string<tchar>;

		string_t result;

		size_t nComma{};
		static auto const digits = ElevateAnsiToStandard<tchar>("0123456789"sv);
		auto pos = sv.find_first_of(digits);
		if (pos != sv.npos) {
			auto len = std::min(sv.size()-pos, sv.substr(pos).find_first_not_of(digits));
			nComma = (len - 1) / interval;
			if (auto jump = len % interval)
				pos += jump;
			else
				pos += interval;
		}

		result.reserve(sv.size()+nComma);
		result = sv.substr(0, pos);
		for (; nComma > 0; nComma--, pos += interval) {
			result += separator;
			result += sv.substr(pos, interval);
		}
		result += sv.substr(pos);
		return result;
	}

	template < concepts::arithmetic tvalue, int interval = 3 >
	constexpr std::string AddThousandCommaA(tvalue value, char separator = ',') {
		return AddThousandComma<char, interval>(std::to_string(value), separator);
	}
	template < concepts::arithmetic tvalue, int interval = 3 >
	constexpr std::wstring AddThousandCommaW(tvalue value, wchar_t separator = L',') {
		return AddThousandComma<wchar_t, interval>(std::to_wstring(value), separator);
	}

	template < concepts::arithmetic tvalue, typename ... targs >
	constexpr std::string ToChars(tvalue value, targs&& ... args) {
		char buf[64];
		std::to_chars_result r = std::to_chars(buf, buf+sizeof(buf), value, std::forward<targs>(args)...);
		if (r.ec != std::errc{})
			return std::format("{}", value);
		return std::string(buf, r.ptr);
	}

}

