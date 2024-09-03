module;

//////////////////////////////////////////////////////////////////////
//
// coord.h:	좌표 관련 struct.
//
// PWH
//
//////////////////////////////////////////////////////////////////////
#include <fmt/core.h>
#include <fmt/xchar.h>
#include <fmt/ostream.h>

export module biscuit.coord;
import std;
//export import <glm/glm.hpp>;
import biscuit.aliases;
import biscuit.concepts;
export import biscuit.coord.concepts;
export import biscuit.coord.base;
export import biscuit.coord.point;
export import biscuit.coord.size;
export import biscuit.coord.rect;
//export import :coord.point;
//export import :coord.size;
//export import :coord.rect;
//export import :coord.srect;
//export import :coord.trans;
//export import :coord.trans.perspective;

//=============================================================================
//
// 참고 : 3차원 변수에 2차원 변수를 대입할 경우, Z 축 은 값을 변경시키지 않도록 했음.
//       (ex, XPoint3d pt3(1, 2, 3); XPoint2d pt2(4, 5); pt3 = pt2; assert(pt3 == XPoint3d(4, 5, 3)); )
//


export namespace biscuit {
#pragma pack(push, 8)

	/// @brief Interpolation (lerp)
	template < typename T, int dim >
	coord::TPoint_<T, dim> lerp(coord::TPoint_<T, dim> const& a, coord::TPoint_<T, dim> const& b, double t) {
		coord::TPoint_<T, dim> c;
		for (int i = 0; i < dim; i++)
			c.member(i) = std::lerp(a.member(i), b.member(i), t);
		return c;
	}


	////-------------------------------------------------------------------------
	//// Archive <---------> TPoint2/3, TSize2/3, TRect2/3
	////
	//template < typename Archive, concepts::coord tcoord >
	//Archive& Coord2Archive(const tcoord& coord, Archive& ar) {
	//	for (const auto& v : coord.data())
	//		ar & v;
	//	return ar;
	//}

	//template < typename Archive, concepts::coord tcoord >
	//Archive& Archive2Coord(Archive& ar, tcoord& coord) {
	//	for (auto& v : coord.data())
	//		ar & v;
	//	return ar;
	//}

	//template < typename Archive, concepts::coord tcoord >
	//Archive& SerializeCoord(Archive& ar, tcoord& coord) {
	//	return ar.IsStoring() ? Coord2Archive<Archive, tcoord>(coord, ar) : Archive2Coord<Archive, tcoord>(ar, coord);
	//}

	//-------------------------------------------------------------------------
	// to Text, From Text

	template < concepts::string_elem tchar, concepts::arithmetic tvalue >
	constexpr fmt::basic_format_string<tchar, tvalue> GetDefaultFormatSpecifier() {
		using namespace std::literals;
		if constexpr (std::is_same_v<tchar, char>) {
			return "{}";
		}
		else if constexpr (std::is_same_v<tchar, wchar_t>) {
			return L"{}";
		}
		else if constexpr (std::is_same_v<tchar, char8_t>) {
			return u8"{}";
		}
		else if constexpr (std::is_same_v<tchar, char16_t>) {
			return u"{}";
		}
		else if constexpr (std::is_same_v<tchar, char32_t>) {
			return U"{}";
		}
		else {
			static_assert(false);
		}
	}

	////-------------------------------------------------------------------------
	//// to Text, From Text
	//template < typename tchar, concepts::is_coord tcoord >
	//std::basic_string<tchar> ToString(tcoord const& coord, fmt::basic_format_string<tchar, typename tcoord::value_type> const& svFMT = GetDefaultFormatSpecifier<tchar, typename tcoord::value_type>()) {
	//	std::basic_string<tchar> str;
	//	//if ( !(fmt::basic_string_view<tchar>(svFMT)).data() )
	//	//	svFMT = GetDefaultFormatSpecifier<tchar, typename tcoord::value_type>();

	//	for (size_t i = 0; i < coord.arr().size(); i++) {
	//		if (i)
	//			str += ',';
	//		str += fmt::vformat((fmt::basic_string_view<tchar>)svFMT, fmt::make_format_args<fmt::buffer_context<tchar>>(coord[i]));
	//	}
	//	return str;
	//}
	//template < concepts::is_coord tcoord, typename tchar >
	//tcoord FromString(std::basic_string_view<tchar> sv) {
	//	tcoord coord;
	//	tchar const* pos = sv.data();
	//	tchar const* const end = sv.data() + sv.size();
	//	for (auto& v : coord.arr()) {
	//		if (pos >= end)
	//			break;
	//		v = tszto<typename tcoord::value_type>(pos, end, &pos);
	//		if (*pos == ',') pos++;	// or....... if (*pos) pos++
	//	}
	//	return coord;
	//}


	/// @brief Round for concepts::generic_coord
	template < concepts::generic_coord tcoord >
	constexpr [[nodiscard]] tcoord Round(tcoord const& coord) {
		tcoord v;
		if constexpr (concepts::has_x<tcoord>) { v.x = std::round(coord.x); }
		if constexpr (concepts::has_y<tcoord>) { v.y = std::round(coord.y); }
		if constexpr (concepts::has_z<tcoord>) { v.z = std::round(coord.z); }
		if constexpr (concepts::has_w<tcoord>) { v.w = std::round(coord.w); }
		if constexpr (concepts::has_width<tcoord>) { v.width = std::round(coord.width); }
		if constexpr (concepts::has_height<tcoord>) { v.height = std::round(coord.height); }
		if constexpr (concepts::has_depth<tcoord>) { v.depth = std::round(coord.depth); }
		return v;
	}

	/// @brief Ceil for concepts::generic_coord
	template < concepts::generic_coord tcoord >
	constexpr [[nodiscard]] tcoord Ceil(tcoord const& coord) {
		tcoord v;
		if constexpr (concepts::has_x<tcoord>) { v.x = std::ceil(coord.x); }
		if constexpr (concepts::has_y<tcoord>) { v.y = std::ceil(coord.y); }
		if constexpr (concepts::has_z<tcoord>) { v.z = std::ceil(coord.z); }
		if constexpr (concepts::has_w<tcoord>) { v.w = std::ceil(coord.w); }
		if constexpr (concepts::has_width<tcoord>) { v.width = std::ceil(coord.width); }
		if constexpr (concepts::has_height<tcoord>) { v.height = std::ceil(coord.height); }
		if constexpr (concepts::has_depth<tcoord>) { v.depth = std::ceil(coord.depth); }
		return v;
	}

	/// @brief Floor for concepts::generic_coord
	template < concepts::generic_coord tcoord >
	constexpr [[nodiscard]] tcoord Floor(tcoord const& coord) {
		tcoord v{};
		if constexpr (concepts::has_x<tcoord>) { v.x = std::floor(coord.x); }
		if constexpr (concepts::has_y<tcoord>) { v.y = std::floor(coord.y); }
		if constexpr (concepts::has_z<tcoord>) { v.z = std::floor(coord.z); }
		if constexpr (concepts::has_w<tcoord>) { v.w = std::floor(coord.w); }
		if constexpr (concepts::has_width<tcoord>) { v.width = std::floor(coord.width); }
		if constexpr (concepts::has_height<tcoord>) { v.height = std::floor(coord.height); }
		if constexpr (concepts::has_depth<tcoord>) { v.depth = std::floor(coord.depth); }
		return v;
	}

	//-------------------------------------------------------------------------
	// Check ROI

	template < concepts::is_rect2 trect, concepts::is_size2 tsize >
	bool IsROI_Valid(trect const& rcROI, tsize const& sizeImage) {
		return 1
			&& ( (rcROI.x >= 0) && (rcROI.y >= 0) )
			&& ( (rcROI.width > 0) && (rcROI.height > 0) )
			&& ( (sizeImage.width < 0) || ((rcROI.x < sizeImage.width) && (rcROI.x+rcROI.width <= sizeImage.width)) )
			&& ( (sizeImage.height < 0) || ((rcROI.y < sizeImage.height) && (rcROI.y+rcROI.height <= sizeImage.height)) )
			;
	};

	template < concepts::is_rect2 trect, concepts::is_size2 tsize >
	[[nodiscard]] trect GetSafeROI(trect const& rc, tsize const& sizeImage) {
		trect rcSafe(rc);

		if (rcSafe.width < 0) {
			rcSafe.x += rcSafe.width;
			rcSafe.width = -rcSafe.width;
		}
		if (rcSafe.height < 0) {
			rcSafe.y += rcSafe.height;
			rcSafe.height = -rcSafe.height;
		}

		if (rcSafe.x < 0)
			rcSafe.x = 0;
		if (rcSafe.y < 0)
			rcSafe.y = 0;

		// Error
		if ( (rcSafe.x >= sizeImage.width) || (rcSafe.y >= sizeImage.height) ) {
			rcSafe = trect{};
			return rcSafe;
		}
		if ( (sizeImage.width > 0) && (rcSafe.x + rcSafe.width > sizeImage.width) )
			rcSafe.width = sizeImage.width - rcSafe.x;
		if ( (sizeImage.height > 0) && (rcSafe.y + rcSafe.height > sizeImage.height) )
			rcSafe.height = sizeImage.height - rcSafe.y;

		// Error
		if ( (sizeImage.width <= 0) || (sizeImage.height <= 0) ) {
			rcSafe = trect{};
			return rcSafe;
		}

		return rcSafe;
	};

#pragma pack(pop)
}	// namespace gtl
