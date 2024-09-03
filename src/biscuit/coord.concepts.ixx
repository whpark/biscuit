module;

export module biscuit.coord.concepts;
import std;
import biscuit.aliases;
import biscuit.concepts;

export namespace biscuit::coord {

	struct mPoint_t {};
	struct mSize_t {};
	struct mRect_t {};

}
export namespace biscuit::concepts {

	//-----------------------------------------------------------------------------------------------------------------------------
	// for biscuit, opencv
	template < typename tcoord > concept has_x		= requires (tcoord a) { {a.x} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_y		= requires (tcoord a) { {a.y} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_z		= requires (tcoord a) { {a.z} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_w		= requires (tcoord a) { {a.w} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_point2	= has_x<tcoord> and has_y<tcoord>;
	template < typename tcoord > concept has_point3	= has_x<tcoord> and has_y<tcoord> and has_z<tcoord>;
	template < typename tcoord > concept has_point4	= has_x<tcoord> and has_y<tcoord> and has_z<tcoord> and has_w<tcoord>;

	template < typename tcoord > concept has_width	= requires (tcoord a) { {a.width } -> std::convertible_to<double>;	};
	template < typename tcoord > concept has_height	= requires (tcoord a) { {a.height} -> std::convertible_to<double>;	};
	template < typename tcoord > concept has_depth	= requires (tcoord a) { {a.depth } -> std::convertible_to<double>;	};
	template < typename tcoord > concept has_size2	= has_width<tcoord> and has_height<tcoord>;
	template < typename tcoord > concept has_size3	= has_width<tcoord> and has_height<tcoord> and has_depth<tcoord>;

	template < typename tcoord > concept has_rect2	= has_point2<tcoord> and has_size2<tcoord>;
	template < typename tcoord > concept has_rect3	= has_point3<tcoord> and has_size3<tcoord>;


	// all concepts is_... are loosely defined.
	template < typename tcoord > concept is_point2	= has_point2<tcoord> and !has_z<tcoord> and !has_w<tcoord> and !has_rect2<tcoord>;
	template < typename tcoord > concept is_point3	= has_point3<tcoord> and !has_w<tcoord>					   and !has_rect2<tcoord>;
	template < typename tcoord > concept is_point4	= has_point4<tcoord>									   and !has_rect2<tcoord>;

	template < typename tcoord > concept is_size2	= has_size2<tcoord> and !has_depth<tcoord> and !has_rect2<tcoord>;
	template < typename tcoord > concept is_size3	= has_size3<tcoord>						   and !has_rect2<tcoord>;

	template < typename tcoord > concept is_rect2	= has_rect2<tcoord> and !has_rect3<tcoord>;
	template < typename tcoord > concept is_rect3	= has_rect3<tcoord>;

	template < typename tcoord > concept is_coord			= is_one_of<typename tcoord::coord_type_t, coord::mPoint_t, coord::mSize_t, coord::mRect_t>;
	template < typename tcoord > concept is_coord_point2	= is_coord<tcoord> and has_point2<tcoord> and !has_z<tcoord> and !has_w<tcoord> and !has_rect2<tcoord>;
	template < typename tcoord > concept is_coord_point3	= is_coord<tcoord> and has_point3<tcoord> and !has_w<tcoord>					and !has_rect2<tcoord>;
	template < typename tcoord > concept is_coord_point4	= is_coord<tcoord> and has_point4<tcoord>										and !has_rect2<tcoord>;
	template < typename tcoord > concept is_coord_size2		= is_coord<tcoord> and has_size2<tcoord> and !has_depth<tcoord> and !has_rect2<tcoord>;
	template < typename tcoord > concept is_coord_size3		= is_coord<tcoord> and has_size3<tcoord>						and !has_rect2<tcoord>;
	template < typename tcoord > concept is_coord_rect2		= is_coord<tcoord> and has_rect2<tcoord> and !has_rect3<tcoord>;
	template < typename tcoord > concept is_coord_rect3		= is_coord<tcoord> and has_rect3<tcoord>;
	template < typename tcoord > concept generic_coord = has_point2<tcoord> or has_size2<tcoord>;

	//-----------------------------------------------------------------------------------------------------------------------------
	// for QT
	template < typename tcoord > concept has_qx			= requires (tcoord a) { {a.x()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_qy			= requires (tcoord a) { {a.y()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_qz			= requires (tcoord a) { {a.z()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_qw			= requires (tcoord a) { {a.w()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_qpoint2	= has_qx<tcoord> and has_qy<tcoord>;
	template < typename tcoord > concept has_qpoint3	= has_qx<tcoord> and has_qy<tcoord> and has_qz<tcoord>;
	template < typename tcoord > concept has_qpoint4	= has_qx<tcoord> and has_qy<tcoord> and has_qz<tcoord> and has_qw<tcoord>;
	template < typename tcoord > concept has_qwidth		= requires (tcoord a) { {a.width() } -> std::convertible_to<double>; };
	template < typename tcoord > concept has_qheight	= requires (tcoord a) { {a.height()} -> std::convertible_to<double>; };
	template < typename tcoord > concept has_qdepth		= requires (tcoord a) { {a.depth() } -> std::convertible_to<double>; };
	template < typename tcoord > concept has_qsize2		= has_qwidth<tcoord> and has_qheight<tcoord>;
	template < typename tcoord > concept has_qsize3		= has_qwidth<tcoord> and has_qheight<tcoord> and has_qdepth<tcoord>;
	template < typename tcoord > concept has_qrect2		= has_qpoint2<tcoord> and has_qsize2<tcoord>;
	template < typename tcoord > concept has_qrect3		= has_qpoint3<tcoord> and has_qsize3<tcoord>;
	// all concepts is_q... are loosely defined.
	template < typename tcoord > concept is_qpoint2		= has_qpoint2<tcoord> and !has_qz<tcoord> and !has_qw<tcoord> and !has_qrect2<tcoord>;
	template < typename tcoord > concept is_qpoint3		= has_qpoint3<tcoord> and !has_qw<tcoord>					  and !has_qrect2<tcoord>;
	template < typename tcoord > concept is_qpoint4		= has_qpoint4<tcoord>										  and !has_qrect2<tcoord>;
	template < typename tcoord > concept is_qsize2		= has_qsize2<tcoord> and !has_qdepth<tcoord> and !has_qrect2<tcoord>;
	template < typename tcoord > concept is_qsize3		= has_qsize3<tcoord>						 and !has_qrect2<tcoord>;
	template < typename tcoord > concept is_qrect2		= has_qrect2<tcoord> and !has_qrect3<tcoord>;
	template < typename tcoord > concept is_qrect3		= has_qrect3<tcoord>;

	template < typename tcoord > concept generic_qcoord = has_qpoint2<tcoord> or has_qsize2<tcoord>;
}

