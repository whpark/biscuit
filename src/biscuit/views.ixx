module;

#include "biscuit/macro.h"

export module biscuit.views;
import std;

export namespace bisucit::views {

//#ifndef __cpp_lib_ranges_enumerate
// std::views::enumerate
// https://www.reedbeta.com/blog/python-like-enumerate-in-cpp17/
	template <typename T,
		typename TIter = decltype(std::begin(std::declval<T>())),
		typename = decltype(std::end(std::declval<T>()))>
	BSC__NODISCARD constexpr auto enumerate(T && iterable) {
		struct iterator {
			size_t i;
			TIter iter;
			bool operator != (const iterator & other) const { return iter != other.iter; }
			void operator ++ () { ++i; ++iter; }
			auto operator * () const { return std::tie(i, *iter); }
		};
		struct iterable_wrapper {
			T iterable;
			auto begin() { return iterator{ 0, std::begin(iterable) }; }
			auto end() { return iterator{ 0, std::end(iterable) }; }
		};
		return iterable_wrapper{ std::forward<T>(iterable) };
	}
//#endif

	template < std::integral size_type, typename T, typename TIter = decltype(std::begin(std::declval<T>())), typename = decltype(std::end(std::declval<T>())) >
	BSC__NODISCARD constexpr auto enumerate_as(T&& iterable) {
		struct iterator {
			size_type i;
			TIter iter;
			bool operator != (iterator const& other) const { return iter != other.iter; }
			void operator ++ () { ++i; ++iter; }
			auto operator * () const { return std::tie(i, *iter); }
		};
		struct iterable_wrapper {
			T iterable;
			auto begin() { return iterator{ 0, std::begin(iterable) }; }
			auto end() { return iterator{ 0, std::end(iterable) }; }
		};
		return iterable_wrapper{ std::forward<T>(iterable) };
	}

}	// namespace biscuit::views
