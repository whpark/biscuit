module;

#include <glaze/glaze.hpp>
#include "biscuit/macro.h"

export module biscuit.color;
import std;

export namespace biscuit {

	//-----------------------------------------------------------------------------
	/// @brief color_rgba_t
	struct color_rgba_t {
		using this_t = color_rgba_t;

		uint8_t r, g, b, a;

		auto operator <=> (this_t const&) const = default;

		uint32_t& Value()		{ return *(uint32_t*)this; }
		uint32_t  Value() const	{ return *(uint32_t const*)this; }
	protected:
		uint32_t& ValueAll()	{ return *(uint32_t*)this; }	// for glaze only
	public:
		struct glaze {
			using T = this_t;
			static constexpr auto value = glz::object(&T::ValueAll);
		};

	public:
		void serialize(auto& ar, unsigned int const file_version = 0) {
			ar & Value();
		}
	};
	BSC__NODISCARD constexpr inline auto ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = {}) -> color_rgba_t {
		return color_rgba_t{.r = r, .g = g, .b = b, .a = a};
	}

	//-----------------------------------------------------------------------------
	/// @brief color_bgra_t
	struct color_bgra_t {
		using this_t = color_bgra_t;

		uint8_t b, g, r, a;

		auto operator <=> (this_t const&) const = default;

		uint32_t& Value()		{ return *(uint32_t*)this; }
		uint32_t  Value() const	{ return *(uint32_t const*)this; }
	protected:
		uint32_t& ValueAll()	{ return *(uint32_t*)this; }	// for glaze only
	public:
		struct glaze {
			using T = this_t;
			static constexpr auto value = glz::object(&T::ValueAll);
		};

	public:
		void serialize(auto& ar, unsigned int const file_version = 0) {
			ar & Value();
		}
	};
	BSC__NODISCARD constexpr inline auto ColorBGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a = {}) -> color_bgra_t {
		return color_bgra_t{.b = b, .g = g, .r = r, .a = a};
	}

	//-----------------------------------------------------------------------------
	/// @brief color_abgr_t
	struct color_abgr_t {
		using this_t = color_abgr_t;

		uint8_t a, b, g, r;

		auto operator <=> (this_t const&) const = default;

		uint32_t& Value()		{ return *(uint32_t*)this; }
		uint32_t  Value() const	{ return *(uint32_t const*)this; }
	protected:
		uint32_t& ValueAll()	{ return *(uint32_t*)this; }	// for glaze only
	public:
		struct glaze {
			using T = this_t;
			static constexpr auto value = glz::object(&T::ValueAll);
		};

	public:
		void serialize(auto& ar, unsigned int const file_version = 0) {
			ar & Value();
		}
	};
	BSC__NODISCARD constexpr inline auto ColorABGR(uint8_t a, uint8_t b, uint8_t g, uint8_t r = {}) -> color_abgr_t {
		return color_abgr_t{.a = a, .b = b, .g = g, .r = r};
	}

	//-----------------------------------------------------------------------------
	BSC__NODISCARD constexpr inline auto ColorRGBA(color_bgra_t cr) { return color_rgba_t{ .r = cr.r, .g = cr.g, .b = cr.b, .a = cr.a }; }
	BSC__NODISCARD constexpr inline auto ColorRGBA(color_abgr_t cr) { return color_rgba_t{ .r = cr.r, .g = cr.g, .b = cr.b, .a = cr.a }; }
	BSC__NODISCARD constexpr inline auto ColorBGRA(color_rgba_t cr) { return color_bgra_t{ .b = cr.b, .g = cr.g, .r = cr.r, .a = cr.a }; }
	BSC__NODISCARD constexpr inline auto ColorBGRA(color_abgr_t cr) { return color_bgra_t{ .b = cr.b, .g = cr.g, .r = cr.r, .a = cr.a }; }
	BSC__NODISCARD constexpr inline auto ColorABGR(color_rgba_t cr) { return color_abgr_t{ .a = cr.a, .b = cr.b, .g = cr.g, .r = cr.r }; }
	BSC__NODISCARD constexpr inline auto ColorABGR(color_bgra_t cr) { return color_abgr_t{ .a = cr.a, .b = cr.b, .g = cr.g, .r = cr.r }; }

}
