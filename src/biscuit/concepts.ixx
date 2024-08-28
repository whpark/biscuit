//////////////////////////////////////////////////////////////////////
//
// concepts.ixx:
//
// PWH
//    2020.11.12
//    2024-01-11 from gtl::concepts.h
//
//////////////////////////////////////////////////////////////////////

module;

//#include "biscuit/macro.h"

export module biscuit.concepts;
import std;
import biscuit.aliases;

export namespace biscuit::concepts {

	template<typename> inline constexpr bool dependent_false_v = false;

	/// @brief arithmetic type
	template < typename T >
	concept arithmetic = std::is_arithmetic_v<T>;


	/// @brief trivially copyable
	template < typename T >
	concept trivially_copyable = std::is_trivially_copyable_v<T>;


	/// @brief array or std::array
	template < typename tarray >
	concept is_array = std::is_array_v<tarray> or std::convertible_to<tarray, std::array<typename tarray::value_type, tarray{}.size()>>;


	/// @brief variadic template check version of 'std::is_same_v<t, t2>'
	template < typename tchar, typename ... ttypes >
	concept is_one_of = (std::is_same_v<tchar, ttypes> || ...);


	/// @brief type for string buffer. ex) char buf[12]; std::array<char, 12> buf; std::vector<char> buf;...
	template < typename tcontainer, typename type >
	concept container =
		requires (tcontainer v) {
			{ v[0] }			-> std::convertible_to<type>;
			{ std::size(v) }	-> std::convertible_to<size_t>;
			v.begin();
			v.end();
	};


	/// @brief type for string string.
	template < typename tchar >
	concept string_elem = is_one_of<std::remove_cvref_t<tchar>, char, char8_t, char16_t, char32_t, wchar_t, charKSSM_t >;
	
	/// @brief type for utf (unicode transformation format) string.
	template < typename tchar >
	concept string_elem_utf = is_one_of<std::remove_cvref_t<tchar>, char8_t, char16_t, char32_t, wchar_t>;



	/// @brief wchar_t to charXX_t
	template < string_elem tchar >
	struct as_utf {
		using type = tchar;
	};
	template <>
	struct as_utf<wchar_t> {
		using type = std::conditional_t<
			sizeof(wchar_t) == sizeof(char16_t),
			char16_t,
			std::conditional_t<sizeof(wchar_t) == sizeof(char32_t), char32_t, char>
		>;
	};
	template < string_elem tchar >
	using as_utf_t = typename as_utf<tchar>::type;

	/// @brief charXX_t to wchar_t
	template < string_elem tchar >
	struct as_wide {
		using type = tchar;
	};
	template <>
	struct as_wide< as_utf_t<wchar_t> > {
		using type = wchar_t;
	};
	template < string_elem tchar >
	using as_wide_t = typename as_wide<tchar>::type;


	/// @brief check if the type is same. (ex, char16_t == wchar_t for windows)
	template < typename tchar1, typename tchar2 >
	concept is_same_utf = (
		std::is_same_v<tchar1, tchar2>
		or (std::is_same_v<as_utf_t<tchar1>, as_utf_t<tchar2>>)
		);

	template < template < typename tchar > typename tstring_or_view, typename tchar >
	concept string_like = std::ranges::contiguous_range<tstring_or_view<tchar>> && string_elem<tchar>;
	//template < template < typename tchar > typename tstring_or_view, typename tchar >
	//concept string_like = requires (tstring_or_view<tchar> str) { str.data(); str.size(); };

	template < typename tstring_or_view >
	concept tchar_string_like = std::ranges::contiguous_range<tstring_or_view> && string_elem<std::ranges::range_value_t<tstring_or_view>>;
	//template < typename tstring_or_view, typename tchar = tstring_or_view::value_type >
	//concept tchar_string_like = requires (tstring_or_view str) { str.data(); str.size(); };


	template < tchar_string_like tstring >
	using value_t = std::ranges::range_value_t<std::remove_cvref_t<tstring>>;

	template < tchar_string_like tstringA, tchar_string_like tstrinB >
	constexpr bool have_same_tchar = (sizeof(value_t<tstringA>) == sizeof(value_t<tstrinB>));


	/// @brief json container. not completed.
	template < typename tjson >
	concept json_like_container = requires (tjson j, char const* psz, int index) {
		// todo : complete json_like_container
		j[psz];
		j[index];
	};

}
