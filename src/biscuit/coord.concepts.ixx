module;

export module biscuit.coord.concepts;
import std;
import "biscuit/dependencies_eigen.hxx";
import biscuit.aliases;
import biscuit.concepts;

export namespace biscuit::coord {

	struct mPoint_t {};
	struct mSize_t {};
	struct mRect_t {};

}
export namespace biscuit::concepts::coord {

	//-----------------------------------------------------------------------------------------------------------------------------
	// for biscuit, opencv
	template < typename tcoord > concept has_x			= requires (tcoord a) { {a.x} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_y			= requires (tcoord a) { {a.y} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_z			= requires (tcoord a) { {a.z} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_w			= requires (tcoord a) { {a.w} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_point2		= has_x<tcoord> and has_y<tcoord>;
	template < typename tcoord > concept has_point3		= has_x<tcoord> and has_y<tcoord> and has_z<tcoord>;
	template < typename tcoord > concept has_point4		= has_x<tcoord> and has_y<tcoord> and has_z<tcoord> and has_w<tcoord>;

	template < typename tcoord > concept has_width		= requires (tcoord a) { {a.width } -> std::convertible_to<double>;	};
	template < typename tcoord > concept has_height		= requires (tcoord a) { {a.height} -> std::convertible_to<double>;	};
	template < typename tcoord > concept has_depth		= requires (tcoord a) { {a.depth } -> std::convertible_to<double>;	};
	template < typename tcoord > concept has_size2		= has_width<tcoord> and has_height<tcoord>;
	template < typename tcoord > concept has_size3		= has_width<tcoord> and has_height<tcoord> and has_depth<tcoord>;

	template < typename tcoord > concept has_rect2		= has_point2<tcoord> and has_size2<tcoord>;
	template < typename tcoord > concept has_rect3		= has_point3<tcoord> and has_size3<tcoord>;

	// bounds - rect with l,t,r,b. (NOT one of is_coord)
	template < typename tcoord > concept has_l			= requires (tcoord a) { {a.l} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_t			= requires (tcoord a) { {a.t} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_r			= requires (tcoord a) { {a.r} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_b			= requires (tcoord a) { {a.b} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_f			= requires (tcoord a) { {a.f} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_bk			= requires (tcoord a) { {a.bk} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_bounds2	= has_l<tcoord> and has_t<tcoord> and has_r<tcoord> and has_b<tcoord>;
	template < typename tcoord > concept has_bounds3	= has_l<tcoord> and has_t<tcoord> and has_r<tcoord> and has_b<tcoord> and has_f<tcoord> and has_bk<tcoord>;

	// all concepts is_... are loosely defined.
	template < typename tcoord > concept is_point2		= has_point2<tcoord> and !has_z<tcoord> and !has_w<tcoord> and !has_rect2<tcoord>;
	template < typename tcoord > concept is_point3		= has_point3<tcoord> and !has_w<tcoord>					   and !has_rect2<tcoord>;
	template < typename tcoord > concept is_point4		= has_point4<tcoord>									   and !has_rect2<tcoord>;
	template < typename tcoord > concept is_point_		= is_point2<tcoord> or is_point3<tcoord> or is_point4<tcoord>;

	template < typename tcoord > concept is_size2		= has_size2<tcoord> and !has_depth<tcoord> and !has_rect2<tcoord>;
	template < typename tcoord > concept is_size3		= has_size3<tcoord>						   and !has_rect2<tcoord>;
	template < typename tcoord > concept is_size_		= is_size2<tcoord> or is_size3<tcoord>;

	template < typename tcoord > concept is_rect2		= has_rect2<tcoord> and !has_rect3<tcoord>;
	template < typename tcoord > concept is_rect3		= has_rect3<tcoord>;
	template < typename tcoord > concept is_rect_		= is_rect2<tcoord> or is_rect3<tcoord>;

	template < typename tcoord > concept is_bsc_coord	= is_one_of<typename tcoord::coord_type_t, biscuit::coord::mPoint_t, biscuit::coord::mSize_t, biscuit::coord::mRect_t>;
	template < typename tcoord > concept is_bsc_point2	= is_bsc_coord<tcoord> and has_point2<tcoord> and !has_z<tcoord> and !has_w<tcoord> and !has_rect2<tcoord>;
	template < typename tcoord > concept is_bsc_point3	= is_bsc_coord<tcoord> and has_point3<tcoord> and !has_w<tcoord>					and !has_rect2<tcoord>;
	template < typename tcoord > concept is_bsc_point4	= is_bsc_coord<tcoord> and has_point4<tcoord>										and !has_rect2<tcoord>;
	template < typename tcoord > concept is_bsc_point_	= is_bsc_point2<tcoord> or is_bsc_point3<tcoord> or is_bsc_point4<tcoord>;
	template < typename tcoord > concept is_bsc_size2	= is_bsc_coord<tcoord> and has_size2<tcoord> and !has_depth<tcoord> and !has_rect2<tcoord>;
	template < typename tcoord > concept is_bsc_size3	= is_bsc_coord<tcoord> and has_size3<tcoord>						and !has_rect2<tcoord>;
	template < typename tcoord > concept is_bsc_size_	= is_bsc_size2<tcoord> or is_bsc_size3<tcoord>;
	template < typename tcoord > concept is_bsc_rect2	= is_bsc_coord<tcoord> and has_rect2<tcoord> and !has_rect3<tcoord>;
	template < typename tcoord > concept is_bsc_rect3	= is_bsc_coord<tcoord> and has_rect3<tcoord>;
	template < typename tcoord > concept is_bsc_rect_	= is_bsc_rect2<tcoord> or is_bsc_rect3<tcoord>;

	template < typename tcoord > concept is_coord		= has_point2<tcoord> or has_size2<tcoord>;

	//-----------------------------------------------------------------------------------------------------------------------------
	// for invokable x, y, ... (for QT)
	template < typename tcoord > concept has_ix			= requires (tcoord a) { {a.x()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_iy			= requires (tcoord a) { {a.y()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_iz			= requires (tcoord a) { {a.z()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_iw			= requires (tcoord a) { {a.w()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_ipoint2	= has_ix<tcoord> and has_iy<tcoord>;
	template < typename tcoord > concept has_ipoint3	= has_ix<tcoord> and has_iy<tcoord> and has_iz<tcoord>;
	template < typename tcoord > concept has_ipoint4	= has_ix<tcoord> and has_iy<tcoord> and has_iz<tcoord> and has_iw<tcoord>;
	template < typename tcoord > concept has_iwidth		= requires (tcoord a) { {a.width() } -> std::convertible_to<double>; };
	template < typename tcoord > concept has_iheight	= requires (tcoord a) { {a.height()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_idepth		= requires (tcoord a) { {a.depth() } -> std::convertible_to<double>; };
	template < typename tcoord > concept has_isize2		= has_iwidth<tcoord> and has_iheight<tcoord>;
	template < typename tcoord > concept has_isize3		= has_iwidth<tcoord> and has_iheight<tcoord> and has_idepth<tcoord>;
	template < typename tcoord > concept has_irect2		= has_ipoint2<tcoord> and has_isize2<tcoord>;
	template < typename tcoord > concept has_irect3		= has_ipoint3<tcoord> and has_isize3<tcoord>;
	// all concepts is_i... are loosely defined.
	template < typename tcoord > concept is_ipoint2		= has_ipoint2<tcoord> and !has_iz<tcoord> and !has_iw<tcoord> and !has_irect2<tcoord>;
	template < typename tcoord > concept is_ipoint3		= has_ipoint3<tcoord> and !has_iw<tcoord>					  and !has_irect2<tcoord>;
	template < typename tcoord > concept is_ipoint4		= has_ipoint4<tcoord>										  and !has_irect2<tcoord>;
	template < typename tcoord > concept is_ipoint_		= is_ipoint2<tcoord> or is_ipoint3<tcoord> or is_ipoint4<tcoord>;
	template < typename tcoord > concept is_isize2		= has_isize2<tcoord> and !has_idepth<tcoord> and !has_irect2<tcoord>;
	template < typename tcoord > concept is_isize3		= has_isize3<tcoord>						 and !has_irect2<tcoord>;
	template < typename tcoord > concept is_isize_		= is_isize2<tcoord> or is_isize3<tcoord>;
	template < typename tcoord > concept is_irect2		= has_irect2<tcoord> and !has_irect3<tcoord>;
	template < typename tcoord > concept is_irect3		= has_irect3<tcoord>;
	template < typename tcoord > concept is_irect_		= is_irect2<tcoord> or is_irect3<tcoord>;

	template < typename tcoord > concept is_icoord		= has_ipoint2<tcoord> or has_isize2<tcoord>;

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tcoord > concept generic_point	= is_point_<tcoord> or is_ipoint_<tcoord>;
	template < typename tcoord > concept generic_size	= is_size_<tcoord> or is_isize_<tcoord>;
	template < typename tcoord > concept generic_rect	= is_rect_<tcoord> or is_irect_<tcoord>;
	template < typename tcoord > concept generic_coord	= is_coord<tcoord> or is_icoord<tcoord>;

	//-----------------------------------------------------------------------------------------------------------------------------
	template < generic_coord tcoord >
	using coord_type_t =
		std::conditional_t<generic_point<tcoord>, biscuit::coord::mPoint_t,
		std::conditional_t<generic_size<tcoord>, biscuit::coord::mSize_t,
		std::conditional_t<generic_rect<tcoord>, biscuit::coord::mRect_t, void>>>;

	namespace detail {
		template < generic_coord tcoord >
		auto GetFirstCoordValue() {
			if constexpr (has_x<tcoord>) { return tcoord().x; }
			else if constexpr (has_width<tcoord>) { return tcoord().width; }
			else if constexpr (has_ix<tcoord>) { return tcoord().x(); }
			else if constexpr (has_iwidth<tcoord>) { return tcoord().width(); }
			else { static_assert(false); }
		}
	}

	template < is_coord tcoord >
	constexpr int CountCoordMemberVariable() {
		int nCount{};
		if constexpr (has_x<tcoord>) {		nCount++; }
		if constexpr (has_y<tcoord>) { 		nCount++; }
		if constexpr (has_z<tcoord>) { 		nCount++; }
		if constexpr (has_w<tcoord>) { 		nCount++; }
		if constexpr (has_width<tcoord>) { 	nCount++; }
		if constexpr (has_height<tcoord>) {	nCount++; }
		if constexpr (has_depth<tcoord>) {	nCount++; }
		return nCount;
	}

	template < generic_coord tcoord >
	using value_t = std::remove_cvref_t<decltype(detail::GetFirstCoordValue<tcoord>())>;
}

