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
	template < concepts::arithmetic tvalue >
	BSC__NODISCARD tvalue ToNumber(std::string_view sv, int base = 10, char seperator = 0) {
		if (sv.size() > BSC_RSIZE_MAX)
			return 0;
		tvalue value{};
		auto* b = sv.data();
		auto* e = sv.data() + sv.size();

		std::string body;

		// skip leading spaces
		while (b < e and std::isspace(*b)) b++;

		// skip '+' sign
		if (b < e and (*b == '+')) {
			b++;
			while (b < e and std::isspace(*b)) b++;
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

		if (seperator != 0) {
			body = tszrmcharTo(std::string_view(b, e), seperator);
			b = body.data();
			e = b + body.size();
		}
		
		if constexpr (std::is_integral_v<tvalue>) {
			std::from_chars(b, e, value, base);
		}
		else {
			std::from_chars(b, e, value);
		}

		return value;
	}

	//-----------------------------------------------------------------------------------------------------------------------------
	// tszto
	template < concepts::arithmetic tvalue, concepts::tstring_like tstring >
	BSC__NODISCARD constexpr inline tvalue tszto(tstring const& sv, int base = 10, char seperator = 0) {
		using tchar = concepts::value_t<tstring>;
		if constexpr (std::is_same_v<tchar, char>) {
			return ToNumber<tvalue>(sv, base, seperator);
		}
		else if constexpr (std::is_same_v<tchar, char8_t>) {
			return ToNumber<tvalue>(u8A(sv), base, seperator);
		}
		else {
			return ToNumber<tvalue>(ConvertString<char>(sv), base, seperator);
		}
	}

	template < concepts::tstring_like tstring >
	BSC__NODISCARD constexpr inline auto tsztoi(tstring const& sv, int base = 10, char seperator = 0) { return tszto<int>(sv,    base, seperator); }
	template < concepts::tstring_like tstring >
	BSC__NODISCARD constexpr inline auto tsztou(tstring const& sv, int base = 10, char seperator = 0) { return tszto<uint>(sv,   base, seperator); }
	template < concepts::tstring_like tstring >
	BSC__NODISCARD constexpr inline auto tsztod(tstring const& sv, int base = 10, char seperator = 0) { return tszto<double>(sv, base, seperator); }


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

}
