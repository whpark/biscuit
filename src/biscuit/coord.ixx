module;

//////////////////////////////////////////////////////////////////////
//
// coord.h:	좌표 관련 struct.
//
// PWH
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"

export module biscuit.coord;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.misc;
import biscuit.convert_codepage;
import biscuit.string;
export import biscuit.coord.concepts;
export import biscuit.coord.base;
export import biscuit.coord.point;
export import biscuit.coord.size;
export import biscuit.coord.rect;
export import biscuit.coord.bounds;
//export import biscuit.coord.bounds;
export import biscuit.coord.transform;
//export import :coord.trans.perspective;

//=============================================================================
//
// CAUTION : when 2d variable is assigned to 3d variable, Z var axis is not changed.
// 주의 : 3차원 변수에 2차원 변수를 대입할 경우, Z 축 은 값을 변경시키지 않도록 했음.
//       (ex, XPoint3d pt3(1, 2, 3); XPoint2d pt2(4, 5); pt3 = pt2; assert(pt3 == XPoint3d(4, 5, 3)); )
//

using namespace std::literals;

export namespace biscuit {
#pragma pack(push, 8)

	//-------------------------------------------------------------------------
	// to Text, From Text
	template < typename tchar = char, concepts::coord::generic_coord tcoord >
		requires std::is_trivially_copyable_v<tcoord>
	std::basic_string<tchar> ToString(tcoord const& coord) {
		using value_t = concepts::coord::value_t<tcoord>;
		constexpr auto dim = sizeof(tcoord) / sizeof(value_t);
		using array_t = std::array<value_t, dim>;
		static_assert(array_t().size() == concepts::coord::CountCoordMemberVariable<tcoord>());
		return biscuit::ToString<char, value_t>(reinterpret_cast<array_t const&>(coord));
	}
	template < typename tchar = char, concepts::coord::generic_coord tcoord >
		requires std::is_trivially_copyable_v<tcoord>
	std::basic_string<tchar> ToString(tcoord const& coord, std::basic_string_view<tchar> format) {
		using value_t = concepts::coord::value_t<tcoord>;
		constexpr auto dim = sizeof(tcoord) / sizeof(value_t);
		using array_t = std::array<value_t, dim>;
		static_assert(array_t().size() == concepts::coord::CountCoordMemberVariable<tcoord>());

		std::basic_string<tchar> str;
		auto const& arr = reinterpret_cast<array_t const&>(coord);
		for (int i = 0; i < dim; i++) {
			str += std::vformat(format, std::make_format_args(arr[i]));
			if (i < dim - 1)
				str += ',';
		}
		return str;
	}
	template < concepts::coord::generic_coord tcoord, concepts::tstring_like tstring >
		requires std::is_trivially_copyable_v<tcoord>
	tcoord FromString(tstring const& sv) {
		using value_t = concepts::coord::value_t<tcoord>;
		constexpr auto dim = sizeof(tcoord) / sizeof(value_t);
		using array_t = std::array<value_t, dim>;

		tcoord coord{};
		auto& arr = reinterpret_cast<array_t&>(coord);
		static_assert(array_t().size() == concepts::coord::CountCoordMemberVariable<tcoord>());

		int i{};
		for (auto item : SplitView(sv, ',')) {
			if (i >= dim)
				break;
			arr[i++] = tszto<value_t>(item, 0);
		}
		return coord;
	}


	/// @brief Round
	template < concepts::coord::is_coord tcoord >
	BSC__NODISCARD constexpr tcoord Round(tcoord const& coord) {
		using namespace biscuit::concepts::coord;
		tcoord v;
		if constexpr (has_x<tcoord>) { v.x = std::round(coord.x); }
		if constexpr (has_y<tcoord>) { v.y = std::round(coord.y); }
		if constexpr (has_z<tcoord>) { v.z = std::round(coord.z); }
		if constexpr (has_w<tcoord>) { v.w = std::round(coord.w); }
		if constexpr (has_width<tcoord>) { v.width = std::round(coord.width); }
		if constexpr (has_height<tcoord>) { v.height = std::round(coord.height); }
		if constexpr (has_depth<tcoord>) { v.depth = std::round(coord.depth); }
		return v;
	}

	/// @brief Ceil
	template < concepts::coord::is_coord tcoord >
	BSC__NODISCARD constexpr tcoord Ceil(tcoord const& coord) {
		using namespace biscuit::concepts::coord;
		tcoord v;
		if constexpr (has_x<tcoord>) { v.x = std::ceil(coord.x); }
		if constexpr (has_y<tcoord>) { v.y = std::ceil(coord.y); }
		if constexpr (has_z<tcoord>) { v.z = std::ceil(coord.z); }
		if constexpr (has_w<tcoord>) { v.w = std::ceil(coord.w); }
		if constexpr (has_width<tcoord>) { v.width = std::ceil(coord.width); }
		if constexpr (has_height<tcoord>) { v.height = std::ceil(coord.height); }
		if constexpr (has_depth<tcoord>) { v.depth = std::ceil(coord.depth); }
		return v;
	}

	/// @brief Floor
	template < concepts::coord::is_coord tcoord >
	BSC__NODISCARD constexpr tcoord Floor(tcoord const& coord) {
		using namespace biscuit::concepts::coord;
		tcoord v{};
		if constexpr (has_x<tcoord>) { v.x = std::floor(coord.x); }
		if constexpr (has_y<tcoord>) { v.y = std::floor(coord.y); }
		if constexpr (has_z<tcoord>) { v.z = std::floor(coord.z); }
		if constexpr (has_w<tcoord>) { v.w = std::floor(coord.w); }
		if constexpr (has_width<tcoord>) { v.width = std::floor(coord.width); }
		if constexpr (has_height<tcoord>) { v.height = std::floor(coord.height); }
		if constexpr (has_depth<tcoord>) { v.depth = std::floor(coord.depth); }
		return v;
	}

	//-------------------------------------------------------------------------
	// Check ROI

	template < concepts::coord::is_rect2 trect, concepts::coord::is_size2 tsize >
	bool IsROI_Valid(trect const& rcROI, tsize const& sizeImage) {
		return 1
			&& ( (rcROI.x >= 0) && (rcROI.y >= 0) )
			&& ( (rcROI.width > 0) && (rcROI.height > 0) )
			&& ( (sizeImage.width < 0) || ((rcROI.x < sizeImage.width) && (rcROI.x+rcROI.width <= sizeImage.width)) )
			&& ( (sizeImage.height < 0) || ((rcROI.y < sizeImage.height) && (rcROI.y+rcROI.height <= sizeImage.height)) )
			;
	};

	template < concepts::coord::is_rect2 trect, concepts::coord::is_size2 tsize >
	BSC__NODISCARD trect GetSafeROI(trect const& rc, tsize const& sizeImage) {
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
