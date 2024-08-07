module;

#include <version>
#include <iterator>
#include <ranges>
#include <fmt/core.h>

#define EXPORT_DLL __declspec(dllexport)

export module biscuit;

// ssize_t
#ifdef __cpp_size_t_suffix
#else
#pragma warning(disable: 4455)
namespace std {
	export using ssize_t = ptrdiff_t;
}
using std::ssize_t;
namespace std::inline literals {
	export constexpr std::ssize_t operator "" z ( unsigned long long n ) { return n; }
	export constexpr std::size_t operator "" uz ( unsigned long long n ) { return n; }
}
#endif

// std::views::enumerate
// https://www.reedbeta.com/blog/python-like-enumerate-in-cpp17/
namespace biscuit::views {
	export
	template <typename T,
		typename TIter = decltype(std::begin(std::declval<T>())),
		typename = decltype(std::end(std::declval<T>()))>
	constexpr auto enumerate(T && iterable) {
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
}

namespace biscuit::views {
	export
	template < std::integral size_type, typename T, typename TIter = decltype(std::begin(std::declval<T>())), typename = decltype(std::end(std::declval<T>())) >
	constexpr auto enumerate_as(T&& iterable) {
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
}

export namespace biscuit {
	EXPORT_DLL bool Print() {
		fmt::print("hello, biscuit!\n");
		return true;
	}
}
