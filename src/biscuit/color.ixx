export module biscuit:color;
import std.compat;
import :concepts;

export namespace biscuit {

	/// @brief color_rgba_t
	union color_rgba_t {
		struct { uint8_t r, g, b, a; };
		uint32_t cr{};

		using this_t = color_rgba_t;

		this_t& operator = (this_t const& B) { cr = B.cr; return *this; }
		bool operator == (this_t const& B) const { return cr == B.cr; }
		auto operator <=> (this_t const& B) const { return cr <=> B.cr; }
		template < typename tjson >
		friend void from_json(tjson const& j, this_t& color) {
			color.cr = (uint32_t)(int64_t)j;
		}
		template < typename tjson >
		friend void to_json(tjson& j, this_t const& color) {
			j = (int64_t)color.cr;
		}

		template < typename archive >
		friend void serialize(archive& ar, this_t& cr, unsigned int const file_version) {
			ar & cr.cr;
		}
		template < typename archive >
		friend archive& operator & (archive& ar, this_t& cr) {
			ar & cr.cr;
			return ar;
		}
	};
	constexpr inline color_rgba_t ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = {}) {
		return color_rgba_t{.r = r, .g = g, .b = b, .a = a};
	}

	/// @brief color_bgra_t
	union color_bgra_t {
		struct { uint8_t b, g, r, a; };
		uint32_t cr{};

		using this_t = color_bgra_t;

		this_t& operator = (this_t const& B) { cr = B.cr; return *this; }
		bool operator == (this_t const& B) const { return cr == B.cr; }
		auto operator <=> (this_t const& B) const { return cr <=> B.cr; }
		template < typename tjson >
		friend void from_json(tjson const& j, this_t& color) {
			color.cr = (uint32_t)(int64_t)j;
		}
		template < typename tjson >
		friend void to_json(tjson& j, this_t const& color) {
			j = (int64_t)color.cr;
		}

		template < typename archive >
		friend void serialize(archive& ar, this_t& cr, unsigned int const file_version) {
			ar & cr.cr;
		}
		template < typename archive >
		friend archive& operator & (archive& ar, this_t& cr) {
			ar & cr.cr;
			return ar;
		}
	};
	constexpr inline color_bgra_t ColorBGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a = {}) {
		return color_bgra_t{ .b = b, .g = g, .r = r, .a = a };
	}

	/// @brief color_abgr_t
	union color_abgr_t {
		struct { uint8_t a, b, g, r; };
		uint32_t cr{};

		using this_t = color_abgr_t;

		this_t& operator = (this_t const& B) { cr = B.cr; return *this; }
		bool operator == (this_t const& B) const { return cr == B.cr; }
		auto operator <=> (this_t const& B) const { return cr <=> B.cr; }
		template < typename tjson >
		friend void from_json(tjson const& j, this_t& color) {
			color.cr = (uint32_t)(int64_t)j;
		}
		template < typename tjson >
		friend void to_json(tjson& j, this_t const& color) {
			j = (int64_t)color.cr;
		}

		template < typename archive >
		friend void serialize(archive& ar, this_t& cr, unsigned int const file_version) {
			ar & cr.cr;
		}
		template < typename archive >
		friend archive& operator & (archive& ar, this_t& cr) {
			ar & cr.cr;
			return ar;
		}
	};
	constexpr inline color_abgr_t ColorABGR(uint8_t b, uint8_t g, uint8_t r, uint8_t a = {}) {
		return color_abgr_t{ .a = a, .b = b, .g = g, .r = r };
	}

	//-----------------------------------------------------------------------------
	constexpr inline color_rgba_t ColorRGBA(color_bgra_t cr) {
		return color_rgba_t{ .r = cr.r, .g = cr.g, .b = cr.b, .a = cr.a };
	}
	constexpr inline color_rgba_t ColorRGBA(color_abgr_t cr) {
		return color_rgba_t{ .r = cr.r, .g = cr.g, .b = cr.b, .a = cr.a };
	}
	constexpr inline color_bgra_t ColorBGRA(color_rgba_t cr) {
		return color_bgra_t{ .b = cr.b, .g = cr.g, .r = cr.r, .a = cr.a };
	}
	constexpr inline color_bgra_t ColorBGRA(color_abgr_t cr) {
		return color_bgra_t{ .b = cr.b, .g = cr.g, .r = cr.r, .a = cr.a };
	}
	constexpr inline color_abgr_t ColorABGR(color_rgba_t cr) {
		return color_abgr_t{ .a = cr.a, .b = cr.b, .g = cr.g, .r = cr.r };
	}
	constexpr inline color_abgr_t ColorABGR(color_bgra_t cr) {
		return color_abgr_t{ .a = cr.a, .b = cr.b, .g = cr.g, .r = cr.r };
	}

}
