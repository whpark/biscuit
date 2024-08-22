module;
export module biscuit:arithmetic;
import :concepts;

//namespace concepts = biscuit::concepts;

export namespace biscuit {

	/// @brief string to int/double ...
	/// @return 
	template < concepts::arithmetic value_t >
	value_t ToNumber(std::string_view sv, int base = 0, std::from_chars_result* result = {}) {
		value_t value{};
		auto* b = sv.data();
		auto* e = sv.data() + sv.size();
		// skip leading spaces
		while (b < e and std::isspace(*b)) b++;
		if constexpr (std::is_integral_v<value_t>) {
			if (base == 0) {
				std::string_view sv(b, e);
				if (sv.starts_with("0x")) {
					base = 16;
					b += 2;
				}
				else if (sv.starts_with("0b")) {
					b += 2;
					base = 2;
				}
				else if (sv.starts_with("0")) {
					b += 1;
					base = 8;
				}
				else {
					base = 10;
				}
			}
			auto r = std::from_chars(b, e, value, base);
			if (result)
				*result = r;
		}
		else {
			auto r = std::from_chars(b, e, value);
			if (result)
				*result = r;
		}
		return value;
	}

	template < concepts::arithmetic value_t, concepts::string_elem tchar_t >
	value_t ToNumber(std::basic_string<tchar_t> const& str, int base = 0, std::from_chars_result* result = {}) {
		return ToNumber<value_t>(std::string_view(ToString<char, tchar_t>(str)), base, result);
	}


	//-------------------------------------------------------------------------
	// simple math
	// Square
	template < typename T >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T>>)
	constexpr T Square(T x) { return x * x; }


	//-------------------------------------------------------------------------
	// Round, Ceil, Floor

	/// @brief Round
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	constexpr [[nodiscard]] T_DEST Round(T_SOURCE v) {
		return (T_DEST)std::round(v);
	}

	template < typename T_DEST = int, typename T_SOURCE= double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	constexpr [[nodiscard]] T_DEST Round(T_SOURCE v, T_SOURCE place) {
		return T_DEST(std::round(v/place*0.1)*place*10);
		//return T_DEST(T_DEST(v/place/10+0.5*(v<T_SOURCE{}?-1:1))*place*10);
	}

	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	constexpr [[nodiscard]] T_DEST RoundOrForward(T_SOURCE v) {
		if constexpr (std::is_integral_v<T_SOURCE>) {
			return T_DEST(v);
		}
		else {
			return (T_DEST)std::round(v);
		}
	}

	/// @brief Ceil
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	constexpr [[nodiscard]] T_DEST Ceil(T_SOURCE v) {
		if constexpr (std::is_integral_v<T_DEST> && std::is_floating_point_v<T_SOURCE>) {
			return T_DEST(std::ceil(v));	// std::ceil is not constexpr yet.
		}
		else {
			return (T_DEST)v;
		}
	}
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	constexpr [[nodiscard]] T_DEST Ceil(T_SOURCE v, T_SOURCE place) {
		return T_DEST(std::ceil(v/place/10)*place*10);
	}

	/// @brief Floor
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	constexpr [[nodiscard]] T_DEST Floor(T_SOURCE v) {
		if constexpr (std::is_integral_v<T_DEST> && std::is_floating_point_v<T_SOURCE>) {
			return T_DEST(std::floor(v));	// std::floor is not constexpr yet.
		}
		else {
			return (T_DEST)v;
		}
	}
	template < typename T_DEST = int, typename T_SOURCE = double >
		requires (std::is_arithmetic_v<std::remove_cvref_t<T_DEST>> and std::is_arithmetic_v<std::remove_cvref_t<T_SOURCE>>)
	constexpr [[nodiscard]] T_DEST Floor(T_SOURCE v, T_SOURCE place) {
		return T_DEST(std::floor(v/place*0.1)*place*10);
	}


}	// namespace biscuit
