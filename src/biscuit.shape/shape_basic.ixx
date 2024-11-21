module;

//////////////////////////////////////////////////////////////////////
//
// shape.ixx:
//
// PWH
// 2021.05.28
// 2021.08.11 Compare
// 2024-10-10. biscuit
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

export module biscuit.shape_basic;
import std;
import biscuit;

export namespace biscuit::shape {

	// =============================================================================================================================
	// 3d READY but NOT all implemented in 3d. scaling is implemented in 2d only
	// =============================================================================================================================

	//using char_t = wchar_t;
	//using string_t = std::wstring;
	using char_t = char;	// UTF-8
	using string_t = std::basic_string<char_t>;
	using point_t = xPoint3d;
	//using rect_t = xRect3d;
	using rect_t = xBounds3d;
	using ct_t = xCoordTrans3d;
	struct line_t { point_t pt0, pt1; };

	//-------------------------------------------------------------------------
	template < typename T >
	struct TPolyPoint : public TPoint<T, 4, false> {
		using base_t = TPoint<T, 4, false>;
		using this_t = TPolyPoint;

		using base_t::base_t;

		T& Bulge() { return base_t::w; }
		T Bulge() const { return base_t::w; }
		auto operator <=> (TPolyPoint const&) const = default;
	};

	using polypoint_t = TPolyPoint<double>;

	using color_t = color_rgba_t;

	//constexpr color_t const CR_DEFAULT = ColorRGBA(255, 255, 255);

	//-------------------------------------------------------------------------
	enum class eSHAPE {
		none = -1,
		e3dface = 0,
		arc_xy,
		block,
		circle_xy,
		dimension,
		dimaligned,
		dimlinear,
		dimradial,
		dimdiametric,
		dimangular,
		dimangular3p,
		dimordinate,
		ellipse_xy,
		hatch,
		image,
		insert,
		leader,
		line,
		lwpolyline,
		mtext,
		dot,
		polyline,
		ray,
		solid,
		spline,
		text,
		trace,
		underlay,
		vertex,
		viewport,
		xline,

		group = 126,
		layer = 127,
		drawing = 128,

		user_defined_1 = 256,
		user_defined_2,
		user_defined_3,
		user_defined_4,
		user_defined_5,
		user_defined_6,
		user_defined_7,
		user_defined_8,
		user_defined_9,
		user_defined_10,
		user_defined_11,
		user_defined_12,
	};

	//-------------------------------------------------------------------------
	struct sCookie {
		void* ptr{};
		std::vector<uint8_t> buffer;
		string_t str;
		std::chrono::nanoseconds duration{};

		GLZ_LOCAL_META(sCookie, ptr, buffer, str, duration);

		auto operator <=> (sCookie const&) const = default;

		template < typename archive >
		void serialize(archive& ar, [[maybe_unused]] std::uint32_t const file_version) {
			ar((ptrdiff_t&)ptr, buffer, str, duration);
		};
	};

	//-------------------------------------------------------------------------
	struct sLineType {
		string_t name;
		enum class eFLAG { xref = 16, xref_resolved = xref|32, modified_internal = 64 /* auto-cad internal command */ };
		int flags{};
		string_t description;
		std::vector<double> path;

		auto operator <=> (sLineType const&) const = default;

		template < typename archive >
		void serialize(archive& ar, [[maybe_unused]] std::uint32_t const file_version) {
			ar(name, flags, path, description);
		}
	};

	bool CohenSutherlandLineClip(xBounds2d roi, xBounds2d& pt0, xBounds2d& pt1);

}

export CEREAL_CLASS_VERSION(biscuit::shape::sCookie, 1);
export CEREAL_CLASS_VERSION(biscuit::shape::sLineType, 1);

